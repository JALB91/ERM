#include "erm/utils/Timer.h"

using namespace std::chrono;

namespace erm {

short Timer::sFrameId = 0;

Timer::Timer()
	: mElapsedTime(0.0)
	, mStartingTime(getCurrentTime())
	, mCurrentUpdateTime(mStartingTime)
	, mPreviousUpdateTime(mStartingTime)
	, mUpdateElapsedTime(0.0)
{}

void Timer::update(double dt /*= getCurrentTime()*/)
{
	mPreviousUpdateTime = mCurrentUpdateTime;
	mCurrentUpdateTime = dt;
	
	mUpdateElapsedTime = mCurrentUpdateTime - mPreviousUpdateTime;

	mElapsedTime += mUpdateElapsedTime;
}

void Timer::restart()
{
	mElapsedTime = 0.0;
	mStartingTime = getCurrentTime();
	mCurrentUpdateTime = mStartingTime;
	mPreviousUpdateTime = mStartingTime;
	mUpdateElapsedTime = 0.0;
}

} // namespace erm
