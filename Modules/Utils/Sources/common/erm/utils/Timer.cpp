#include "erm/utils/Timer.h"

using namespace std::chrono;

namespace erm {

short Timer::sFrameId = 0;

Timer::Timer()
	: mElapsedTime(0.0)
	, mStartingTime(GetCurrentTime())
	, mCurrentUpdateTime(mStartingTime)
	, mPreviousUpdateTime(mStartingTime)
	, mUpdateElapsedTime(0.0)
{}

void Timer::Update(double dt /*= GetCurrentTime()*/)
{
	mPreviousUpdateTime = mCurrentUpdateTime;
	mCurrentUpdateTime = dt;
	
	mUpdateElapsedTime = mCurrentUpdateTime - mPreviousUpdateTime;

	mElapsedTime += mUpdateElapsedTime;
}

void Timer::Restart()
{
	mElapsedTime = 0.0;
	mStartingTime = GetCurrentTime();
	mCurrentUpdateTime = mStartingTime;
	mPreviousUpdateTime = mStartingTime;
	mUpdateElapsedTime = 0.0;
}

} // namespace erm
