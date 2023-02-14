#ifndef BOUNDING_VOLUMES_H
#define BOUNDING_VOLUMES_H

#include <Eigen/Core>
#include <Eigen/Dense>

struct AABB
{
	Eigen::Vector3f min;
	Eigen::Vector3f max;
};

int TestAABBs(const AABB& a, const AABB& b);

int TestAABBs(AABB a, AABB b, const Eigen::Affine3f& ma, const Eigen::Affine3f& mb);

struct Sphere
{
	Eigen::Vector3f pos;
	float r;
};

int TestSpheres(Sphere a, Sphere b);
int TestSpheres(Sphere a, Sphere b, const Eigen::Affine3f& ma, const Eigen::Affine3f& mb);


#endif
