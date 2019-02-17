#pragma once

namespace erm {

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		~VertexBuffer();
		
		void Bind() const;
		void Unbind() const;
		
	private:
		unsigned int mRendererId;
		
	};
	
}
