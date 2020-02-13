#pragma once

#include <chrono>
#include <ctime>

namespace erm {
	
	class Timer
	{
	public:
		Timer();
		
		void Update();
		
		inline double GetCurrentTime() const { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }
		inline double GetElapsedTime() const { return mStartingTime; }
		inline double GetFrameElapsedTime() const { return mCurrentFrameTime - mPreviousFrameTime; }
		
		inline clock_t GetSystemClock() const { return mSystemClock; }
		inline clock_t GetAppClock() const { return mAppClock; }
		
	private:
		const double mStartingTime;
		double mCurrentFrameTime;
		double mPreviousFrameTime;
		clock_t mSystemClock;
		clock_t mAppClock;
		
	};
	
}
