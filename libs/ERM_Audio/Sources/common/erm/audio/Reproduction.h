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

	float getDuration() const;
	float getTimeLeft() const;
	float getElapsedTime() const;

	void setMode(FMOD_MODE mode) const;
	FMOD_MODE getMode() const;

	void setLoopCount(LoopCount count) const;
	LoopCount getLoopCount() const;

	void setMuted(bool muted) const;
	bool isMuted() const;

	void setPosition(float position) const;
	float getPosition() const;

	void setVolume(float volume) const;
	float getVolume() const;

	void setFrequency(float frequency) const;
	float getFrequency() const;

	void setPitch(float pitch) const;
	float getPitch() const;

	bool isPaused() const;
	bool isEnded() const;
	bool isLooping() const;
	bool isStopped() const;

	void resume() const;
	void pause() const;
	void stop() const;

private:
	float mDuration;
	unsigned int mLoops;
	mutable bool mStopped;

};

} // namespace erm
