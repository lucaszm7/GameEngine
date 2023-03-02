#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

struct AABB
{
	Eigen::Vector3f min;
	Eigen::Vector3f max;
};

int TestAABBs(const AABB& a, const AABB& b);
void AABBEnclosingAABBs(const AABB& a, const AABB& b, AABB& result);
void CalculateAABBMinMax(const std::vector<const std::vector<Eigen::Vector3f>*>& points, Eigen::Vector3f& min, Eigen::Vector3f& max);
