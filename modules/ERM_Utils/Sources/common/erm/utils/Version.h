#pragma once

namespace erm {

struct Version
{
    constexpr Version(int major, int minor, int patch)
        : mMajor(major)
        , mMinor(minor)
        , mPatch(patch)
    {}

    constexpr bool operator>=(const Version& version) const
    {
        if (mMajor > version.mMajor)
        {
            return true;
        }
        else if (mMajor == version.mMajor)
        {
            if (mMinor > version.mMinor)
            {
                return true;
            }
            else if (mMinor == version.mMinor)
            {
                return mPatch >= version.mPatch;
            }
        }
        return false;
    }

    int mMajor;
    int mMinor;
    int mPatch;
};

}