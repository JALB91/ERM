#pragma once

#include <erm/assets/data_structs/Bone.h>

#include <erm/math/Types.h>

namespace erm {

using UboId = u8;

struct IUbo
{};

struct UboMVPOnly : public IUbo
{
	static constexpr UboId ID = 0;

	alignas(16) mat4 mMVP;
};

struct UboModelViewProj : public IUbo
{
	static constexpr UboId ID = 1;

	alignas(16) mat4 mModel;
	alignas(16) mat4 mView;
	alignas(16) mat4 mProjection;
};

struct UboMaterial : public IUbo
{
	static constexpr UboId ID = 2;

	alignas(16) vec3 mAmbient;
	alignas(16) vec3 mDiffuse;
	alignas(16) vec3 mSpecular;
	alignas(4) float mShininess;
};

struct UboLight : public IUbo
{
	static constexpr UboId ID = 3;

	alignas(16) vec3 mPosition;
	alignas(16) vec3 mAmbient;
	alignas(16) vec3 mDiffuse;
	alignas(16) vec3 mSpecular;
};

struct UboView : public IUbo
{
	static constexpr UboId ID = 4;

	alignas(16) vec3 mPosition;
};

struct UboSkeleton : public IUbo
{
	static constexpr UboId ID = 5;

	alignas(16) mat4 mModel;
	alignas(16) mat4 mView;
	alignas(16) mat4 mProjection;
	alignas(16) mat4 mBonesTransforms[MAX_BONES];
};

struct UboPBMaterial : public IUbo
{
	static constexpr UboId ID = 6;

	alignas(16) vec3 mAlbedo;
	alignas(4) float mMetallic;
	alignas(4) float mRoughness;
	alignas(4) float mAO;
};

struct UboPBLight : public IUbo
{
	static constexpr UboId ID = 7;

	alignas(16) vec3 mPosition;
	alignas(16) vec3 mColor;
};

struct UboBonesDebug : public IUbo
{
	static constexpr UboId ID = 8;

	alignas(16) mat4 mBonesModels[MAX_BONES];
	alignas(16) mat4 mModel;
	alignas(16) mat4 mView;
	alignas(16) mat4 mProj;
};

struct UboCamera : public IUbo
{
	static constexpr UboId ID = 9;

	alignas(16) vec3 mPosition;
	alignas(4) float mZNear;
	alignas(4) float mZFar;
	alignas(4) float mFov;
};

#ifdef ERM_RAY_TRACING_ENABLED
struct UboRTBasic : public IUbo
{
	static constexpr UboId ID = 10;

	alignas(16) mat4 mProjInv;
	alignas(16) mat4 mViewInv;
};
#endif

} // namespace erm
