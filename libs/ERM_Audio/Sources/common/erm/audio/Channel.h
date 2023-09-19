#pragma once

#include <erm/utils/StaticString.h>

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

	bool isPlaying() const;
	str128 getCurrentSoundName() const;

protected:
	Channel(FMOD::Channel* channel = nullptr);

	FMOD::Channel* mChannel;
};

} // namespace erm
