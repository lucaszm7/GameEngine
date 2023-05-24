#pragma once

#include "BoundingVolumes.h"
#include "SplineModel.hpp"
#include "CatmullRom.h"
#include "SplineCollDet.h"

#include <unsupported/Eigen/AlignedVector3>

#define EIGEN_VECTORIZE_SSE4_2
#define EIGEN_VECTORIZE_SSE4_1
#define EIGEN_VECTORIZE_SSSE3
#define EIGEN_VECTORIZE_SSE3

constexpr auto EPSILON = 0.000001;

enum class NodeType
{
	NODE, 
	LEAF
};

struct NodeV7
{
	NodeType type;
	NodeV7* left;
	NodeV7* right;
	AABB bv;
	std::vector<Eigen::Vector3f> controlPoints;
	std::vector<Eigen::Vector3f> splinePoints;
	float radiusRod;
	
	SplineModel* splineModel;
	unsigned int ControlPointStartIndex;

	std::vector<std::vector<std::pair<Eigen::Vector3f, Eigen::Vector3f>>> vectorsPerControlPoint;
};

struct CollisionResult
{
	// Vetor com: ponto endoscopio - ponto parede de colon
	std::vector<Eigen::Vector3f> collisionVectors;

	// Ideia para retornar pontos fora do colon em outra estrutura auxiliar
	// std::vector<Eigen::Vector3f> linePointsOut;

	//				   Endo Spline Point    Distance    Colon Spline Point
	std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>> broadPhaseEndoColonClosestSplinePoints;
	std::unordered_map<Eigen::Vector3f*, float> posRadius;

	//                Colon Spline Point      Vectors Per Spline Point ( Previous Radius   Next Radius )
	std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f, Eigen::Vector3f>>> colonInterpolatedVectorsSplinePoints;

	unsigned int nInterpolatedControlPoints = 10;

	float subRadius;
	float endoRadius;

	std::unordered_map<NodeV7*, std::vector<NodeV7*>> pairsToBeSampled;
	std::vector<Eigen::Vector3f>& CalcCollisionPointsBilinear();
};

struct SplineCollDet
{
	CollisionResult collisionResults;

	std::vector<NodeV7> leavesEndo;
	std::vector<NodeV7> leavesColon;

	NodeV7* rootEndo = nullptr;
	NodeV7* rootColon = nullptr;

	std::vector<NodeV7> nodesEndo;
	std::vector<NodeV7> nodesColon;

	std::vector<NodeV7*> leavesPointer;
	std::vector<NodeV7*> bvhBuild1;
	std::vector<NodeV7*> bvhBuild2;

	void ConstructLeavesBilinearSurfaceV7(SplineModel& splineModel, std::vector<NodeV7>& leaves) const;
	NodeV7* BottomUpBVTreeV7(const std::vector<NodeV7*>& leaves, std::vector<NodeV7>& nodesCache);
	void CollisionCheck(SplineModel& endo, SplineModel& colon);
};

void LinearInterpolateRadiusBilinearV6(const SplineModel* colonModel, const unsigned int startCPIndex,
	std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
	std::vector<Eigen::Vector3f>& splinePoints, const unsigned int nSegments);
void BVHCollisionBilinearSurfaceV7(NodeV7* a, NodeV7* b, CollisionResult& result);
void BVHCollisionBilinearSurfaceParallelV7(NodeV7* a, NodeV7* b, CollisionResult& result);

