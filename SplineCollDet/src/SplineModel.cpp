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
				unsigned int getAt = (i * (vectorsPerControlPointsCount * 3)) + j;
				controlPointsVectorDir.at(i).emplace_back(pControlPointsVectorDir[getAt], pControlPointsVectorDir[getAt + 1], pControlPointsVectorDir[getAt + 2]);
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

SplineModel::SplineModel (Eigen::Vector3f posInit,
						  Eigen::Vector3f posEnd,
					      const int controlPointsCount,
					      const int vectorsPerControlPointCount,
					      const float radius)
	:uniformRadius(radius)
{
	Eigen::Vector3f ptop = posEnd - posInit;
	Eigen::Vector3f ptopn = ptop / (float)(controlPointsCount - 1);
	controlPoints.reserve(controlPointsCount);
	controlPointsVectorDir.resize(controlPointsCount);
	controlPointsVectorPos.resize(controlPointsCount);
	Eigen::Vector3f pci;
	pci = posInit;

	for (int i = 0; i < controlPointsCount; ++i)
	{
		auto point = pci + ptopn * (float)i;
		controlPoints.emplace_back(point.x(), point.y(), point.z());
		controlPointsVectorDir[i].reserve(vectorsPerControlPointCount);
		controlPointsVectorPos[i].reserve(vectorsPerControlPointCount);
	}

	for (int i = 0; i < controlPointsCount; ++i) {
		Eigen::Vector3f w = ptop; // rotation axis
		w.normalize();
		Eigen::Vector3f ortho = ptopn;
		if (ortho.y() == 0.0f && ortho.z() == 0.0f)
			ortho = ortho.cross(Eigen::Vector3f::UnitY()).normalized();
		else
			ortho = ortho.cross(Eigen::Vector3f::UnitX()).normalized();

		ortho *= radius;
		constexpr float my_pi = std::numbers::pi_v<float>;
		Eigen::Quaternionf qr(Eigen::AngleAxisf(-2.0f * my_pi / (float)vectorsPerControlPointCount, w));
		for (int j = 0; j < vectorsPerControlPointCount; ++j)
		{
			controlPointsVectorDir[i].emplace_back(ortho.x(), ortho.y(), ortho.z());
			controlPointsVectorPos[i].emplace_back(controlPoints[i] + controlPointsVectorDir[i].back());
			ortho = qr * ortho;
		}
	}
}
