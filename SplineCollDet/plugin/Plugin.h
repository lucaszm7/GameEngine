#ifndef SPLINE_COLL_DET_PLUGIN_API_H
#define SPLINE_COLL_DET_PLUGIN_API_H

#include <vector>
#include <memory>
#include <Eigen/Core>
#include <SplineHelper.h>
#include <AccelerationStructures.h>

#ifndef _WIN32
	#define __stdcall 
#endif

#ifdef _WIN32
	#define DLLExport __declspec(dllexport)
#else
	#define DLLExport
#endif

struct Vector3
{
	float x;
	float y;
	float z;

	Eigen::Vector3f toEigen() {
		return Eigen::Vector3f(x, y, z);
	}

  Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3(Eigen::Vector3f v) {
		x = v.x();
		y = v.y();
		z = v.z();
	}
};


struct Spline {
	Vector3 *controlPoints;
	Vector3 **controlRaddi;
	Vector3 **controlRaddiPos;
	float uniformRadius;
};

typedef void(__stdcall* DebugCallback) (const char* str);
DebugCallback gDebugCallback;

std::unique_ptr<SplineModel> m_colon;
std::unique_ptr<SplineModel> m_endo;
std::unique_ptr<CollisionResult> m_result;
std::unique_ptr<SplineCollDet> m_detector;

extern "C"
{
	DLLExport void RegisterDebugCallback(DebugCallback callback);
	
	DLLExport void init();
	DLLExport void loadSplineModel(const char* filePath);
	DLLExport int getControlPointsCount();
	DLLExport int getVectorsCount();
	DLLExport Vector3 getEndoControlPoint(int index);
	DLLExport Vector3 getColonControlPoint(int index);
	DLLExport Vector3 getEndoVectorDir(int iControlPoint, int iVector);
	DLLExport Vector3 getColonVectorDir(int iControlPoint, int iVector);
	DLLExport Vector3 getEndoVectorPos(int iControlPoint, int iVector);
	DLLExport Vector3 getColonVectorPos(int iControlPoint, int iVector);
	DLLExport void animate(float dt);
	DLLExport int checkCollision();
	DLLExport int getCollisionLinePointsCount();
	DLLExport Vector3 getCollisionLinePoint(int index);
}



inline void GenerateSurface(SplineModel &splineModel) {
  const unsigned int numControlPoints = splineModel.controlPoints.size();
	const unsigned int numControlRadii = splineModel.controlRadii[0].size();

  splineModel.controlRadiiPos.clear();
	splineModel.controlRadiiPos.resize(splineModel.controlPoints.size());
	
  // Calc all vertices
	for (unsigned int i=0; i<numControlPoints; ++i)
	{
		for (int j=0; j<numControlRadii + 1; ++j)
		{
			Eigen::Vector3f normal;
      Eigen::Vector3f position;
			if (j < numControlRadii)
				normal = splineModel.controlRadii[i][j]; // 0.05f * radius;
			else
				normal = splineModel.controlRadii[i][0]; // 0.05f * radius;
			position = normal + splineModel.controlPoints[i];

			splineModel.controlRadiiPos[i].push_back(position);
		}
	}
}

inline void DebugInUnity(std::string message) {
	if (gDebugCallback) {
		gDebugCallback(message.c_str());
	}
}

#endif