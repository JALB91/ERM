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

	void update(double dt = getCurrentTime());
	void restart();
	
	static double clockToSeconds(clock_t ticks)
	{
		return ticks / static_cast<double>(CLOCKS_PER_SEC);
	}
	
	static double getCurrentTime()
	{
		return clockToSeconds(clock());
	}
	
	inline double getElapsedTime() const { return mElapsedTime; }
	inline double getUpdateElapsedTime() const { return mUpdateElapsedTime; }

private:
	double mElapsedTime;
	double mStartingTime;
	double mCurrentUpdateTime;
	double mPreviousUpdateTime;
	double mUpdateElapsedTime;
	
};

} // namespace erm
