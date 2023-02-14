#include "BoundingVolumes.h"


int TestAABBs(const AABB& a, const AABB& b)
{
	// Exit with no intersection if separated along an axis
	if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return 0;
	if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return 0;
	if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return 0;
	// Overlapping on all axes means AABBs are intersecting
	return 1;
}

int TestAABBs(AABB a, AABB b, const Eigen::Affine3f& ma, const Eigen::Affine3f& mb)
{
	a.min = ma * a.min;
	a.max = ma * a.max;
	b.min = mb * b.min;
	b.max = mb * b.max;
	// Exit with no intersection if separated along an axis
	if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return 0;
	if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return 0;
	if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return 0;
	// Overlapping on all axes means AABBs are intersecting
	return 1;
}

int TestSpheres(Sphere a, Sphere b)
{
	float dist2 = (a.pos-b.pos).squaredNorm();
	float radiusSum = a.r + b.r;
	return dist2 <= radiusSum * radiusSum;
}

int TestSpheres(Sphere a, Sphere b, const Eigen::Affine3f& ma, const Eigen::Affine3f& mb)
{
	a.pos = ma * a.pos;
	b.pos = mb * b.pos;
	float dist2 = (a.pos-b.pos).squaredNorm();
	float radiusSum = a.r + b.r;
	return dist2 <= radiusSum * radiusSum;
}
