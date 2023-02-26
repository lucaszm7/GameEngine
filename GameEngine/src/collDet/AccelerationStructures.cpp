#include "AccelerationStructures.h"
#include <iostream>
#include "engine/Timer.hpp"

std::vector<Eigen::Vector3f>& CollisionResult::CalcCollisionPointsBilinear()
{	
	const float radius2 = endoRadius * endoRadius;

	// Iterador pelos candidatos a colisão dos pontos interpolados do endoscopio
	auto itBroadPhaseCandidates = broadPhaseEndoColonClosestSplinePoints.begin();

	// Numero de vetores por pontos de controle do colon
	// it->second->second é o ponto do colon mais proximo do 1º ponto do endoscopio
	const size_t nVectorsPerControlPoint = colonInterpolatedVectorsSplinePoints[itBroadPhaseCandidates->second.second].size();

	Eigen::Vector3f DistanceEndoColon;
	Eigen::Vector3f colonRadius = { .0f, .0f, .0f };

	// Itera por todos pontos interpolados do endoscopio
	for (; itBroadPhaseCandidates != broadPhaseEndoColonClosestSplinePoints.end(); itBroadPhaseCandidates++)
	{
		// Endo control point -  Colon control point
		DistanceEndoColon = *(itBroadPhaseCandidates->first) - *(itBroadPhaseCandidates->second.second);

		// Vetor dos raios do ponto de controle do colon mais proximo do ponto de controle do endoscopio
		std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& colonVectorsSplinePoint = colonInterpolatedVectorsSplinePoints[itBroadPhaseCandidates->second.second];

		// Itera pelos raios do ponto do colon mais proximo do ponto atual do endoscopio
		for (unsigned int j = 0; j < nVectorsPerControlPoint; j++)
		{
			// Raio proximo - Raio atual
			const Eigen::Vector3f& pairPreviousNextColonRadius = colonVectorsSplinePoint[j].second - colonVectorsSplinePoint[j].first;

			// Ponto atual do endoscopio - Raio atual
			const Eigen::Vector3f& vectorEndoSplinePointColonRadiusPrevious = *(itBroadPhaseCandidates->first) - colonVectorsSplinePoint[j].first;

			float c1, c2;
			float dist;
			Eigen::Vector3f collisionPoint (1,1,1);
			const Eigen::Vector3f* shortestPointToLine;

			// Dot Product da diferença entre o raio atual e o proximo
			// com o ponto de controle e o raio atual

			// O ponto esta na metade anterior do raio atual
			// Angulo entre (w e v) >= 90º
			if ((c1 = vectorEndoSplinePointColonRadiusPrevious.dot(pairPreviousNextColonRadius)) <= 0.0f)
			{
				dist = vectorEndoSplinePointColonRadiusPrevious.squaredNorm();
				shortestPointToLine = &colonVectorsSplinePoint[j].first;
			}
			// O ponto está depois de V
			// Está fora da faixa de teste, depois do raio proximo
			else if ((c2 = pairPreviousNextColonRadius.dot(pairPreviousNextColonRadius)) <= c1)
			{
				dist = (*(itBroadPhaseCandidates->first) - colonVectorsSplinePoint[j].second).squaredNorm();
				shortestPointToLine = &colonVectorsSplinePoint[j].second;
			}
			// Entre o raio atual e o proximo
			// Pb é o raio correspondente a colisão
			else
			{
				const float b = c1 / c2;
				collisionPoint = colonVectorsSplinePoint[j].first + b * pairPreviousNextColonRadius;
				dist = (*(itBroadPhaseCandidates->first) - collisionPoint).squaredNorm();
				shortestPointToLine = &collisionPoint;

				// === Ve se esta fora ===
				colonRadius = (collisionPoint - *(itBroadPhaseCandidates->second.second));
				// Verifica se saiu
				// Projeção do vetor ColonEndo em relação ao raio atual
				if (DistanceEndoColon.squaredNorm() > colonRadius.squaredNorm() && false)
					dist = - (DistanceEndoColon - colonRadius).squaredNorm();
			}

			// Endoscopio fora do colon, troca a ordem de colisão para
			// inverter vetor colisão na IPS
			if ((dist < 0) && false)
			{
				collisionVectors.push_back(*shortestPointToLine);
				collisionVectors.push_back(*(itBroadPhaseCandidates->first));
			}
			// Dist é a menor distancia do ponto de controle do endoscopio
			// até a faixa que estamos testando.
			// E aqui verificamos se esta distancia é menor que o raio do endoscopio

			// Ponto do endoscopio
			// Ponto parede do colon
			else if (dist < radius2) 
			{
				collisionVectors.push_back(*(itBroadPhaseCandidates->first));
				collisionVectors.push_back(*shortestPointToLine);
			}
		}
	}
	return collisionVectors;
}

