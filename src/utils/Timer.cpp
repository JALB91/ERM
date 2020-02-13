#include "utils/Timer.h"

using namespace std::chrono;

namespace erm {
	
	Timer::Timer()
		: mStartingTime(GetCurrentTime())
		, mCurrentFrameTime(mStartingTime)
		, mPreviousFrameTime(mStartingTime)
		, mSystemClock(clock())
		, mAppClock(0)
	{}
	
	void Timer::Update()
	{
		mPreviousFrameTime = mCurrentFrameTime;
		mCurrentFrameTime = GetCurrentTime();
		
		mSystemClock = clock();
		++mAppClock;
	}
	
}
