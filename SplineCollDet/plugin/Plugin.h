#pragma once

#include <vector>
#include <Eigen/Core>
#include <SplineModel.hpp>
#include <AccelerationStructures.h>

#ifndef _WIN32
	#define __stdcall 
#endif

#ifdef _WIN32
	#define DLLExport __declspec(dllexport)
#else
	#define DLLExport
#endif


std::unique_ptr<SplineModel> m_colon;
std::unique_ptr<SplineModel> m_endo;
std::unique_ptr<SplineCollDet> m_collisionDetection;

extern "C"
{
	DLLExport void ConstructSplineCollisionDetection();
	DLLExport void ConstructColonSplineModel(const float* pControlPoints,
											 const float* pControlPointsVectorDir,
											 unsigned int controlPointsCount,
											 unsigned int vectorsPerControlPointsCount);
	DLLExport void ConstructDefaultColonSplineModel();
	DLLExport void ConstructEndoSplineModel(Eigen::Vector3f posInit,
											Eigen::Vector3f posEnd,
											int controlPointsCount,
											int vectorsPerControlPointCount,
											float radius);
	DLLExport void ConstructDefaultEndoSplineModel();
	DLLExport void* GetColonControlPointsPtr();
	DLLExport void* GetColonVectorsDirOfControlPoint(unsigned int controlPointIndex);
	DLLExport void* GetEndoControlPointsPtr();
	DLLExport void* GetCollisionResultPtr();
	DLLExport unsigned int GetCollisionResultCount();
}

