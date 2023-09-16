#pragma once

#include <chrono>
#include <ctime>

namespace erm {

using FrameID = short;

class Timer
{
public:
	static FrameID sFrameId;

public:
	Timer();

	void Update(double dt = GetCurrentTime());
	void Restart();
	
	static double ClockToSeconds(clock_t ticks)
	{
		return ticks / static_cast<double>(CLOCKS_PER_SEC);
	}
	
	static double GetCurrentTime()
	{
		return ClockToSeconds(clock());
	}
	
	inline double GetElapsedTime() const { return mElapsedTime; }
	inline double GetUpdateElapsedTime() const { return mUpdateElapsedTime; }

private:
	double mElapsedTime;
	double mStartingTime;
	double mCurrentUpdateTime;
	double mPreviousUpdateTime;
	double mUpdateElapsedTime;
	
};

} // namespace erm
