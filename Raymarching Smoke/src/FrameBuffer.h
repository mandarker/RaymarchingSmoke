#pragma once

class FrameBuffer 
{
public:
	FrameBuffer(const unsigned int shadowWidth, const unsigned int shadowHeight);
	~FrameBuffer();
	void Bind() const;
	void Unbind() const;
	void BindTexture() const;
	void UnbindTexture() const;
private:
	unsigned int m_RendererID;
	unsigned int m_RendererTextureID;
};