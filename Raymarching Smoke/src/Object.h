#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Object {
public:
	Object(const char * filePath, bool b);
	~Object();

	void SetVisiblity(bool t);
	void Scale(float x, float y, float z);
	void Translate(float x, float y, float z);

	void Render(Renderer *renderer, Shader *shader);
	glm::mat4 model();

	VertexArray va;
	IndexBuffer ib;
private:
	VertexBufferLayout layout;

	float * vertexInfo;
	unsigned int * indices;
	bool isBumpMap;
	bool isDisplaying;

	glm::vec3 scale;
	glm::vec3 trans;
};