#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count, GLenum mode)
    : m_Count(count)
{
    // Gen a buffer in the graphics card, and return the ID of that buffer
    glGenBuffers(1, &m_RendererID);
    // std::cout << "Creating Index Buffer - " << m_RendererID << "\n";
    // Tell the OpenGL what type is that buffer and selected
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    // As we know the data we want, we can provide with data right way
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, mode);
}

IndexBuffer::~IndexBuffer()
{
    std::cout << "Deleting Index Buffer - " << m_RendererID << "\n";
    Unbind();
    glDeleteBuffers(1, &m_RendererID);
}

size_t IndexBuffer::Size() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    return size;
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

