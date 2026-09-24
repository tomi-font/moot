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

static GlobalClock::Ticks getRequestedPrintInterval()
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
	static_assert(GlobalClock::TicksAreMicroseconds);
	return static_cast<GlobalClock::Ticks>(seconds * 1e6);
}

SystemProfiler::SystemProfiler() :
	m_aggregateRows(AggregateCount),
	m_printIntervalTicks(getRequestedPrintInterval()),
	m_frameBeginTicks(0),
	m_frameSystemsTicks(0),
	m_lastFrameEndTicks(0),
	m_intervalStartTicks(0),
	m_profiling(false),
	m_printRequested(false)
{
	m_aggregateRows[AllSystems] = {"all systems"};
	m_aggregateRows[OutsideSystems] = {"outside systems"};
	m_aggregateRows[Frame] = {"frame"};
	m_aggregateRows[FrameAndVsync] = {"frame + vsync"};

	if (m_printIntervalTicks)
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
	m_intervalStartTicks = GlobalClock::ticksSinceStart();
	m_frameBeginTicks = 0;
	m_lastFrameEndTicks = 0;
}

void SystemProfiler::beginFrame()
{
	if (!m_profiling)
		return;

	m_frameBeginTicks = GlobalClock::ticksSinceStart();
}

void SystemProfiler::recordSystemUpdate(std::size_t systemIndex, GlobalClock::Ticks duration)
{
	if (!m_frameBeginTicks)
		return;

	m_systemRows[systemIndex].samples.push_back(duration);
	m_frameSystemsTicks += duration;
}

void SystemProfiler::endFrame()
{
	if (!m_profiling)
		return;

	const GlobalClock::Ticks now = GlobalClock::ticksSinceStart();
	const GlobalClock::Ticks frameTicks = now - m_frameBeginTicks;

	if (!m_frameBeginTicks)
	{
		// Profiling requested mid-frame; start it on the next one.
		m_intervalStartTicks = now;
		m_lastFrameEndTicks = now;
		return;
	}

	m_aggregateRows[AllSystems].samples.push_back(m_frameSystemsTicks);
	m_aggregateRows[Frame].samples.push_back(frameTicks);
	m_aggregateRows[OutsideSystems].samples.push_back(frameTicks - m_frameSystemsTicks);
	if (m_lastFrameEndTicks)
		m_aggregateRows[FrameAndVsync].samples.push_back(now - m_lastFrameEndTicks);

	m_frameSystemsTicks = 0;
	m_lastFrameEndTicks = now;

	const GlobalClock::Ticks intervalTicks = now - m_intervalStartTicks;
	if (m_printRequested || (m_printIntervalTicks && intervalTicks >= m_printIntervalTicks))
	{
		print(intervalTicks);
		m_printRequested = false;
		m_intervalStartTicks = now;
	}
}

static void printRow(SystemProfiler::Row* row, std::size_t nameWidth)
{
	std::vector<GlobalClock::Ticks>& samples = row->samples;
	if (samples.empty())
		return; // A row that skipped a frame has none in a one-frame interval.

	GlobalClock::Ticks min = std::numeric_limits<GlobalClock::Ticks>::max();
	GlobalClock::Ticks max = 0;
	GlobalClock::Ticks mean = 0;
	for (const GlobalClock::Ticks sample : samples)
	{
		min = std::min(min, sample);
		max = std::max(max, sample);
		mean += sample;
	}
	mean /= samples.size();

	static_assert(GlobalClock::TicksAreMicroseconds);
	const auto ms = [](GlobalClock::Ticks ticks) { return double(ticks) / 1e3; };

	std::println("  {:<{}} {:>9.3f} {:>9.3f} {:>9.3f}",
	             row->name, nameWidth, ms(min), ms(mean), ms(max));

	samples.clear();
}

void SystemProfiler::print(GlobalClock::Ticks intervalTicks)
{
	std::size_t nameWidth = 0;
	for (const Row& row : m_systemRows)
		nameWidth = std::max(nameWidth, row.name.size());
	for (const Row& row : m_aggregateRows)
		nameWidth = std::max(nameWidth, row.name.size());

	std::println("\nprofiling over {:.3f}s ({} frames), in ms", double(intervalTicks) / 1e6, m_aggregateRows[AllSystems].samples.size());
	std::println("  {:<{}} {:>9} {:>9} {:>9}", "", nameWidth, "min", "mean", "max");

	for (Row& row : m_systemRows)
		printRow(&row, nameWidth);
	for (Row& row : m_aggregateRows)
		printRow(&row, nameWidth);
}
