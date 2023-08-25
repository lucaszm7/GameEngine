#pragma once

#include <chrono>

struct Timer
{
	using Clock = std::chrono::high_resolution_clock;
	std::chrono::steady_clock::time_point start;
	std::chrono::duration<double> elapsed_time;

	Timer();
	void stop();
	double duration() const;
	double duration_ms() const;
	double duration_now();
	double duration_ms_now();
	void reset_hard();
	void reset_soft();
};
