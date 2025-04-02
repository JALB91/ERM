#include "erm/audio/Reproduction.h"

#include "erm/audio/AudioUtils.h"

#include <fmod.hpp>

namespace erm {

FMOD_RESULT ChannelCallback(
	FMOD_CHANNELCONTROL* channelcontrol,
	FMOD_CHANNELCONTROL_TYPE controltype,
	FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
	void* /*commanddata1*/,
	void* /*commanddata2*/)
{
	if (controltype != FMOD_CHANNELCONTROL_CHANNEL &&
		callbacktype != FMOD_CHANNELCONTROL_CALLBACK_END &&
		callbacktype != FMOD_CHANNELCONTROL_CALLBACK_SYNCPOINT)
	{
		return FMOD_OK;
	}

	auto* channel = (FMOD::Channel*)channelcontrol;
	void* data;
	ERM_CHECK_FMOD_RESULT(channel->getUserData(&data));
	auto* repro = static_cast<Reproduction*>(data);

	switch (callbacktype)
	{
		case FMOD_CHANNELCONTROL_CALLBACK_END:
		{
			break;
		}
		case FMOD_CHANNELCONTROL_CALLBACK_SYNCPOINT:
		{
			repro->mLoops++;
			break;
		}
		default:
		{
			break;
		}
	}

	return FMOD_OK;
}

Reproduction::Reproduction(
	Sound& sound,
	Channel&& channel)
	: Sound(sound)
	, Channel(channel)
	, mDuration(static_cast<float>(getLenght()) / 1000.0f)
	, mLoops(0)
	, mStopped(false)
{
	ERM_CHECK_FMOD_RESULT(mChannel->setUserData(this));
	ERM_CHECK_FMOD_RESULT(mChannel->setCallback(ChannelCallback));
}

Reproduction::~Reproduction()
{}

float Reproduction::getDuration() const
{
	return mDuration;
}

float Reproduction::getTimeLeft() const
{
	return mDuration - (mDuration * getPosition());
}

float Reproduction::getElapsedTime() const
{
	return mDuration * getPosition();
}

void Reproduction::setMode(FMOD_MODE mode) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setMode(mode));
}

FMOD_MODE Reproduction::getMode() const
{
	FMOD_MODE mode = FMOD_2D;
	ERM_CHECK_FMOD_RESULT(mChannel->getMode(&mode));
	return mode;
}

void Reproduction::setLoopCount(LoopCount count) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setLoopCount(static_cast<int>(count)));
}

LoopCount Reproduction::getLoopCount() const
{
	int count = 0;
	ERM_CHECK_FMOD_RESULT(mChannel->getLoopCount(&count));
	return static_cast<LoopCount>(count);
}

void Reproduction::setMuted(bool muted) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setMute(muted));
}

bool Reproduction::isMuted() const
{
	bool muted = false;
	ERM_CHECK_FMOD_RESULT(mChannel->getMute(&muted));
	return muted;
}

void Reproduction::setPosition(float position) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPosition(static_cast<unsigned int>(position * getLenght()), FMOD_TIMEUNIT_MS));
}

float Reproduction::getPosition() const
{
	unsigned int position = 0;
	ERM_CHECK_FMOD_RESULT(mChannel->getPosition(&position, FMOD_TIMEUNIT_MS));
	return static_cast<float>(position) / static_cast<float>(getLenght());
}

void Reproduction::setVolume(float volume) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setVolume(volume));
}

float Reproduction::getVolume() const
{
	float volume = 0.0f;
	ERM_CHECK_FMOD_RESULT(mChannel->getVolume(&volume));
	return volume;
}

void Reproduction::setFrequency(float frequency) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setFrequency(frequency));
}

float Reproduction::getFrequency() const
{
	float frequency = 44100.0f;
	ERM_CHECK_FMOD_RESULT(mChannel->getFrequency(&frequency));
	return frequency;
}

void Reproduction::setPitch(float pitch) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPitch(pitch));
}

float Reproduction::getPitch() const
{
	float pitch = 1.0f;
	ERM_CHECK_FMOD_RESULT(mChannel->getPitch(&pitch));
	return pitch;
}

bool Reproduction::isPaused() const
{
	bool isPaused = false;
	ERM_CHECK_FMOD_RESULT(mChannel->getPaused(&isPaused));
	return isPaused;
}

bool Reproduction::isEnded() const
{
	return isStopped() || (getTimeLeft() <= 0.0f && !isLooping());
}

bool Reproduction::isLooping() const
{
	bool looping = false;

	switch (getLoopCount())
	{
		case LoopCount::FOREVER:
		{
			looping = true;
			break;
		}
		case LoopCount::NO_LOOP:
		{
			looping = false;
			break;
		}
		case LoopCount::ONCE:
		{
			looping = mLoops <= 1;
			break;
		}
	}

	return looping && getMode() & FMOD_LOOP_NORMAL;
}

bool Reproduction::isStopped() const
{
	return mStopped;
}

void Reproduction::resume() const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPaused(false));
}

void Reproduction::pause() const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPaused(true));
}

void Reproduction::stop() const
{
	ERM_CHECK_FMOD_RESULT(mChannel->stop());
	mStopped = true;
}

} // namespace erm
