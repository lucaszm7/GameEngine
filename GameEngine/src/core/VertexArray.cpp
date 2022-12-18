#include "VertexArray.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
    // std::cout << "Creating Vertex Array - " << m_RendererID << "\n";
}

VertexArray::~VertexArray()
{
    Unbind();
    // std::cout << "Deleting Vertex Array - " << m_RendererID << "\n";
    glDeleteVertexArrays(1, &m_RendererID);
}

// We bind our VAO, bind our Buffer and set up our Layout
void VertexArray::AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout) const
{
    Bind();
	vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        const auto& element = elements[i];

        // We need to enable our vertexAttribArray
        glEnableVertexAttribArray(i);

        // The Vertex Attrib Pointer is what links this buffer to the current VAO!!!
        // 
        // 1. Witch attribute we want? (index)
        // 2. How many types build this attribute? Ex: 2 floats build a 2D position
        // 3. Witch type are this attribute? Ex: float, int, ...
        // 4. Are normalize? Ex: RGB(255, 0, 0) needs to pass to [0, 1]
        // 5. Is the size of the vertex, as the vertex has many attributes, we need
        //      to call vertex[2], so we need to know the size of witch vertex. 
        //      We can say that is the amount of bytes between each vertex
        // 6. Inside this Vertex, what is the offset for this attribute? 
        //      Ex: if a vertex has (position, color, textCoord)
        //      then the offset for 0, 12, 20;
        // 
        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glVertexAttribPointer(
            i,
            element.count,
            element.type,
            element.normalized,
            layout.GetStride(),
            (const void*)offset);

        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }

}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
