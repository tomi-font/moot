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
	void recordSystemUpdate(std::size_t systemIndex, GlobalClock::Microseconds duration);
	void endFrame();

	struct Row
	{
		std::string name;
		std::vector<GlobalClock::Microseconds> samples;
	};

private:

	void listenToEvents() override;
	void onEvent(const Event&) override;

	void startProfiling();
	void print(GlobalClock::Microseconds intervalDuration);

	std::vector<Row> m_systemRows;
	std::vector<Row> m_aggregateRows;

	const GlobalClock::Microseconds m_printInterval;
	GlobalClock::Microseconds m_frameBegin;
	GlobalClock::Microseconds m_frameSystemsDuration;
	GlobalClock::Microseconds m_lastFrameEnd;
	GlobalClock::Microseconds m_intervalStart;

	bool m_profiling;
	bool m_printRequested;
};
