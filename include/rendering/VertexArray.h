#pragma once

namespace erm {
	
	class VertexBuffer;
	class VertexBufferLayout;
	
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();
		
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbl) const;
		
		void Bind() const;
		void Unbind() const;
		
	private:
		unsigned int mRendererId;
		
	};
	
}
