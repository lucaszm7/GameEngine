#pragma once

#include <unordered_map>
#include <vector>
#include <mutex>
#include <Eigen/Core>


void DistSampleCPU(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		float distLarger, std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& pointsLargerDist);

void DistSampleCPUParallel(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& pointsLargerDist, std::mutex& mutex);

void DistSampleCPU_old(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		float distLarger, std::vector<Eigen::Vector3f>& pointsLargerDist);

void DistSampleBilinearCPU(std::vector<Eigen::Vector3f>& splinePoints, const float radiusRod,
		std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& endoColonMap,
		std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
		std::vector<Eigen::Vector3f>& linePoints);

inline float DistancePointLineSquared(const Eigen::Vector3f lineP0, const Eigen::Vector3f lineP1, const Eigen::Vector3f p);

