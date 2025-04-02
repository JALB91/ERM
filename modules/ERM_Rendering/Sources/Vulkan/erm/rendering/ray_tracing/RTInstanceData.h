#pragma once

#include <erm/math/Types.h>

namespace erm {
class RTBlas;
}

namespace erm {

class RTInstanceData
{
public:
	RTInstanceData(
		RTBlas* blas,
		const mat4& transform);
	~RTInstanceData();

	RTInstanceData(RTInstanceData&& other);
	RTInstanceData(const RTInstanceData&) = delete;

	RTInstanceData& operator=(RTInstanceData&& other);
	RTInstanceData& operator=(const RTInstanceData&) = delete;

	inline RTBlas* getBlas() const { return mBlas; }
	inline const mat4& getTransform() const { return mTransform; }

	void update(RTBlas* blas, const mat4& transform);

private:
	RTBlas* mBlas;
	mat4 mTransform;
};

} // namespace erm
