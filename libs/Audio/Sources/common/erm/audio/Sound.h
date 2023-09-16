#pragma once

#include <string>

namespace FMOD {
class Sound;
}

namespace erm {

class Sound
{
public:
	friend class AudioManager;

public:
	Sound(
		const char* path,
		FMOD::Sound* sound,
		bool isStream);
	virtual ~Sound();

	inline const std::string& GetPath() const { return mPath; }
	inline bool IsStream() const { return mIsStream; }

	const char* GetName() const;
	unsigned int GetLenght() const;

protected:
	std::string mPath;
	FMOD::Sound* mSound;
	bool mIsStream;
};

} // namespace erm
