#include "Object.h"
#include "OBJ-Loader/OBJ_Loader.h"

Object::Object(const char * filePath, bool b)
	: isBumpMap(b)
{
	objl::Loader loader;
	loader.LoadFile(filePath);

	if (isBumpMap) {
		// We're considering a bump map, calculate tangents
		vertexInfo = new float[loader.LoadedVertices.size() * 11];
		for (unsigned int i = 0; i < loader.LoadedVertices.size(); ++i) {
			vertexInfo[i * 11] = loader.LoadedVertices[i].Position.X;
			vertexInfo[i * 11 + 1] = loader.LoadedVertices[i].Position.Y;
			vertexInfo[i * 11 + 2] = loader.LoadedVertices[i].Position.Z;
			vertexInfo[i * 11 + 3] = loader.LoadedVertices[i].Normal.X;
			vertexInfo[i * 11 + 4] = loader.LoadedVertices[i].Normal.Y;
			vertexInfo[i * 11 + 5] = loader.LoadedVertices[i].Normal.Z;
			vertexInfo[i * 11 + 6] = loader.LoadedVertices[i].TextureCoordinate.X;
			vertexInfo[i * 11 + 7] = loader.LoadedVertices[i].TextureCoordinate.Y;
			vertexInfo[i * 11 + 8] = 0; // tangent x
			vertexInfo[i * 11 + 9] = 0; // tangent y
			vertexInfo[i * 11 + 10] = 0; // tangent z
		}

		for (unsigned int i = 0; i < loader.LoadedIndices.size(); i += 3) {
			unsigned int v0Index = loader.LoadedIndices[i];
			unsigned int v1Index = loader.LoadedIndices[i + 1];
			unsigned int v2Index = loader.LoadedIndices[i + 2];

			glm::vec3 edge1 = glm::vec3(loader.LoadedVertices[v1Index].Position.X, loader.LoadedVertices[v1Index].Position.Y, loader.LoadedVertices[v1Index].Position.Z)
				- glm::vec3(loader.LoadedVertices[v0Index].Position.X, loader.LoadedVertices[v0Index].Position.Y, loader.LoadedVertices[v0Index].Position.Z);
			glm::vec3 edge2 = glm::vec3(loader.LoadedVertices[v2Index].Position.X, loader.LoadedVertices[v2Index].Position.Y, loader.LoadedVertices[v2Index].Position.Z)
				- glm::vec3(loader.LoadedVertices[v0Index].Position.X, loader.LoadedVertices[v0Index].Position.Y, loader.LoadedVertices[v0Index].Position.Z);

			float deltaU1 = loader.LoadedVertices[v1Index].TextureCoordinate.X - loader.LoadedVertices[v0Index].TextureCoordinate.X;
			float deltaV1 = loader.LoadedVertices[v1Index].TextureCoordinate.Y - loader.LoadedVertices[v0Index].TextureCoordinate.Y;
			float deltaU2 = loader.LoadedVertices[v2Index].TextureCoordinate.X - loader.LoadedVertices[v0Index].TextureCoordinate.X;
			float deltaV2 = loader.LoadedVertices[v2Index].TextureCoordinate.Y - loader.LoadedVertices[v0Index].TextureCoordinate.Y;

			float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

			glm::vec3 tangent;

			tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
			tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
			tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

			for (unsigned int i = 8; i < 11; ++i) {
				vertexInfo[v0Index * 11 + i] += tangent[i - 8];
				vertexInfo[v1Index * 11 + i] += tangent[i - 8];
				vertexInfo[v2Index * 11 + i] += tangent[i - 8];
			}
		}

		VertexBuffer vb(vertexInfo, loader.LoadedVertices.size() * 11 * sizeof(float));
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		va.AddBuffer(vb, layout);
		vb.Unbind();
	}
	else {
		vertexInfo = new float[loader.LoadedVertices.size() * 8];
		for (unsigned int i = 0; i < loader.LoadedVertices.size(); ++i) {
			vertexInfo[i * 8] = loader.LoadedVertices[i].Position.X;
			vertexInfo[i * 8 + 1] = loader.LoadedVertices[i].Position.Y;
			vertexInfo[i * 8 + 2] = loader.LoadedVertices[i].Position.Z;
			vertexInfo[i * 8 + 3] = loader.LoadedVertices[i].Normal.X;
			vertexInfo[i * 8 + 4] = loader.LoadedVertices[i].Normal.Y;
			vertexInfo[i * 8 + 5] = loader.LoadedVertices[i].Normal.Z;
			vertexInfo[i * 8 + 6] = loader.LoadedVertices[i].TextureCoordinate.X;
			vertexInfo[i * 8 + 7] = loader.LoadedVertices[i].TextureCoordinate.Y;
		}

		VertexBuffer vb(vertexInfo, loader.LoadedVertices.size() * 8 * sizeof(float));
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		vb.Unbind();
	}

	indices = new unsigned int[loader.LoadedIndices.size()];
	for (unsigned int i = 0; i < loader.LoadedIndices.size(); ++i)
		indices[i] = loader.LoadedIndices[i];

	ib = IndexBuffer(indices, loader.LoadedIndices.size());


	va.Unbind();
	ib.Unbind();
}

Object::~Object()
{
	delete[] vertexInfo;
	delete[] indices;
}

void Object::SetVisiblity(bool t)
{
	isDisplaying = t;
}

void Object::Scale(float x, float y, float z)
{
	scale += glm::vec3(x, y, z);
}

void Object::Translate(float x, float y, float z)
{
	trans += glm::vec3(x, y, z);
}

glm::mat4 Object::model() 
{
	return glm::translate(glm::scale(glm::mat4(1.0f), scale), trans);
}