#pragma once

#include <moot/Event/User.hh>
#include <moot/Global/Clock.hh>
#include <cstddef>
#include <string>
#include <vector>

class SystemProfiler : public EventUser
{
public:

	SystemProfiler();

	void insertSystemRow(std::size_t index, std::string name);

	void beginFrame();
	void recordSystemUpdate(std::size_t systemIndex, GlobalClock::Ticks duration);
	void endFrame();

	struct Row
	{
		std::string name;
		std::vector<GlobalClock::Ticks> samples;
	};

private:

	void listenToEvents() override;
	void onEvent(const Event&) override;

	void startProfiling();
	void print(GlobalClock::Ticks intervalTicks);

	std::vector<Row> m_systemRows;
	std::vector<Row> m_aggregateRows;

	const GlobalClock::Ticks m_printIntervalTicks;
	GlobalClock::Ticks m_frameBeginTicks;
	GlobalClock::Ticks m_frameSystemsTicks;
	GlobalClock::Ticks m_lastFrameEndTicks;
	GlobalClock::Ticks m_intervalStartTicks;

	bool m_profiling;
	bool m_printRequested;
};
