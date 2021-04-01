#pragma once

#include <vector>

namespace erm {

struct VertexBufferElement
{
	unsigned int mType;
	unsigned int mCount;
	unsigned int mOffset;
	unsigned char mNormalized;
};

class VertexBufferLayout
{
public:
	VertexBufferLayout()
		: mStride(0)
	{}

	template<typename T>
	void Push(unsigned int count);

	inline const std::vector<VertexBufferElement>& GetElements() const { return mElements; }
	inline unsigned int GetStride() const { return mStride; }

private:
	std::vector<VertexBufferElement> mElements;
	unsigned int mStride;
};

} // namespace erm
