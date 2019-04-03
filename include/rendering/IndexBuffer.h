#pragma once

namespace erm {

	class IndexBuffer
	{
	public:
		IndexBuffer(const unsigned int* data, unsigned int count);
		~IndexBuffer();
		
		void Bind() const;
		void Unbind() const;
		
		inline unsigned int GetCount() const { return mCount; }
		
	private:
		const unsigned int mCount;
		unsigned int mRendererId;
		
	};
	
}
