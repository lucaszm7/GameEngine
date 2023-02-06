#pragma once
#include <iostream>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
/*
        Vertex Array Object (VAO)

        The whole goal of Vertex Array Objects is to tied up the vertex buffer
        and it's layout.

        In the current state (before VAO) what we are doing is :
            Bind our shader
            Bind our vertex Buffer
            Set up the vertex layout
            Bind our index buffer
            And then, call the DrawCall

        Now with Vertex Array Object (VAO):
            Bind our shader
            Bind our Vertex Array
            And then, call the DrawCall

        Vertex Array Objects are mandatory, what are happening is that
        OpenGL is creating one for us right now, because of the compatibility.
        But if we change the OpenGL profile from COMPATIBILITE to CORE, it's not
        gonna create one for us.
         */
class VertexArray
{
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;
	void Bind() const;
	void Unbind() const;
};