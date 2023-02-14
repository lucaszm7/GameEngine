#include "SplineCollDet.h"
#include <iostream>


// General Dist Sampling
// TODO distLarger not used


void DistSampleCPU(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		float distLarger, std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& pointsLargerDist)
{
	//distLarger = distLarger * distLarger;
	unsigned int pointsAsize = pointsA.size();
	unsigned int pointsBsize = pointsB.size();
	for (unsigned int i=0;i<pointsAsize;i++)
	{
		float smallestDist = std::numeric_limits<float>().max();
		int smallestJ = -1;
		for (int j=0;j<pointsBsize;j++)
		{
			const float dist = (pointsA[i]-pointsB[j]).squaredNorm();
			if (dist < smallestDist)
			{
				smallestDist = dist;
				smallestJ = j;
			}
		}
		// Found local smallest dist
		// Update Global State
		auto it = pointsLargerDist.find(&pointsA[i]);
		if (it != pointsLargerDist.end())
		{
			if (smallestDist < it->second.first) 
			{
				it->second = std::make_pair(smallestDist, &pointsB[smallestJ]);
			}
		}
		else
		{
			pointsLargerDist[&pointsA[i]] = std::make_pair(smallestDist, &pointsB[smallestJ]);
		}
	}
}


void DistSampleCPUParallel(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& pointsLargerDist, std::mutex& mutex)
{
	unsigned int pointsAsize = pointsA.size();
	unsigned int pointsBsize = pointsB.size();
	for (unsigned int i=0;i<pointsAsize;i++)
	{
		float smallestDist = std::numeric_limits<float>().max();
		int smallestJ = -1;
		for (int j=0;j<pointsBsize;j++)
		{
			const float dist = (pointsA[i]-pointsB[j]).squaredNorm();
			if (dist < smallestDist)
			{
				smallestDist = dist;
				smallestJ = j;
			}
		}
		// Found local smallest dist
		

		// Update Global State
		auto it = pointsLargerDist.find(&pointsA[i]);
		if (it != pointsLargerDist.end())
		{
			if (smallestDist < it->second.first) 
			{
				it->second = std::make_pair(smallestDist, &pointsB[smallestJ]);
			}
		}
		else
		{
			mutex.lock();
			pointsLargerDist[&pointsA[i]] = std::make_pair(smallestDist, &pointsB[smallestJ]);
			mutex.unlock();
		}
	}
}

void DistSampleCPU_old(std::vector<Eigen::Vector3f>& pointsA, std::vector<Eigen::Vector3f>& pointsB,
		float distLarger, std::vector<Eigen::Vector3f>& pointsLargerDist)
{
	distLarger = distLarger * distLarger;
	for (int i=0;i<pointsA.size();i++)
	{
		float smallestDist = std::numeric_limits<float>().max();
		int smallestJ = -1;
		for (int j=0;j<pointsB.size();j++)
		{
			float dist = (pointsA[i]-pointsB[j]).squaredNorm();
			if (dist < smallestDist)
			{
				smallestDist = dist;
				smallestJ = j;
			}
		}
		//std::cout << smallestDist << std::endl;
		if (smallestJ > -1 && smallestDist > distLarger)
		{
			pointsLargerDist.push_back(pointsA[i]);
			pointsLargerDist.push_back(pointsB[smallestJ]);
		}
	}
}


//DistSampleBilinearCPU(a->splinePoints, a->radiusRod, b->radiusLines, result.linePoints);
void DistSampleBilinearCPU(std::vector<Eigen::Vector3f>& splinePoints, const float radiusRod,
		std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& endoColonMap,
		std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
		std::vector<Eigen::Vector3f>& linePoints)
{
	const float radius2 = radiusRod * radiusRod;
	const unsigned int splineSegments = splinePoints.size();
	const unsigned int circleSegments = colonRadiiMap[endoColonMap[&(splinePoints[0])].second].size();

	for (unsigned int i=0; i<splineSegments; i++)
	{
		std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& radiusLines = colonRadiiMap[endoColonMap[&(splinePoints[i])].second];
		for (unsigned int j=0; j<circleSegments; j++)
		{
			//const float dist = DistancePointLineSquared(radiusLines[i][j].first, radiusLines[i][j].second, splinePoints[i]);
			const Eigen::Vector3f& v = radiusLines[j].second - radiusLines[j].first;
			const Eigen::Vector3f& w = splinePoints[i] - radiusLines[j].first;
			float c1,c2;
			Eigen::Vector3f Pb;
			float dist;
			const Eigen::Vector3f* shortestPointToLine;
			if ( (c1 = w.dot(v)) <= 0.0f )
			{
				//if ((splinePoints[i]-radiusLines[i][j].first).squaredNorm() < radius2) {
				//	
				//}
				dist = (splinePoints[i]-radiusLines[j].first).squaredNorm();
				shortestPointToLine = &radiusLines[j].first; 
			}	
			else if ( (c2 = v.dot(v)) <= c1 )
			{
				//return (p-lineP1).squaredNorm();
				dist = (splinePoints[i]-radiusLines[j].second).squaredNorm(); 
				shortestPointToLine = &radiusLines[j].second; 
			}
			else
			{
				const float b = c1 / c2;
				Pb = radiusLines[j].first + b*v;
				dist = (splinePoints[i]-Pb).squaredNorm();
				shortestPointToLine = &Pb;
			}

			if (dist < radius2) // Collision
			//if (dist < radiusRod) // Collision
			{
				linePoints.push_back(splinePoints[i]);
				linePoints.push_back(*shortestPointToLine);
			}
		}
	}

}

float DistancePointLineSquared(const Eigen::Vector3f lineP0, const Eigen::Vector3f lineP1, const Eigen::Vector3f p)
{
	//return (((lineP1-lineP0).normalized()).cross(p-lineP0)).squaredNorm();
	//return (((lineP1-lineP0).normalized()).cross(p-lineP0)).norm();
	const Eigen::Vector3f v =  lineP1- lineP0;
	const Eigen::Vector3f w = p - lineP0;
	float c1,c2;
	if ( (c1 = w.dot(v)) <= 0.0f )
	{
		return (p-lineP0).squaredNorm();
	}
	if ( (c2 = v.dot(v)) <= c1 )
	{
		return (p-lineP1).squaredNorm();
	}

	const float b = c1 / c2;
	const Eigen::Vector3f Pb = lineP0 + b*v;
	return (p-Pb).squaredNorm();
}

