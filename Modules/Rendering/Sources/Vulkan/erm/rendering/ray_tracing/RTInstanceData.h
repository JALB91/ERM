#pragma once

#include <erm/math/math.h>

namespace erm {
class RTBlas;
}

namespace erm {

class RTInstanceData
{
public:
	RTInstanceData(
		RTBlas* blas,
		const math::mat4& transform);
	~RTInstanceData();

	RTInstanceData(RTInstanceData&& other);
	RTInstanceData(const RTInstanceData&) = delete;

	RTInstanceData& operator=(RTInstanceData&& other);
	RTInstanceData& operator=(const RTInstanceData&) = delete;

	inline RTBlas* GetBlas() const { return mBlas; }
	inline const math::mat4& GetTransform() const { return mTransform; }

	void Update(RTBlas* blas, const math::mat4& transform);

private:
	RTBlas* mBlas;
	math::mat4 mTransform;
};

} // namespace erm
