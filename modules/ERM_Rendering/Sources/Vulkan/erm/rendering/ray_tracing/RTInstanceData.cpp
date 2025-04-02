#include "erm/rendering/ray_tracing/RTInstanceData.h"

namespace erm {

RTInstanceData::RTInstanceData(
	RTBlas* blas,
	const mat4& transform)
{
	update(blas, transform);
}

RTInstanceData::~RTInstanceData() = default;

RTInstanceData::RTInstanceData(RTInstanceData&& other)
	: mBlas(other.mBlas)
	, mTransform(std::move(other.mTransform))
{}

RTInstanceData& RTInstanceData::operator=(RTInstanceData&& other)
{
	if (this == &other)
	{
		return *this;
	}

	mBlas = other.mBlas;
	mTransform = std::move(other.mTransform);

	return *this;
}

void RTInstanceData::update(
	RTBlas* blas,
	const mat4& transform)
{
	mBlas = blas;
	mTransform = transform;
}

} // namespace erm
