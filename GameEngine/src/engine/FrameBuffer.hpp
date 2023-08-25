#pragma once

#include <gl/GL.h>
#include <IMGUI/imgui.h>
#include <memory>
#include <Shader.h>
#include <Texture.h>

class FrameBuffer
{
private:

    enum class Type;

    Type m_Type;

	std::shared_ptr<unsigned int> pScreenWidth;
	std::shared_ptr<unsigned int> pScreenHeight;

	std::unique_ptr<Shader> m_FramebufferShader;

	unsigned int m_MultisampleFramebuffer;
    std::unique_ptr<Texture> m_MultisampleTextureColorBuffer;

    unsigned int m_SingleSampleFramebuffer;
    std::unique_ptr<Texture> m_SingleSampleTextureColorBuffer;

    VertexArray quadVAO;
    VertexBufferLayout quadVBL;
    std::unique_ptr<VertexBuffer> quadVBO;

public:
	FrameBuffer(std::shared_ptr<unsigned int> screenWidth, std::shared_ptr<unsigned int> screenHeight, FrameBuffer::Type type)
		:pScreenWidth(screenWidth), pScreenHeight(screenHeight), m_Type(type)
	{
		m_FramebufferShader = std::make_unique<Shader>("resources/shaders/viewport_vertex.shader", "resources/shaders/viewport_fragment.shader");
        
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        quadVAO.Bind();
        quadVBO = std::make_unique<VertexBuffer>(quadVertices, sizeof(float) * 24, GL_STATIC_DRAW);
        quadVBL.Push<float>(2);
        quadVBL.Push<float>(2);
        quadVAO.AddBuffer(*quadVBO, quadVBL);
        quadVAO.Unbind();
        
        _Init();
	}

    FrameBuffer()
    {
        m_FramebufferShader = std::make_unique<Shader>("resources/shaders/viewport_vertex.shader", "resources/shaders/viewport_fragment.shader");

        m_Type = Type::MONOSAMPLE;

        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        quadVAO.Bind();
        quadVBO = std::make_unique<VertexBuffer>(quadVertices, sizeof(float) * 24, GL_STATIC_DRAW);
        quadVBL.Push<float>(2);
        quadVBL.Push<float>(2);
        quadVAO.AddBuffer(*quadVBO, quadVBL);
        quadVAO.Unbind();
    }

    void OnRenderTexture(const Texture& tex) const
    {
        glDisable(GL_DEPTH_TEST);
        m_FramebufferShader->Bind();
        quadVAO.Bind();
        tex.Bind();
        tex.SetFiltering();
        m_FramebufferShader->SetUniform1i("screenTexture", 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadVAO.Unbind();
    }

    // Render the frame buffer inside a texture
	void OnRenderDirectlyToScreen()
	{
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

        // Draw to default frameBuffer e.g. the screen itself
        glClear(GL_COLOR_BUFFER_BIT);
        m_FramebufferShader->Bind();
        quadVAO.Bind();
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, m_MultisampleTextureColorBuffer->GetID());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadVAO.Unbind();
	}

    // Catch the rendered texture and display as an image
    void OnImGuiRender()
    {
        if (m_Type == Type::MULTISAMPLE)
        {
            // This resolve the multisample texture into a normal texture that we can read from
            // Draw on final image
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MultisampleFramebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_SingleSampleFramebuffer);
            glBlitFramebuffer(0, 0, *pScreenWidth, *pScreenHeight, 0, 0, *pScreenWidth, *pScreenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

        static bool show = false;
        ImGui::Begin("Viewport", &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Get final Image
        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.
        ImGui::Image((ImTextureID)m_SingleSampleTextureColorBuffer->GetID(), wsize, ImVec2(0, 1), ImVec2(1, 0));

        if ((float)*pScreenWidth != ImGui::GetWindowWidth() || (float)*pScreenHeight != ImGui::GetWindowHeight())
        {
            *pScreenWidth  = (unsigned int)ImGui::GetWindowWidth();
            *pScreenHeight = (unsigned int)ImGui::GetWindowHeight();
            glViewport(0, 0, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
            Reset();
        }

        ImGui::End();
    }

    // Witch frame buffer it will draw
    void Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Type == Type::MULTISAMPLE ? m_MultisampleFramebuffer : m_SingleSampleFramebuffer);
    }

    void Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Called when changed viewport size
    void Reset()
    {
        glDeleteFramebuffers(1, &m_SingleSampleFramebuffer);
        
        if (m_Type == Type::MULTISAMPLE)
            glDeleteFramebuffers(1, &m_MultisampleFramebuffer);

        _Init();
    }

private:
    void _Init()
    {
        // Final Image
        m_SingleSampleFramebuffer = 0;
        glGenFramebuffers(1, &m_SingleSampleFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_SingleSampleFramebuffer);

        // Create Attachment (memory) to write the color on
        m_SingleSampleTextureColorBuffer = std::make_unique<Texture>(*pScreenWidth, *pScreenHeight, Texture::Type::RAW);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SingleSampleTextureColorBuffer->GetID(), 0);

        // Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        unsigned int final_rbo;
        glGenRenderbuffers(1, &final_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, final_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *pScreenWidth, *pScreenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, final_rbo); // now actually attach it

        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        Unbind();

        if (m_Type == Type::MULTISAMPLE)
        {
            // Create FrameBuffer
            m_MultisampleFramebuffer = 0;
            glGenFramebuffers(1, &m_MultisampleFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_MultisampleFramebuffer);

            // Create Attachment (memory) to write the color on
            m_MultisampleTextureColorBuffer = std::make_unique<Texture>(*pScreenWidth, *pScreenHeight, Texture::Type::MULTISAMPLED);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MultisampleTextureColorBuffer->GetID(), 0);

            // Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, *pScreenWidth, *pScreenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
            
            // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

            Unbind();
        }
    }

public:
    enum class Type
    {
        MONOSAMPLE,
        MULTISAMPLE
    };
};