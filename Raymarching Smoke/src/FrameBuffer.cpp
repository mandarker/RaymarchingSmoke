#include "FrameBuffer.h"

#include "Renderer.h"
#include "GL/glew.h"
#include "glm/glm.hpp"

FrameBuffer::FrameBuffer(const unsigned int shadowWidth, const unsigned int shadowHeight)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
	// create depth texture
	GLCall(glGenTextures(1, &m_RendererTextureID));

	BindTexture();
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
	// attach depth texture as FBO's depth buffer
	Bind();
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_RendererTextureID, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	Unbind();
}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void FrameBuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::Unbind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::BindTexture() const 
{
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererTextureID));
}

void FrameBuffer::UnbindTexture() const 
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}