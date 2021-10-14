#include "erm/audio/Reproduction.h"

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
	channel->getUserData(&data);
	Reproduction* repro = (Reproduction*)data;

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
	mChannel->setUserData(this);
	mChannel->setCallback(ChannelCallback);
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
	mChannel->setMode(mode);
}

FMOD_MODE Reproduction::GetMode() const
{
	FMOD_MODE mode = FMOD_2D;
	mChannel->getMode(&mode);

	return mode;
}

void Reproduction::SetLoopCount(LoopCount count) const
{
	mChannel->setLoopCount(static_cast<int>(count));
}

LoopCount Reproduction::GetLoopCount() const
{
	int count = 0;
	mChannel->getLoopCount(&count);

	return static_cast<LoopCount>(count);
}

void Reproduction::SetMuted(bool muted) const
{
	mChannel->setMute(muted);
}

bool Reproduction::IsMuted() const
{
	bool muted = false;
	mChannel->getMute(&muted);

	return muted;
}

void Reproduction::SetPosition(float position) const
{
	mChannel->setPosition(static_cast<unsigned int>(position * GetLenght()), FMOD_TIMEUNIT_MS);
}

float Reproduction::GetPosition() const
{
	unsigned int position = 0;
	mChannel->getPosition(&position, FMOD_TIMEUNIT_MS);

	return static_cast<float>(position) / static_cast<float>(GetLenght());
}

void Reproduction::SetVolume(float volume) const
{
	mChannel->setVolume(volume);
}

float Reproduction::GetVolume() const
{
	float volume = 0.0f;
	mChannel->getVolume(&volume);

	return volume;
}

void Reproduction::SetFrequency(float frequency) const
{
	mChannel->setFrequency(frequency);
}

float Reproduction::GetFrequency() const
{
	float frequency = 44100.0f;
	mChannel->getFrequency(&frequency);

	return frequency;
}

void Reproduction::SetPitch(float pitch) const
{
	mChannel->setPitch(pitch);
}

float Reproduction::GetPitch() const
{
	float pitch = 1.0f;
	mChannel->getPitch(&pitch);

	return pitch;
}

bool Reproduction::IsPaused() const
{
	bool isPaused = false;
	mChannel->getPaused(&isPaused);

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
	mChannel->setPaused(false);
}

void Reproduction::Pause() const
{
	mChannel->setPaused(true);
}

void Reproduction::Stop() const
{
	mChannel->stop();
}

} // namespace erm
