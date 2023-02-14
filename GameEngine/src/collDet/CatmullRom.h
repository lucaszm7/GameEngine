#ifndef CATMULLROM_H
#define CATMULLROM_H

#include <string>
#include <vector>
#include <Eigen/Dense>
//#include <GL/glew.h>
//#include "Camera.h"
//#include "Circle.h"
//#include "NewtonSolver.h"
//#include "SplineHelper.h"
//#include "Mesh.h"
//#include "BoundingVolumes.h"

class CatmullRom
{
public:
	static float GetT(float t, float alpha, Eigen::Vector3f p0, Eigen::Vector3f p1);
	static void Evaluate(std::vector<Eigen::Vector3f>& controlPoints,
		std::vector<Eigen::Vector3f>& splinePoints, int nSegments, float alpha);
};

#endif
