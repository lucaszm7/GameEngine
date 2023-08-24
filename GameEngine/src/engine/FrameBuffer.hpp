#pragma once

#include <gl/GL.h>
#include <IMGUI/imgui.h>
#include <memory>
#include <Shader.h>

class FrameBuffer
{
private:
	std::shared_ptr<unsigned int> pScreenWidth;
	std::shared_ptr<unsigned int> pScreenHeight;

	std::unique_ptr<Shader> viewportShader;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;

    VertexArray quadVAO;
    VertexBufferLayout quadVBL;
    std::unique_ptr<VertexBuffer> quadVBO;

public:
	FrameBuffer(std::shared_ptr<unsigned int> screenWidth, std::shared_ptr<unsigned int> screenHeight)
		:pScreenWidth(screenWidth), pScreenHeight(screenHeight)
	{
		viewportShader = std::make_unique<Shader>("resources/shaders/viewport_vertex.shader", "resources/shaders/viewport_fragment.shader");
        
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
        viewportShader = std::make_unique<Shader>("resources/shaders/viewport_vertex.shader", "resources/shaders/viewport_fragment.shader");

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
        viewportShader->Bind();
        quadVAO.Bind();
        tex.Bind();
        tex.SetFiltering();
        viewportShader->SetUniform1i("screenTexture", 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadVAO.Unbind();
    }

    // Render the frame buffer inside a texture
	void OnRender()
	{
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClear(GL_COLOR_BUFFER_BIT);
        viewportShader->Bind();
        quadVAO.Bind();
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        quadVAO.Unbind();
	}

    // Catch the rendered texture and display as an image
    void OnImGuiRender()
    {
        static bool show = false;
        ImGui::Begin("Viewport", &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.
        ImGui::Image((ImTextureID)textureColorbuffer, wsize, ImVec2(0, 1), ImVec2(1, 0));

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
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    // Called when changed viewport size
    void Reset()
    {
        glDeleteFramebuffers(1, &framebuffer);
        _Init();
    }

private:
    void _Init()
    {
        // framebuffer configuration
        // -------------------------
        framebuffer = 0;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *pScreenWidth, *pScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *pScreenWidth, *pScreenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};