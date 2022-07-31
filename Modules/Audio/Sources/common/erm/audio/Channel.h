#pragma once

namespace FMOD {
class Channel;
}

namespace erm {

class Channel
{
public:
	friend class AudioManager;

public:
	virtual ~Channel();

	bool IsPlaying() const;
	const char* GetCurrentSoundName() const;

protected:
	Channel(FMOD::Channel* channel = nullptr);

	FMOD::Channel* mChannel;
};

} // namespace erm
