#ifndef SPLINE_COLLISION_DETECTION_H
#define SPLINE_COLLISION_DETECTION_H

//#include "Bezier.h"
//#include "CatmullRom.h"
#include <unordered_map>
#include <mutex>
#include <vector>
#include <Eigen/Core>


void DistSampleCPU(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		float distLarger, std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& pointsLargerDist);

void DistSampleCPUParallel(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& pointsLargerDist, std::mutex& mutex);

void DistSampleCPU_old(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		float distLarger, std::vector<Eigen::Vector3f>& pointsLargerDist);

//void DistSampleBilinearCPU(const std::vector<Eigen::Vector3f>& splinePoints, const float radiusRod,
//		const std::vector<std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& radiusLines,
//		std::vector<Eigen::Vector3f>& linePoints);
void DistSampleBilinearCPU(std::vector<Eigen::Vector3f>& splinePoints, const float radiusRod,
		std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& endoColonMap,
		std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
		std::vector<Eigen::Vector3f>& linePoints);

inline float DistancePointLineSquared(const Eigen::Vector3f lineP0, const Eigen::Vector3f lineP1, const Eigen::Vector3f p);

#endif
