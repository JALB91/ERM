#pragma once

namespace erm {

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		~VertexBuffer();
		
		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer(const VertexBuffer&) = delete;
		
		VertexBuffer& operator=(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		
		void Bind() const;
		void Unbind() const;
		
	private:
		unsigned int mRendererId;
		
	};
	
}
