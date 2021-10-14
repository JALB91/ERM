#include "erm/utils/Timer.h"

using namespace std::chrono;

namespace erm {

short Timer::sFrameId = 0;

Timer::Timer()
	: mElapsedTime(0.0)
	, mStartingTime(GetCurrentTime())
	, mCurrentUpdateTime(mStartingTime)
	, mPreviousUpdateTime(mStartingTime)
	, mSystemClock(clock())
	, mAppClock(0)
{}

void Timer::Update()
{
	mPreviousUpdateTime = mCurrentUpdateTime;
	mCurrentUpdateTime = GetCurrentTime();

	mElapsedTime += GetUpdateElapsedTime();

	mSystemClock = clock();
	++mAppClock;
}

void Timer::Update(float dt)
{
	mPreviousUpdateTime = mCurrentUpdateTime;
	mCurrentUpdateTime = dt;

	mElapsedTime += dt;

	mSystemClock = clock();
	++mAppClock;
}

void Timer::Restart()
{
	mElapsedTime = 0.0f;
	mStartingTime = GetCurrentTime();
	mCurrentUpdateTime = mStartingTime;
	mPreviousUpdateTime = mStartingTime;
	mSystemClock = clock();
	mAppClock = 0;
}

} // namespace erm
