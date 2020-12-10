#include "DensityField.h"

#include <random>
#include <math.h>

DensityField::DensityField(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	bbox = BoundingBox{ 
		glm::vec2(minX, maxX), 
		glm::vec2(minY, maxY),
		glm::vec2(minZ, maxZ)
	};
}

void DensityField::addPointsRandom(int count) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib_x(bbox.x.x, bbox.x.y);
	std::uniform_real_distribution<> distrib_y(bbox.y.x, bbox.y.y);
	std::uniform_real_distribution<> distrib_z(bbox.z.x, bbox.z.y);

	for (int i = 0; i < count; ++i) {
		float rand_x = distrib_x(gen);
		float rand_y = distrib_y(gen);
		float rand_z = distrib_z(gen);

		std::tuple<glm::vec3, float> n_rbf(glm::vec3(rand_x, rand_y, rand_z), 1);

		rbf_list.push_back(n_rbf);
	}
}

float DensityField::getDensity(glm::vec3 point)
{
	float density = 0;
	for (std::tuple<glm::vec3, float> elem : rbf_list) {
		float magnitude = glm::distance(point, std::get<0>(elem));
		density += exp(-pow(magnitude / std::get<1>(elem), 2));
	}
	return density;
}

std::list<std::tuple<glm::vec3, float>> DensityField::getList() const
{
	return rbf_list;
}