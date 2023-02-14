#pragma once

#include "spline/SplineModel.hpp"
#include "CatmullRom.h"
#include "BoundingVolumes.h"
#include "SplineCollDet.h"

#include <unsupported/Eigen/AlignedVector3>

#define EIGEN_VECTORIZE_SSE4_2
#define EIGEN_VECTORIZE_SSE4_1
#define EIGEN_VECTORIZE_SSSE3
#define EIGEN_VECTORIZE_SSE3


#define EPSILON 0.000001
#define NUM_BUCKETS 1024

struct Cell
{
	Cell(int px, int py, int pz);
	int x, y, z;
};

struct Object
{
	Object* pNextObject;
	Eigen::Vector3f pos;
	float radius;
	int bucket;
	CatmullRom* spline;
};

struct UniformGrid
{
	Object* objectBucket[NUM_BUCKETS];
};

int ComputeHashBucketIndex(Cell cellPos);

typedef enum {NODE, LEAF} TYPE;

struct ObjectBVH
{
	CatmullRom* spline;
};


struct NodeV7
{
	TYPE type;
	NodeV7* left;
	NodeV7* right;
	AABB bv;
	std::vector<Eigen::Vector3f> controlPoints;
	std::vector<Eigen::Vector3f> splinePoints;
	float radiusRod;
	
	SplineModel* splineModel;
	unsigned int startCPIndex;
	std::vector<std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>> radiusLines;

};

struct CollisionResult
{
	// Vetor com: ponto endoscopio - ponto parede de colon
	std::vector<Eigen::Vector3f> linePoints;

	// Ideia para retornar pontos fora do colon em outra estrutura auxiliar
	// std::vector<Eigen::Vector3f> linePointsOut;

	std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>> map;
	std::unordered_map<Eigen::Vector3f*, float> posRadius;

	std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f, Eigen::Vector3f>>> colonRadiiMap;

	unsigned int controlPointSegments {5};

	float subRadius;
	float endoRadius;

	float AABBAddFactor = 2.0f;
	bool OutDetection = true;

	std::mutex mutex;
	// ThreadPool pool {8};

	std::unordered_map<NodeV7*, std::vector<NodeV7*>> pairsToBeSampled;
	void SetLeavesEndo(const std::vector<NodeV7*>& nodes);
	void DistPairsSampleInParallel();
	void DistPairsSampleInParallelHelp(const unsigned int begin, const unsigned int end, const unsigned size);

	void SetCollisionCheckParam(float AABBNewScaleFactor, bool SetOutDetection);

	std::vector<Eigen::Vector3f>& CalcCollisionPoints();
	std::vector<Eigen::Vector3f>& CalcCollisionPointsCircle();
	std::vector<Eigen::Vector3f>& CalcCollisionPointsBilinear();
};

struct SplineCollDet
{
	CollisionResult r;

	std::vector<NodeV7> leavesA, leavesB;
	std::vector<NodeV7> nodesA, nodesB;
	std::vector<NodeV7*> leavesPointer;
	std::vector<NodeV7*> bvhBuild1;
	std::vector<NodeV7*> bvhBuild2;

	void ConstructLeavesBilinearSurfaceV7(SplineModel& splineModel, std::vector<NodeV7>& leaves);
	NodeV7* BottomUpBVTreeV7(std::vector<NodeV7*>& leaves, std::vector<NodeV7>& nodes);
	void CollisionCheck(SplineModel endo, SplineModel colon);
};

void SphereEnclosingSpheres(Sphere s0, Sphere s1, Sphere& s);
void AABBEnclosingAABBs(const AABB& a0, const AABB& a1, AABB& a);
void CalculateAABBMinMax(const std::vector<const std::vector<Eigen::Vector3f>*>& points,
		Eigen::Vector3f& min, Eigen::Vector3f& max, float& AABBAddFactor);
void LinearInterpolateRadiusBilinearV6(const SplineModel* splineModel, const unsigned int startCPIndex,
	std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
	std::vector<Eigen::Vector3f>& splinePoints, const unsigned int nSegments);
//void GenerateSphereFromLeavesV6(std::vector<NodeV6*> leaves, std::vector<Mesh>& meshes);
//void GenerateSphereFromTreeV6(NodeV6* root, std::vector<Mesh>& meshes);


void BVHCollisionBilinearSurfaceV7(NodeV7* a, NodeV7* b, CollisionResult& result);
void BVHCollisionBilinearSurfaceParallelV7(NodeV7* a, NodeV7* b, CollisionResult& result);


void MostSeparatedPointsOnAABB(int& min, int&max, std::vector<Eigen::Vector3f>& pt);
void SphereFromDistantPoints(Sphere &s, std::vector<Eigen::Vector3f>& pt);
void SphereOfSphereAndPt(Sphere &s, Eigen::Vector3f &p);
void RitterSphere(Sphere &s, std::vector<Eigen::Vector3f>& pt);



