#pragma once

#include <chrono>
struct Timer
{
	using Clock = std::chrono::high_resolution_clock;
	std::chrono::steady_clock::time_point start;
	std::chrono::duration<double> duration;

	Timer()
	{
		start = Clock::now();
	}

	void Stop()
	{
		duration = Clock::now() - start;
	}

	inline double ResultMs() const
	{
		return duration.count() * 1000.0;
	}

	double Now()
	{
		duration = Clock::now() - start;
		return duration.count();
	}

	double NowMs()
	{
		duration = Clock::now() - start;
		return duration.count() * 1000.0;
	}

	void Reset()
	{
		duration = std::chrono::duration<double>();
		start = Clock::now();
	}
};
	
inline std::ostream& operator << (std::ostream& out, const Timer& t)
{
	out << t.ResultMs() << " ms";
	return out;
}