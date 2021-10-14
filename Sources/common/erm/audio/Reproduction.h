#pragma once

#include "erm/audio/Channel.h"
#include "erm/audio/LoopCount.h"
#include "erm/audio/Sound.h"

#include <fmod.hpp>

namespace erm {

class Reproduction
	: public Sound
	, public Channel
{
public:
	friend FMOD_RESULT ChannelCallback(
		FMOD_CHANNELCONTROL* channelcontrol,
		FMOD_CHANNELCONTROL_TYPE controltype,
		FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
		void* commanddata1,
		void* commanddata2);

public:
	Reproduction(
		Sound& sound,
		Channel&& channel);
	~Reproduction();

	float GetDuration() const;
	float GetTimeLeft() const;
	float GetElapsedTime() const;

	void SetMode(FMOD_MODE mode) const;
	FMOD_MODE GetMode() const;

	void SetLoopCount(LoopCount count) const;
	LoopCount GetLoopCount() const;

	void SetMuted(bool muted) const;
	bool IsMuted() const;

	void SetPosition(float position) const;
	float GetPosition() const;

	void SetVolume(float volume) const;
	float GetVolume() const;

	void SetFrequency(float frequency) const;
	float GetFrequency() const;

	void SetPitch(float pitch) const;
	float GetPitch() const;

	bool IsPaused() const;
	bool IsEnded() const;
	bool IsLooping() const;

	void Resume() const;
	void Pause() const;
	void Stop() const;

private:
	float mDuration;
	int mLoops;
};

} // namespace erm
