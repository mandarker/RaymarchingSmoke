#pragma once

#include <vector>
#include "glm/glm.hpp"

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

};