NodeV7* SplineCollDet::BottomUpBVTreeV7(const std::vector<NodeV7*>& leavesPointer, std::vector<NodeV7>& nodesCache)
{
	size_t numNodesOfThisTreeLayer = leavesPointer.size();

	nodesCache.clear();
	nodesCache.resize(numNodesOfThisTreeLayer - 1);

	std::vector<NodeV7*>* oldLayer = &bvhBuild1;
	std::vector<NodeV7*>* newLayer = &bvhBuild2;
	oldLayer->clear();
	newLayer->clear();
	oldLayer->reserve(leavesPointer.size());
	newLayer->reserve(leavesPointer.size());
	*oldLayer = leavesPointer;

	unsigned int nodeCounter = 0;
	while (numNodesOfThisTreeLayer > 1)
	{
		for (int i = 0; i < oldLayer->size(); i += 2)
		{
			if (i + 1 < oldLayer->size())
			{
				NodeV7& newPair = nodesCache[nodeCounter++];
				newPair.type = TYPE::NODE;
				newPair.left = (*oldLayer)[i];
				newPair.right = (*oldLayer)[i+1];
				AABBEnclosingAABBs((*oldLayer)[i]->bv, (*oldLayer)[i+1]->bv, newPair.bv);
				newLayer->push_back(&newPair);
			}
			else
			{
				newLayer->push_back((*oldLayer)[i]);
			}
		}
		numNodesOfThisTreeLayer = newLayer->size();
		std::vector<NodeV7*>* x = oldLayer;
		oldLayer = newLayer;
		newLayer = x;
		newLayer->clear();
	}
	NodeV7* ret = (*oldLayer)[0];
	return ret;
}

void SplineCollDet::ConstructLeavesBilinearSurfaceV7(SplineModel& splineModel, std::vector<NodeV7>& leaves) const
{
	const size_t numControlPoints = splineModel.controlPoints.size();
	leaves.clear();
	leaves.resize(numControlPoints - 3);

	for (int i = 0; i < numControlPoints - 3; i++)
	{
		Eigen::Vector3f p0,p1,p2,p3;
		p0 = splineModel.controlPoints[i];
		p1 = splineModel.controlPoints[i+1];
		p2 = splineModel.controlPoints[i+2];
		p3 = splineModel.controlPoints[i+3];

		leaves[i].controlPoints.clear();
		leaves[i].controlPoints.reserve(4);
		leaves[i].controlPoints.push_back(p0);
		leaves[i].controlPoints.push_back(p1);
		leaves[i].controlPoints.push_back(p2);
		leaves[i].controlPoints.push_back(p3);
		leaves[i].radiusRod = splineModel.uniformRadius;

		leaves[i].type = TYPE::LEAF;
		leaves[i].left = nullptr;
		leaves[i].right = nullptr;

		// Interpolate linear/bilinear
		std::vector<const std::vector<Eigen::Vector3f>*> points;
		points.push_back(&splineModel.controlPointsVectorPos[i+1]);
		points.push_back(&splineModel.controlPointsVectorPos[i+2]);
		CalculateAABBMinMax(points, leaves[i].bv.min, leaves[i].bv.max);

		leaves[i].splineModel = &splineModel;
		leaves[i].ControlPointStartIndex = i;
	}
}

