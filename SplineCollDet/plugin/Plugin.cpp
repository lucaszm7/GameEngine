#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <vector>

#include "Plugin.h"

#include <SplineModel.hpp>
#include <AccelerationStructures.h>


void ConstructSplineCollisionDetection()
{
	m_collisionDetection = std::make_unique<SplineCollDet>();
}

void ConstructColonSplineModel(const float* pControlPoints, const float* pControlPointsVectorDir, unsigned int controlPointsCount, unsigned int vectorsPerControlPointsCount)
{
	m_colon = std::make_unique<SplineModel>(pControlPoints, pControlPointsVectorDir, controlPointsCount, vectorsPerControlPointsCount);
}

void ConstructDefaultColonSplineModel()
{
	m_colon = std::make_unique<SplineModel>(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(10.0f, 0.0f, 0.0f), 10, 32, 10);
}

void ConstructEndoSplineModel(Eigen::Vector3f posInit, Eigen::Vector3f posEnd, int controlPointsCount, int vectorsPerControlPointCount, float radius)
{
	m_endo = std::make_unique<SplineModel>(posInit, posEnd, controlPointsCount, vectorsPerControlPointCount, radius);
}

void ConstructDefaultEndoSplineModel()
{
	m_endo = std::make_unique<SplineModel>(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(10.0f, 0.0f, 0.0f), 10, 32, 1);
}

void* GetColonControlPointsPtr()
{
	return m_colon->controlPoints.data();
}

void* GetColonVectorsDirOfControlPoint(unsigned int controlPointIndex)
{
	try
	{
		return m_colon->controlPointsVectorDir.at(controlPointIndex).data();
	}
	catch (std::exception exp)
	{
		std::cout << exp.what() << std::endl;
	}
}

void* GetEndoControlPointsPtr()
{
	return m_endo->controlPoints.data();
}

void* GetCollisionResultPtr()
{
	return m_collisionDetection->collisionResults.collisionVectors.data();
}

unsigned int GetCollisionResultCount()
{
	return (unsigned int)m_collisionDetection->collisionResults.collisionVectors.size();
}




