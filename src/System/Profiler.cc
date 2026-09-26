#include <moot/System/Profiler.hh>
#include <moot/Event/Engine.hh>
#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <print>
#include <string_view>
#include <system_error>

// The rows after the systems'.
enum Aggregate
{
	AllSystems,
	OutsideSystems,
	Frame,
	FrameAndVsync,
	AggregateCount
};

static constexpr const char* ProfilingIntervalEnvName = "MOOT_PROFILING_INTERVAL";

static GlobalClock::Microseconds getRequestedPrintInterval()
{
	const char* const value = std::getenv(ProfilingIntervalEnvName);
	if (!value)
		return 0;

	const std::string_view text = value;
	double seconds = 0;
	const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), seconds);
	if (error != std::errc() || end != text.data() + text.size() || !(seconds > 0))
	{
		std::println(stderr, "{}: expected an interval in seconds, got \"{}\"", ProfilingIntervalEnvName, text);
		return 0;
	}
	return static_cast<GlobalClock::Microseconds>(seconds * 1e6);
}

SystemProfiler::SystemProfiler() :
	m_aggregateRows(AggregateCount),
	m_printInterval(getRequestedPrintInterval()),
	m_frameBegin(0),
	m_frameSystemsDuration(0),
	m_lastFrameEnd(0),
	m_intervalStart(0),
	m_profiling(false),
	m_printRequested(false)
{
	m_aggregateRows[AllSystems] = {"all systems"};
	m_aggregateRows[OutsideSystems] = {"outside systems"};
	m_aggregateRows[Frame] = {"frame"};
	m_aggregateRows[FrameAndVsync] = {"frame + vsync"};

	if (m_printInterval)
		startProfiling();
}

void SystemProfiler::listenToEvents()
{
	listenTo(EngineEvent::ProfilingRequest);
}

void SystemProfiler::onEvent(const Event& event)
{
	assert(event.id == EngineEvent::ProfilingRequest);
	if (m_profiling)
	{
		m_printRequested = true;
	}
	else
	{
		startProfiling();
		std::println("profiling started");
	}
}

void SystemProfiler::insertSystemRow(std::size_t index, std::string name)
{
	m_systemRows.emplace(m_systemRows.begin() + index, std::move(name));
}

void SystemProfiler::startProfiling()
{
	m_profiling = true;
	m_intervalStart = GlobalClock::microsecondsSinceStart();
	m_frameBegin = 0;
	m_lastFrameEnd = 0;
}

void SystemProfiler::beginFrame()
{
	if (!m_profiling)
		return;

	m_frameBegin = GlobalClock::microsecondsSinceStart();
}

void SystemProfiler::recordSystemUpdate(std::size_t systemIndex, GlobalClock::Microseconds duration)
{
	if (!m_frameBegin)
		return;

	m_systemRows[systemIndex].samples.push_back(duration);
	m_frameSystemsDuration += duration;
}

void SystemProfiler::endFrame()
{
	if (!m_profiling)
		return;

	const GlobalClock::Microseconds now = GlobalClock::microsecondsSinceStart();
	const GlobalClock::Microseconds frameDuration = now - m_frameBegin;

	if (!m_frameBegin)
	{
		// Profiling requested mid-frame; start it on the next one.
		m_intervalStart = now;
		m_lastFrameEnd = now;
		return;
	}

	m_aggregateRows[AllSystems].samples.push_back(m_frameSystemsDuration);
	m_aggregateRows[Frame].samples.push_back(frameDuration);
	m_aggregateRows[OutsideSystems].samples.push_back(frameDuration - m_frameSystemsDuration);
	if (m_lastFrameEnd)
		m_aggregateRows[FrameAndVsync].samples.push_back(now - m_lastFrameEnd);

	m_frameSystemsDuration = 0;
	m_lastFrameEnd = now;

	const GlobalClock::Microseconds intervalDuration = now - m_intervalStart;
	if (m_printRequested || (m_printInterval && intervalDuration >= m_printInterval))
	{
		print(intervalDuration);
		m_printRequested = false;
		m_intervalStart = now;
	}
}

static void printRow(SystemProfiler::Row* row, std::size_t nameWidth)
{
	std::vector<GlobalClock::Microseconds>& samples = row->samples;
	if (samples.empty())
		return; // A row that skipped a frame has none in a one-frame interval.

	GlobalClock::Microseconds min = std::numeric_limits<GlobalClock::Microseconds>::max();
	GlobalClock::Microseconds max = 0;
	GlobalClock::Microseconds mean = 0;
	for (const GlobalClock::Microseconds sample : samples)
	{
		min = std::min(min, sample);
		max = std::max(max, sample);
		mean += sample;
	}
	mean /= samples.size();

	const auto ms = [](GlobalClock::Microseconds duration) { return double(duration) / 1e3; };

	std::println("  {:<{}} {:>9.3f} {:>9.3f} {:>9.3f}",
	             row->name, nameWidth, ms(min), ms(mean), ms(max));

	samples.clear();
}

void SystemProfiler::print(GlobalClock::Microseconds intervalDuration)
{
	std::size_t nameWidth = 0;
	for (const Row& row : m_systemRows)
		nameWidth = std::max(nameWidth, row.name.size());
	for (const Row& row : m_aggregateRows)
		nameWidth = std::max(nameWidth, row.name.size());

	std::println("\nprofiling over {:.3f}s ({} frames), in ms", double(intervalDuration) / 1e6, m_aggregateRows[AllSystems].samples.size());
	std::println("  {:<{}} {:>9} {:>9} {:>9}", "", nameWidth, "min", "mean", "max");

	for (Row& row : m_systemRows)
		printRow(&row, nameWidth);
	for (Row& row : m_aggregateRows)
		printRow(&row, nameWidth);
}
