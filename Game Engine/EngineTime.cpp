#include "Game Engine/EngineTime.h"


double EngineTime::deltaTime()
{
	return EngineTime::get().delta_time;
}

void EngineTime::LogFrameStart()
{
	EngineTime::get().start = std::chrono::system_clock::now();
}

void EngineTime::LogFrameEnd()
{
	EngineTime::get().end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = EngineTime::get().end - EngineTime::get().start;

	EngineTime::get().delta_time = elapsed.count();
}

std::string EngineTime::now()
{
	std::stringstream s;
	std::chrono::system_clock::time_point time = std::chrono::system_clock::now();

	std::chrono::time_point<std::chrono::system_clock, std::chrono::days> dp = std::chrono::floor<std::chrono::days>(time);
	std::chrono::year_month_day ymd = std::chrono::year_month_day{ dp };         
	std::chrono::hh_mm_ss hms{ time - dp };

	s << unsigned(ymd.day()) << unsigned(ymd.month()) << int(ymd.year());
	s << int(hms.hours().count()) << int(hms.minutes().count()) << int(hms.seconds().count());

	return s.str();
}
