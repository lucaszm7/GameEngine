#include "VertexBuffer.h"
#include <iostream>

VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum mode)
{
    // Gen a buffer in the graphics card, and return the ID of that buffer
    glGenBuffers(1, &m_RendererID);
    // std::cout << "Creating Vertex Buffer - " << m_RendererID << "\n";
    // Tell the OpenGL what type is that buffer and selected
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    // As we know the data we want, we can provide with data right way
    glBufferData(GL_ARRAY_BUFFER, size, data, mode);
}

VertexBuffer::~VertexBuffer()
{
    std::cout << "Deleting Vertex Buffer - " << m_RendererID << "\n";
    Unbind();
    glDeleteBuffers(1, &m_RendererID);
}

size_t VertexBuffer::Size() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLint size = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    return size;
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

