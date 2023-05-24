#include "BoundingVolumes.h"

int TestAABBs(const AABB& a, const AABB& b)
{
	enum
	{
		SEPARATED = 0,
		COLLIDING = 1,
	};
	// Exit with no intersection if separated along an axis
	if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return SEPARATED;
	if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return SEPARATED;
	if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return SEPARATED;
	// Overlapping on all axes means AABBs are intersecting
	return COLLIDING;
}

void AABBEnclosingAABBs(const AABB& a, const AABB& b, AABB& result)
{
	result.min[0] = std::min(a.min[0], b.min[0]);
	result.min[1] = std::min(a.min[1], b.min[1]);
	result.min[2] = std::min(a.min[2], b.min[2]);
	result.max[0] = std::max(a.max[0], b.max[0]);
	result.max[1] = std::max(a.max[1], b.max[1]);
	result.max[2] = std::max(a.max[2], b.max[2]);
}

void CalculateAABBMinMax(const std::vector<const std::vector<Eigen::Vector3f>*>& points, Eigen::Vector3f& min, Eigen::Vector3f& max)
{
	min = max = (*points[0])[0];
	const size_t numPointSets = points.size();
	const size_t numPoints = points[0]->size();
	for (size_t i = 0; i < numPointSets; i++)
	{
		for (size_t j = 0; j < numPoints; j++)
		{
			if ((*points[i])[j][0] < min[0])
			{
				min[0] = (*points[i])[j][0];
			}
			if ((*points[i])[j][1] < min[1])
			{
				min[1] = (*points[i])[j][1];
			}
			if ((*points[i])[j][2] < min[2])
			{
				min[2] = (*points[i])[j][2];
			}
			if ((*points[i])[j][0] > max[0])
			{
				max[0] = (*points[i])[j][0];
			}
			if ((*points[i])[j][1] > max[1])
			{
				max[1] = (*points[i])[j][1];
			}
			if ((*points[i])[j][2] > max[2])
			{
				max[2] = (*points[i])[j][2];
			}
		}
	}
}