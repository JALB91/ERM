#pragma once

#include "erm/math/mat.h"
#include "erm/math/vec.h"

namespace erm {

	typedef uint8_t UboId;

	struct IUbo
	{};

	struct UboBasic : public IUbo
	{
		static constexpr UboId ID = 0;

		alignas(16) math::mat4 mMVP;
	};

	struct UboModelViewProj : public IUbo
	{
		static constexpr UboId ID = 1;

		alignas(16) math::mat4 mModel;
		alignas(16) math::mat4 mViewProj;
	};

	struct UboMaterial : public IUbo
	{
		static constexpr UboId ID = 2;

		alignas(16) math::vec3 mAmbient;
		alignas(16) math::vec3 mDiffuse;
		alignas(16) math::vec3 mSpecular;
		alignas(16) float mShininess;
	};

	struct UboLight : public IUbo
	{
		static constexpr UboId ID = 3;

		alignas(16) math::vec3 mPosition;
		alignas(16) math::vec3 mAmbient;
		alignas(16) math::vec3 mDiffuse;
		alignas(16) math::vec3 mSpecular;
	};

	struct UboView : public IUbo
	{
		static constexpr UboId ID = 4;

		alignas(16) math::vec3 mPosition;
	};

} // namespace erm