void BVHCollisionBilinearSurfaceV7(NodeV7* endoNode, NodeV7* colonNode, CollisionResult& collResult)
{
	if (!TestAABBs(endoNode->bv,colonNode->bv))
		return;

	if (endoNode->type == TYPE::LEAF && colonNode->type == TYPE::LEAF)
	{
		if (endoNode->splinePoints.empty())
			CatmullRom::Evaluate(endoNode->controlPoints, endoNode->splinePoints, collResult.nInterpolatedControlPoints, 1.0);
		if (colonNode->splinePoints.empty())
		{
			CatmullRom::Evaluate(colonNode->controlPoints, colonNode->splinePoints, collResult.nInterpolatedControlPoints, 1.0); 
			LinearInterpolateRadiusBilinearV6(colonNode->splineModel, colonNode->ControlPointStartIndex, collResult.colonInterpolatedVectorsSplinePoints, colonNode->splinePoints, collResult.nInterpolatedControlPoints);
		}
		DistSampleCPU(endoNode->splinePoints, colonNode->splinePoints, endoNode->radiusRod, collResult.broadPhaseEndoColonClosestSplinePoints); 
	}
	else
	{
		if (endoNode->type == TYPE::LEAF)
		{
			BVHCollisionBilinearSurfaceV7(endoNode, colonNode->left, collResult);
			BVHCollisionBilinearSurfaceV7(endoNode, colonNode->right, collResult);
		}
		else
		{
			BVHCollisionBilinearSurfaceV7(endoNode->left, colonNode, collResult);
			BVHCollisionBilinearSurfaceV7(endoNode->right, colonNode, collResult);
		}
	}
}

void BVHCollisionBilinearSurfaceParallelV7(NodeV7* a, NodeV7* b, CollisionResult& result)
{
	if (!TestAABBs(a->bv,b->bv))
		return;
	if (a->type == TYPE::LEAF && b->type == TYPE::LEAF)
	{
		if (a->splinePoints.empty())
			CatmullRom::Evaluate(a->controlPoints, a->splinePoints, 10, 1.0);
		if (b->splinePoints.empty())
		{
			CatmullRom::Evaluate(b->controlPoints, b->splinePoints, 10, 1.0); 
			LinearInterpolateRadiusBilinearV6(b->splineModel, b->ControlPointStartIndex, result.colonInterpolatedVectorsSplinePoints, b->splinePoints, 10);
		}
		result.pairsToBeSampled[a].push_back(b);
	}
	else
	{
		if (a->type == TYPE::LEAF)
		{
			//if (b->left != nullptr)
				BVHCollisionBilinearSurfaceParallelV7(a, b->left, result);
			//if (b->right != nullptr)
				BVHCollisionBilinearSurfaceParallelV7(a, b->right, result);
		}
		else
		{
			//if (a->left != nullptr)
				BVHCollisionBilinearSurfaceParallelV7(a->left, b, result);
			//if (a->right != nullptr)
				BVHCollisionBilinearSurfaceParallelV7(a->right, b, result);
		}
	}
}

// Spline Model = Colon
// Start CPI (Control Point Index) da folha 
// Onde botar o resultado, hash table controlRadiiMap Vector3 - pair(Vector3, Vector3)
// Pontos de interpolação da folha
// nSegments = {5} ??? Não sei pq

// Esta criando os vetores por pontos de controle para os pontos interpolados
// Passa por os pontos interpolados de [1 a 5] (4), calculando a interpolação dos raios por cada ponto de interpolação
// gerando um par associado do raio com o raio anterior

// No final o controlRadiiMap tem um mapeamento de ponteiros de cada ponto da interpoalção
// com um vetor de pares de raios (raio anterior e atual)

// Depois ele só vai colocando no mapeamento P1 e P2 que já eram conhecidos

