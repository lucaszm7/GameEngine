#include "SplineModel.hpp"

SplineModel::SplineModel(const float* pControlPoints,
						 const float* pControlPointsVectorDir,
						 unsigned int controlPointsCount,
						 unsigned int vectorsPerControlPointsCount)
{
	try
	{
		if (controlPointsCount == 0 || vectorsPerControlPointsCount == 0)
			throw std::exception("No Control Points passed to initialize Spline Model");

		controlPoints = { (Eigen::Vector3f*)pControlPoints , (Eigen::Vector3f*)pControlPoints + controlPointsCount };

		controlPointsVectorDir.resize(controlPointsCount);
		controlPointsVectorPos.resize(controlPointsCount);

		for (unsigned int i = 0; i < controlPointsCount; ++i)
		{
			controlPointsVectorDir.at(i).reserve(vectorsPerControlPointsCount);
			controlPointsVectorPos.at(i).reserve(vectorsPerControlPointsCount);

			for (unsigned int j = 0; j < vectorsPerControlPointsCount * 3; j += 3)
			{
				controlPointsVectorDir.at(i).emplace_back(pControlPointsVectorDir[j], pControlPointsVectorDir[j + 1], pControlPointsVectorDir[j + 2]);
				controlPointsVectorPos.at(i).push_back(this->controlPoints.at(i) + this->controlPointsVectorDir.at(i).back());
			}
		}
	}
	catch (std::exception exc)
	{
		std::cout << exc.what() << std::endl;
	}
}

SplineModel::SplineModel (const std::vector<Eigen::Vector3f>& controlPoints, 
						  const std::vector<std::vector<Eigen::Vector3f>>& controlPointsVectorDir)
	: controlPoints(controlPoints), controlPointsVectorDir(controlPointsVectorDir)
{
	try
	{
		size_t controlPointsCount = this->controlPoints.size();
		if (controlPointsCount == 0)
			throw std::exception("No Control Points passed to initialize Spline Model");

		size_t vectorsPerControlPointsCount = this->controlPointsVectorDir.back().size();
		if (vectorsPerControlPointsCount == 0)
			throw std::exception("No Vectors per Control Points passed to initialize Spline Model");

		this->controlPointsVectorPos.resize(this->controlPoints.size());
		
		for (unsigned int i = 0; i < controlPointsCount; ++i)
		{
			this->controlPointsVectorPos[i].reserve(vectorsPerControlPointsCount);
			for (unsigned int j = 0; j < vectorsPerControlPointsCount; ++j)
			{
				this->controlPointsVectorPos[i].push_back(this->controlPoints[i] + this->controlPointsVectorDir[i][j]);
			}
		}
	}
	catch (std::exception exc)
	{
		std::cout << exc.what() << std::endl;
	}
}