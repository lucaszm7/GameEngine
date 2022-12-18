#pragma once
#include <GL/glew.h>
#include <iostream>


// Index Buffer Object
class IndexBuffer
{
private:
	// ID of the buffer in the graphics card
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count, GLenum mode = GL_DYNAMIC_DRAW);
	~IndexBuffer();
	size_t Size() const;
	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; };
};