// |  | | | |  |
// p1 - - - - p2
// |  | | | |  |
void LinearInterpolateRadiusBilinearV6(const SplineModel* colonModel, 
									   const unsigned int startCPIndex,
									   std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
	                                   std::vector<Eigen::Vector3f>& splinePoints, 
									   const unsigned int nSegments)
{
	const float delta = 1.0f / static_cast<float>(nSegments);

	const unsigned int indexP1 = startCPIndex + 1; // p0 - |p1 - - - -  p2 - p3
	const unsigned int indexP2 = startCPIndex + 2; // p0 -  p1 - - - - |p2 - p3

	const size_t nVectorsPerControlPoint = colonModel->controlPointsVectorDir[indexP1].size();
	unsigned int intervalIndex = 1;

	// need to interpolate positions instead of radi
	float t = delta;

	for (;intervalIndex < nSegments; intervalIndex++)
	{
		// &(splinePoints[intervalIndex]) is the Key of the Unordered Map
		colonRadiiMap[&(splinePoints[intervalIndex])] = std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>();
		std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& vectorsPerSplineInterpolatedPoints = colonRadiiMap[&(splinePoints[intervalIndex])];

		vectorsPerSplineInterpolatedPoints.reserve(nVectorsPerControlPoint);

		//									0.8 * RadiiPos[P1][0] + 0.2 * RadiiPos[P2][0]
		//									Quanto mais perto de P2, mais influenciado por ele
		Eigen::Vector3f previousRadiusPos = (1-t) * colonModel->controlPointsVectorPos[indexP1][0] + t * colonModel->controlPointsVectorPos[indexP2][0];
		Eigen::Vector3f firstPos = previousRadiusPos;
		Eigen::Vector3f radiusPos;

		for (unsigned int i = 1; i < nVectorsPerControlPoint; i++) // sec. vec
		{
			//					0.8 * RadiiPos[P1][i] + 0.2 * RadiiPos[P2][i]
			//					Quanto mais perto de P2, mais influenciado por ele
			radiusPos = (1-t) * colonModel->controlPointsVectorPos[indexP1][i] + t * colonModel->controlPointsVectorPos[indexP2][i];

			//					Carrega o raio anterior e o seu
			vectorsPerSplineInterpolatedPoints.emplace_back(previousRadiusPos, radiusPos);
			previousRadiusPos = radiusPos;
		}

		// E o ultimo raio é também o primeiro, pois só pode ser adicionado depois que soubemos qual era o anterior a ele
		vectorsPerSplineInterpolatedPoints.emplace_back(previousRadiusPos, firstPos);

		// Raios circulares!!! Que são calculados para todos pontos interpolados!!!

		t += delta;
	}	

	colonRadiiMap[&(splinePoints[0])] = std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>();
	std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& vectorsOnP1 =	colonRadiiMap[&(splinePoints[0])];
	vectorsOnP1.reserve(nVectorsPerControlPoint);
	for (unsigned int i = 1; i < nVectorsPerControlPoint; i++) // sec. vec
	{
		vectorsOnP1.emplace_back(colonModel->controlPointsVectorPos[indexP1][i-1], colonModel->controlPointsVectorPos[indexP1][i]);
	}
	vectorsOnP1.emplace_back(colonModel->controlPointsVectorPos[indexP1][0], colonModel->controlPointsVectorPos[indexP1][nVectorsPerControlPoint-1]);

	colonRadiiMap[&(splinePoints[intervalIndex-1])] = std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>();
	std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& vectorsOnP2 =	colonRadiiMap[&(splinePoints[intervalIndex-1])];
	vectorsOnP2.reserve(nVectorsPerControlPoint);
	for (unsigned int i = 1; i < nVectorsPerControlPoint; i++) // sec. vec
	{
		vectorsOnP2.emplace_back(colonModel->controlPointsVectorPos[indexP2][i-1], colonModel->controlPointsVectorPos[indexP2][i]);
	}
	vectorsOnP2.emplace_back(colonModel->controlPointsVectorPos[indexP2][0], colonModel->controlPointsVectorPos[indexP2][nVectorsPerControlPoint - 1]);
}

void SplineCollDet::CollisionCheck(SplineModel endo, SplineModel colon)
{
	endo.TransformPoints();
	colon.TransformPoints();

	collisionResults.collisionVectors.clear();
	collisionResults.broadPhaseEndoColonClosestSplinePoints.clear();
	collisionResults.colonInterpolatedVectorsSplinePoints.clear();

	// BROAD PHASE
	ConstructLeavesBilinearSurfaceV7(endo, leavesEndo);
	ConstructLeavesBilinearSurfaceV7(colon, leavesColon);

	leavesPointer.clear();
	leavesPointer.reserve(leavesEndo.size());	
	for (NodeV7& pLeaveEndo : leavesEndo)
		leavesPointer.push_back(&pLeaveEndo);	
	NodeV7* rootEndo = BottomUpBVTreeV7(leavesPointer, nodesEndo);

	leavesPointer.clear();
	leavesPointer.reserve(leavesColon.size());	
	for (NodeV7& pLeaveColon : leavesColon)
		leavesPointer.push_back(&pLeaveColon);	
	NodeV7* rootColon = BottomUpBVTreeV7(leavesPointer, nodesColon);

	// NARROW PHASE
	BVHCollisionBilinearSurfaceV7(rootEndo, rootColon, collisionResults);
	collisionResults.endoRadius = endo.uniformRadius;
	collisionResults.CalcCollisionPointsBilinear();
}
