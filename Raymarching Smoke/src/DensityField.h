#pragma once

#include "glm/glm.hpp"
#include <tuple>
#include <list>

class DensityField {
private:
	struct BoundingBox{
		glm::vec2 x;
		glm::vec2 y;
		glm::vec2 z;
	};

	BoundingBox bbox;
	std::list<std::tuple<glm::vec3, float>> rbf_list;

public:
	DensityField(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
	std::list<std::tuple<glm::vec3, float>> getList() const;
	void addPointsRandom(int count);
	float getDensity(glm::vec3 point);
};