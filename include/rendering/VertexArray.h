#pragma once

namespace erm {
	
	class VertexBuffer;
	class VertexBufferLayout;
	
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();
		
		VertexArray(VertexArray&&) = delete;
		VertexArray(const VertexArray&) = delete;
		
		VertexArray& operator=(VertexArray&&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;
		
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbl) const;
		
		void Bind() const;
		void Unbind() const;
		
	private:
		unsigned int mRendererId;
		
	};
	
}
