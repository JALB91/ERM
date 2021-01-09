#pragma once

#include <chrono>
#include <ctime>

namespace erm {

	class Timer
	{
	public:
		Timer();

		void Update();

		static double GetCurrentTime() { return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) * 0.001; }
		inline double GetElapsedTime() const { return GetCurrentTime() - mStartingTime; }
		inline double GetUpdateElapsedTime() const { return mCurrentUpdateTime - mPreviousUpdateTime; }

		inline clock_t GetSystemClock() const { return mSystemClock; }
		inline clock_t GetAppClock() const { return mAppClock; }

	private:
		const double mStartingTime;
		double mCurrentUpdateTime;
		double mPreviousUpdateTime;
		clock_t mSystemClock;
		clock_t mAppClock;
	};

} // namespace erm
