#include <Timer.hpp>

Timer::Timer()
	:elapsed_time()
{
	start = Clock::now();
}

void Timer::stop()
{
	elapsed_time = Clock::now() - start;
}

double Timer::duration() const
{
	return elapsed_time.count();
}

double Timer::duration_ms() const
{
	return elapsed_time.count() * 1000.0;
}

double Timer::duration_now()
{
	elapsed_time = Clock::now() - start;
	return elapsed_time.count();
}

double Timer::duration_ms_now()
{
	elapsed_time = Clock::now() - start;
	return elapsed_time.count() * 1000.0;
}

void Timer::reset_hard()
{
	elapsed_time = std::chrono::duration<double>();
	start = Clock::now();
}

void Timer::reset_soft()
{
	start = Clock::now();
}

std::ostream& operator << (std::ostream& out, const Timer& t)
{
	out << t.duration() << " ms";
	return out;
}