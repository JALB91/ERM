#include "erm/audio/Reproduction.h"

#include "erm/audio/AudioUtils.h"

#include "erm/engine/Engine.h"

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

	FMOD::Channel* channel = (FMOD::Channel*)channelcontrol;
	void* data;
	ERM_CHECK_FMOD_RESULT(channel->getUserData(&data));
	Reproduction* repro = static_cast<Reproduction*>(data);

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
			break;
	}

	return FMOD_OK;
}

Reproduction::Reproduction(
	Sound& sound,
	Channel&& channel)
	: Sound(sound)
	, Channel(channel)
	, mDuration(static_cast<float>(GetLenght()) / 1000.0f)
	, mLoops(0)
{
	ERM_CHECK_FMOD_RESULT(mChannel->setUserData(this));
	ERM_CHECK_FMOD_RESULT(mChannel->setCallback(ChannelCallback));
}

Reproduction::~Reproduction()
{}

float Reproduction::GetDuration() const
{
	return mDuration;
}

float Reproduction::GetTimeLeft() const
{
	return mDuration - (mDuration * GetPosition());
}

float Reproduction::GetElapsedTime() const
{
	return mDuration * GetPosition();
}

void Reproduction::SetMode(FMOD_MODE mode) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setMode(mode));
}

FMOD_MODE Reproduction::GetMode() const
{
	FMOD_MODE mode = FMOD_2D;
	ERM_CHECK_FMOD_RESULT(mChannel->getMode(&mode));
	return mode;
}

void Reproduction::SetLoopCount(LoopCount count) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setLoopCount(static_cast<int>(count)));
}

LoopCount Reproduction::GetLoopCount() const
{
	int count = 0;
	ERM_CHECK_FMOD_RESULT(mChannel->getLoopCount(&count));
	return static_cast<LoopCount>(count);
}

void Reproduction::SetMuted(bool muted) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setMute(muted));
}

bool Reproduction::IsMuted() const
{
	bool muted = false;
	ERM_CHECK_FMOD_RESULT(mChannel->getMute(&muted));
	return muted;
}

void Reproduction::SetPosition(float position) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPosition(static_cast<unsigned int>(position * GetLenght()), FMOD_TIMEUNIT_MS));
}

float Reproduction::GetPosition() const
{
	unsigned int position = 0;
	ERM_CHECK_FMOD_RESULT(mChannel->getPosition(&position, FMOD_TIMEUNIT_MS));
	return static_cast<float>(position) / static_cast<float>(GetLenght());
}

void Reproduction::SetVolume(float volume) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setVolume(volume));
}

float Reproduction::GetVolume() const
{
	float volume = 0.0f;
	ERM_CHECK_FMOD_RESULT(mChannel->getVolume(&volume));
	return volume;
}

void Reproduction::SetFrequency(float frequency) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setFrequency(frequency));
}

float Reproduction::GetFrequency() const
{
	float frequency = 44100.0f;
	ERM_CHECK_FMOD_RESULT(mChannel->getFrequency(&frequency));
	return frequency;
}

void Reproduction::SetPitch(float pitch) const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPitch(pitch));
}

float Reproduction::GetPitch() const
{
	float pitch = 1.0f;
	ERM_CHECK_FMOD_RESULT(mChannel->getPitch(&pitch));
	return pitch;
}

bool Reproduction::IsPaused() const
{
	bool isPaused = false;
	ERM_CHECK_FMOD_RESULT(mChannel->getPaused(&isPaused));
	return isPaused;
}

bool Reproduction::IsEnded() const
{
	return GetTimeLeft() <= 0.0f && !IsLooping();
}

bool Reproduction::IsLooping() const
{
	bool looping = false;

	switch (GetLoopCount())
	{
		case LoopCount::FOREVER:
			looping = true;
			break;
		case LoopCount::NO_LOOP:
			looping = false;
			break;
		case LoopCount::ONCE:
			looping = mLoops <= 1;
			break;
	}

	return looping && GetMode() & FMOD_LOOP_NORMAL;
}

void Reproduction::Resume() const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPaused(false));
}

void Reproduction::Pause() const
{
	ERM_CHECK_FMOD_RESULT(mChannel->setPaused(true));
}

void Reproduction::Stop() const
{
	ERM_CHECK_FMOD_RESULT(mChannel->stop());
}

} // namespace erm
