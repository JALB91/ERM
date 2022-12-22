#pragma once

#include "erm/utils/Timer.h"

#include <array>
#include <numeric>

namespace erm {

class Sampler
{
public:
	Sampler()
		: mSamples{0.0}
		, mSampleIndex(0)
	{}
	
	inline void AddToCurrentSample(double time)
	{
		if (time <= std::numeric_limits<double>::epsilon())
			return;
		
		mSamples[mSampleIndex] += time;
	}
	
	inline void AddSample(double time)
	{
		if (time <= std::numeric_limits<double>::epsilon())
			return;
		
		mSampleIndex = (mSampleIndex + 1) % static_cast<short>(mSamples.size());
		mSamples[mSampleIndex] = time;
	}
	
	inline double GetAverage() const
	{
		const double total = std::accumulate(mSamples.cbegin(), mSamples.cend(), 0.0);
		return total / static_cast<double>(mSamples.size());
	}

private:
	std::array<double, 20> mSamples;
	short mSampleIndex;
	
};

extern Timer gSimTimer;
extern Sampler gSimSampler;

extern Timer gRenderTimer;
extern Sampler gRenderSampler;

}

