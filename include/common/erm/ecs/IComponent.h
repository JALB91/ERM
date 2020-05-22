#pragma once

namespace erm::ecs {
	
#define SENSIBLE_MEMBER(NAME, TYPE, VARIABLE) \
	inline void Set##NAME(TYPE type) \
	{ \
		if (type == VARIABLE) return; \
		VARIABLE = type; \
		SetDirty(true); \
	} \
	inline TYPE Get##NAME() const { return VARIABLE; }
	
	struct IComponent
	{
	public:
		IComponent()
			: mIsDirty(true)
		{}
		virtual ~IComponent() = default;
		
		inline void SetDirty(bool isDirty) { mIsDirty = isDirty; }
		inline bool IsDirty() const { return mIsDirty; }
		
	private:
		bool mIsDirty;
		
	};
	
}
