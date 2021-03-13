#include "erm/utils/Timer.h"

using namespace std::chrono;

namespace erm {

	short Timer::sFrameId = 0;

	Timer::Timer()
		: mStartingTime(GetCurrentTime())
		, mCurrentUpdateTime(mStartingTime)
		, mPreviousUpdateTime(mStartingTime)
		, mSystemClock(clock())
		, mAppClock(0)
	{}

	void Timer::Update()
	{
		mPreviousUpdateTime = mCurrentUpdateTime;
		mCurrentUpdateTime = GetCurrentTime();

		mSystemClock = clock();
		++mAppClock;
	}

} // namespace erm
