#include "Time.h"


double Time::deltaTime()
{
	return Time::get().delta_time;
}

void Time::LogFrameStart()
{
	Time::get().start = std::chrono::system_clock::now();
}

void Time::LogFrameEnd()
{
	Time::get().end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = Time::get().end - Time::get().start;

	Time::get().delta_time = elapsed.count();
}
