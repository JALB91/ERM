#pragma once

#include <fmod.hpp>

#include <string>

namespace erm {

struct AudioDriver
{
	int mId;
	std::string mName;
	FMOD_GUID mFmodGuid;
	int mSystemRate;
	FMOD_SPEAKERMODE mSpeakerMode;
	int mSpeakerModeChannels;
};

} // namespace erm
