#include "AccelerationStructures.h"
#include <iostream>


int ComputeHashBucketIndex(Cell cellPos)
{
	const int h1 = 0x8da6b343; // Large multiplicative constants;
	const int h2 = 0xd8163841; // here arbitrarily chosen primes
	const int h3 = 0xcb1ab31f;
	int n = h1 * cellPos.x + h2 * cellPos.y + h3 * cellPos.z;
	n = n % NUM_BUCKETS;
	if (n < 0) n += NUM_BUCKETS;
	return n;
}

void SphereEnclosingSpheres(Sphere s0, Sphere s1, Sphere& s)
{
	Eigen::Vector3f d = s1.pos - s0.pos;
	float dist2 = d.squaredNorm();
	float sr = s1.r - s0.r;
	if (sr*sr >= dist2)
	{
		if (s1.r >= s0.r)
			s = s1;
		else
			s = s0;
	}
	else
	{
		float dist = std::sqrt(dist2);
		s.r = (dist + s0.r + s1.r) * 0.5f;
		s.pos = s0.pos;
		if (dist > EPSILON)
			s.pos += ((s.r - s0.r) / dist) * d;
	}
}

void AABBEnclosingAABBs(const AABB& a0, const AABB& a1, AABB& a)
{
	a.min[0] = std::min(a0.min[0], a1.min[0]);
	a.min[1] = std::min(a0.min[1], a1.min[1]);
	a.min[2] = std::min(a0.min[2], a1.min[2]);
	a.max[0] = std::max(a0.max[0], a1.max[0]);
	a.max[1] = std::max(a0.max[1], a1.max[1]);
	a.max[2] = std::max(a0.max[2], a1.max[2]);
}

std::vector<Eigen::Vector3f>& CollisionResult::CalcCollisionPoints()
{
	linePoints.clear();
	float d2 = subRadius * subRadius;
	for (auto it=map.begin();it!=map.end();it++)
	{
		if (it->second.first > d2)
		{
			linePoints.push_back(*(it->first));
			linePoints.push_back(*(it->second.second));
		}
	}
	return linePoints;
}

std::vector<Eigen::Vector3f>& CollisionResult::CalcCollisionPointsCircle()
{
	linePoints.clear();
	//float d2 = subRadius * subRadius;
	// iterate Endo->Colon points
	for (auto it=map.begin();it!=map.end();it++)
	{
		float d2 = endoRadius - posRadius[it->second.second];
		//std::cout << "found" << std::endl;
		//float d2 = endoRadius - 0.05f;
		//float d2 = endoRadius - search->second; 
		d2 = d2 * d2;
		//if (it->second.first < d2)
		if (it->second.first > d2)
		{
			linePoints.push_back(*(it->first));
			linePoints.push_back(*(it->second.second));
		}
		//else
		//	std::cout << "not found" << std::endl;
	}
	return linePoints;
}

std::vector<Eigen::Vector3f>& CollisionResult::CalcCollisionPointsBilinear()
{	
	const float radius2 = endoRadius * endoRadius;

	// Iterador pelos pontos interpolados do endoscopio
	auto it = map.begin();

	// Numero de vetores por pontos de controle do colon
	// it->second->second é o ponto do colon mais proximo do 1º ponto do endoscopio
	const unsigned int circleSegments = colonRadiiMap[it->second.second].size();

	Eigen::Vector3f DisEndoColon;
	Eigen::Vector3f colonRadius = { .0f, .0f, .0f };

	// Itera por todos pontos interpolados do endoscopio
	for ( ;it!=map.end();it++)
	{

		// Endo control point -  Colon control point
		DisEndoColon = *(it->first) - *(it->second.second);

		// Vetor dos raios do ponto de controle do colon mais proximo do ponto de controle do endoscopio
		std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& radiusLines = colonRadiiMap[it->second.second];

		// Itera pelos raios do ponto do colon mais proximo do ponto atual do endoscopio
		for (unsigned int j = 0; j < circleSegments; j++)
		{
			// Raio proximo - Raio atual
			const Eigen::Vector3f& v = radiusLines[j].second - radiusLines[j].first;

			// Ponto atual do endoscopio - Raio atual
			const Eigen::Vector3f& w = *(it->first) - radiusLines[j].first;

			float c1, c2, c3;
			Eigen::Vector3f Pb (1,1,1);
			// float DisEndoColon;
			// float outDis = -1;
			float dist;
			const Eigen::Vector3f* shortestPointToLine;

			// Dot Product da diferença entre o raio atual e o proximo
			// com o ponto de controle e o raio atual

			// O ponto esta na metade anterior do raio atual
			// Angulo entre (w e v) >= 90º
			if ( (c1 = w.dot(v)) <= 0.0f )
			{
				dist = (*(it->first)-radiusLines[j].first).squaredNorm();
				shortestPointToLine = &radiusLines[j].first;

				// Verificacao se esta fora
				if (dist < radius2)
				{
					colonRadius = radiusLines[j].first;
					if (colonRadius.dot(DisEndoColon) > colonRadius.dot(colonRadius))
					{
						dist *= -1;
					}

				}

			}

			// O ponto está depois de V
			// Está fora da faixa de teste, depois do raio proximo
			else if ( (c2 = v.dot(v)) <= c1 )
			{
				dist = (*(it->first)-radiusLines[j].second).squaredNorm(); 
				shortestPointToLine = &radiusLines[j].second;

				// Verificacao se esta fora
				if (dist < radius2)
				{
					colonRadius = radiusLines[j].second;
					if (colonRadius.dot(DisEndoColon) > colonRadius.dot(colonRadius))
					{
						dist *= -1;
					}

				}

			}

			// Entre o raio atual e o proximo
			// Pb é o raio correspondente a colisão
			else
			{
				const float b = c1 / c2;
				Pb = radiusLines[j].first + b*v;
				dist = (*(it->first)-Pb).squaredNorm();
				shortestPointToLine = &Pb;

				// === Ve se esta fora ===
				colonRadius = (Pb - *(it->second.second));

				// Verifica se saiu
				// Projeção do vetor ColonEndo em relação ao raio atual
				if ( ((c3 = colonRadius.dot(DisEndoColon)) > colonRadius.squaredNorm()) && OutDetection)
				{
					dist = - (DisEndoColon - colonRadius).squaredNorm();
				}
			}

			// Endoscopio fora do colon, troca a ordem de colisão para
			// inverter vetor colisão na IPS
			if ((dist < 0) && OutDetection)
			{
				linePoints.push_back(*shortestPointToLine);
				linePoints.push_back(*(it->first));
			}

			// Dist é a menor distancia do ponto de controle do endoscopio
			// até a faixa que estamos testando.
			// E aqui verificamos se esta distancia é menor que o raio do endoscopio

			// Ponto do endoscopio
			// Ponto parede do colon
			else if (dist < radius2) 
			{
				linePoints.push_back(*(it->first));
				linePoints.push_back(*shortestPointToLine);
			}
		}
	}
	return linePoints;
}

void CollisionResult::SetLeavesEndo(const std::vector<NodeV7*>& nodes)
{
	const size_t size = nodes.size();
	pairsToBeSampled.reserve(size);

	for (size_t i=0;i<size;i++)
		pairsToBeSampled[nodes[i]] = std::vector<NodeV7*>();
}

void CollisionResult::DistPairsSampleInParallel()
{
	/*std::vector<std::thread> v;
	v.reserve(pairsToBeSampled.size());
	const size_t size = pairsToBeSampled.size();

	auto re1 = pool.enqueue([=] () {DistPairsSampleInParallelHelp(0,size/4,size);});
	auto re2 = pool.enqueue([=] () {DistPairsSampleInParallelHelp(size/4,2*size/4,size);});
	auto re3 = pool.enqueue([=] () {DistPairsSampleInParallelHelp(2*size/4,3*size/4,size);});
	auto re4 = pool.enqueue([=] () {DistPairsSampleInParallelHelp(3*size/4,size,size);});

	re1.get();
	re2.get();
	re3.get();
	re4.get();*/

}

void CollisionResult::DistPairsSampleInParallelHelp(const unsigned int begin, const unsigned int end, const unsigned size)
{	
	auto it=pairsToBeSampled.begin();
	std::advance(it, begin);
	for (unsigned int i=begin; i<end; i++)
	{
		for (NodeV7* b : it->second)
			DistSampleCPUParallel(it->first->splinePoints, b->splinePoints, map, mutex); 
		++it;
	}
}

void CollisionResult::SetCollisionCheckParam(float AABBNewAddFactor, bool SetOutDetection)
{
	// controlPointSegments = controlPointSeg;
	AABBAddFactor = AABBNewAddFactor;
	OutDetection = SetOutDetection;
}

NodeV7* SplineCollDet::BottomUpBVTreeV7(std::vector<NodeV7*>& leaves, std::vector<NodeV7>& nodes)
{
	int numObjects = leaves.size();

	nodes.clear();
	nodes.resize(numObjects - 1);

	std::vector<NodeV7*>* tmp1 = &bvhBuild1;
	std::vector<NodeV7*>* tmp2 = &bvhBuild2;
	tmp1->clear();
	tmp2->clear();
	tmp1->reserve(leaves.size());
	tmp2->reserve(leaves.size());
	*tmp1 = leaves;
	unsigned int nodeCounter = 0;
	while (numObjects > 1)
	{
		for (int i=0; i<tmp1->size();i+=2)
		{
			if (i+1 < tmp1->size())
			{
				NodeV7& newPair = nodes[nodeCounter++];
				newPair.type = NODE;
				newPair.left = (*tmp1)[i];
				newPair.right = (*tmp1)[i+1];
				AABBEnclosingAABBs((*tmp1)[i]->bv, (*tmp1)[i+1]->bv, newPair.bv);
				tmp2->push_back(&newPair);
			}
			else
			{
				tmp2->push_back((*tmp1)[i]);
			}
		}
		numObjects = tmp2->size();
		std::vector<NodeV7*>* x = tmp1;
		tmp1 = tmp2;
		tmp2 = x;
		tmp2->clear();
	}
	// At the end both "pointers" (li,ri) point at the same index
	NodeV7* ret = (*tmp1)[0];
	return ret;
}



// Compute indices to the two most separated points of the (up to) six points
// defining the AABB encompassing the point set. Return these as min and max.
void MostSeparatedPointsOnAABB(int &min, int &max, std::vector<Eigen::Vector3f>& pt)
{
	// First find most extreme points along principal axes
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	for (int i = 1; i < pt.size(); i++) {
		if (pt[i][0] < pt[minx][0]) minx = i;
		if (pt[i][0] > pt[maxx][0]) maxx = i;
		if (pt[i][1] < pt[miny][1]) miny = i;
		if (pt[i][1] > pt[maxy][1]) maxy = i;
		if (pt[i][2] < pt[minz][2]) minz = i;
		if (pt[i][2] > pt[maxz][2]) maxz = i;
	}
	// Compute the squared distances for the three pairs of points
	float dist2x = (pt[maxx] - pt[minx]).dot(pt[maxx] - pt[minx]);
	float dist2y = (pt[maxy] - pt[miny]).dot(pt[maxy] - pt[miny]);
	float dist2z = (pt[maxz] - pt[minz]).dot(pt[maxz] - pt[minz]);
	// Pick the pair (min,max) of points most distant
	min = minx;
	max = maxx;
	if (dist2y > dist2x && dist2y > dist2z) {
		max = maxy;
		min = miny;
	}
	if (dist2z > dist2x && dist2z > dist2y) {
		max = maxz;
		min = minz;
	}
}

void SphereFromDistantPoints(Sphere &s, std::vector<Eigen::Vector3f>& pt)
{
	// Find the most separated point pair defining the encompassing AABB
	int min, max;
	MostSeparatedPointsOnAABB(min, max, pt);
	// Set up sphere to just encompass these two points
	s.pos = (pt[min] + pt[max]) * 0.5f;
	s.r = (pt[max] - s.pos).dot(pt[max] - s.pos);
	s.r = std::sqrt(s.r);
}

// Given Sphere s and Point p, update s (if needed) to just encompass p
void SphereOfSphereAndPt(Sphere &s, Eigen::Vector3f &p)
{
	// Compute squared distance between point and sphere center
	Eigen::Vector3f d = p - s.pos;
	float dist2 = d.dot(d);
	// Only update s if point p is outside it
	if (dist2 > s.r * s.r) {
		float dist = std::sqrt(dist2);
		float newRadius = (s.r + dist) * 0.5f;
		float k = (newRadius - s.r) / dist;
		s.r = newRadius;
		s.pos += d * k;
	}
}

void RitterSphere(Sphere &s, std::vector<Eigen::Vector3f>& pt)
{
	// Get sphere encompassing two approximately most distant points
	SphereFromDistantPoints(s, pt);
	// Grow sphere to include all points
	for (int i = 0; i < pt.size(); i++)
		SphereOfSphereAndPt(s, pt[i]);
}

void SplineCollDet::ConstructLeavesBilinearSurfaceV7(SplineModel& splineModel, std::vector<NodeV7>& leaves)
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

		leaves[i].type = LEAF;
		leaves[i].left = NULL;
		leaves[i].right = NULL;

		// Interpolate linear/bilinear
		std::vector<const std::vector<Eigen::Vector3f>*> points;
		points.push_back(&splineModel.controlRadiiPos[i+1]);
		points.push_back(&splineModel.controlRadiiPos[i+2]);
		CalculateAABBMinMax(points, leaves[i].bv.min, leaves[i].bv.max, r.AABBAddFactor);
		// AABBScale(leaves[i].bv, r.AABBScaleFactor);

		leaves[i].splineModel = &splineModel;
		leaves[i].startCPIndex = i;
	}
}

void CalculateAABBMinMax(const std::vector<const std::vector<Eigen::Vector3f>*>& points,
		Eigen::Vector3f& min, Eigen::Vector3f& max, float& AABBAddFactor)
{
	min = max = (*points[0])[0];
	const size_t numPointSets = points.size();
	const size_t numPoints = points[0]->size();
	for (size_t i=0;i<numPointSets;i++)
	{
		for (size_t j=0;j<numPoints;j++)
		{
			if ((*points[i])[j][0] < min[0])
			{
				min[0] = (*points[i])[j][0];
				min[0] -= AABBAddFactor;
			}
			if ((*points[i])[j][1] < min[1])
			{
				min[1] = (*points[i])[j][1];
				min[1] -= AABBAddFactor;
			}
			if ((*points[i])[j][2] < min[2])
			{
				min[2] = (*points[i])[j][2];
				min[2] -= AABBAddFactor;
			}

			if ((*points[i])[j][0] > max[0])
			{
				max[0] = (*points[i])[j][0];
				max[0] += AABBAddFactor;
			}
			if ((*points[i])[j][1] > max[1])
			{
				max[1] = (*points[i])[j][1];
				max[1] += AABBAddFactor;
			}
			if ((*points[i])[j][2] > max[2])
			{
				max[2] = (*points[i])[j][2];
				max[2] += AABBAddFactor;
			}
		}
	}
}	


void LinearInterpolateRadiusAndHash(float* controlRadi, std::vector<Eigen::Vector3f>& splinePoints,
		int nSegments, std::unordered_map<Eigen::Vector3f*, float>& posRadius)
{
	// linear interpolate
	float delta = 1.0f / static_cast<float>(nSegments);

	int intervalIndex = 0;
	for (float t=delta;t<1.0f;t+=delta)
	{
		float radius = (1-t) * controlRadi[1] + t * controlRadi[2];
		posRadius[&(splinePoints[intervalIndex])] = radius;
		++intervalIndex;
	}		
	//std::cout << intervalIndex << std::endl;
}

void BVHCollisionBilinearSurfaceV7(NodeV7* a, NodeV7* b, CollisionResult& result)
{
	if (!TestAABBs(a->bv,b->bv))
		return;
	if (a->type == LEAF && b->type == LEAF)
	{
		if (a->splinePoints.empty())
			CatmullRom::Evaluate(a->controlPoints, a->splinePoints, result.controlPointSegments, 1.0);
		if (b->splinePoints.empty())
		{
			CatmullRom::Evaluate(b->controlPoints, b->splinePoints, result.controlPointSegments, 1.0); 
			LinearInterpolateRadiusBilinearV6(b->splineModel, b->startCPIndex, result.colonRadiiMap, b->splinePoints, result.controlPointSegments);
		}
		DistSampleCPU(a->splinePoints, b->splinePoints, a->radiusRod, result.map); 
	}
	else
	{
		if (a->type == LEAF)
		{
			//if (b->left != NULL)
				BVHCollisionBilinearSurfaceV7(a, b->left, result);
			//if (b->right != NULL)
				BVHCollisionBilinearSurfaceV7(a, b->right, result);
		}
		else
		{
			//if (a->left != NULL)
				BVHCollisionBilinearSurfaceV7(a->left, b, result);
			//if (a->right != NULL)
				BVHCollisionBilinearSurfaceV7(a->right, b, result);
		}
	}
}

void BVHCollisionBilinearSurfaceParallelV7(NodeV7* a, NodeV7* b, CollisionResult& result)
{
	if (!TestAABBs(a->bv,b->bv))
		return;
	if (a->type == LEAF && b->type == LEAF)
	{
		if (a->splinePoints.empty())
			CatmullRom::Evaluate(a->controlPoints, a->splinePoints, 10, 1.0);
		if (b->splinePoints.empty())
		{
			CatmullRom::Evaluate(b->controlPoints, b->splinePoints, 10, 1.0); 
			LinearInterpolateRadiusBilinearV6(b->splineModel, b->startCPIndex, result.colonRadiiMap, b->splinePoints, 10);
		}
		result.pairsToBeSampled[a].push_back(b);
	}
	else
	{
		if (a->type == LEAF)
		{
			//if (b->left != NULL)
				BVHCollisionBilinearSurfaceParallelV7(a, b->left, result);
			//if (b->right != NULL)
				BVHCollisionBilinearSurfaceParallelV7(a, b->right, result);
		}
		else
		{
			//if (a->left != NULL)
				BVHCollisionBilinearSurfaceParallelV7(a->left, b, result);
			//if (a->right != NULL)
				BVHCollisionBilinearSurfaceParallelV7(a->right, b, result);
		}
	}
}

void MapEndoPointsToColonRadii(std::unordered_map<Eigen::Vector3f*, std::pair<float, Eigen::Vector3f*>>& endoColonMap,
		std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f, Eigen::Vector3f>>>& colonRadiiMap,
		std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f, Eigen::Vector3f>>>& endoToColonRadiiMap)
{
	//endo

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

void LinearInterpolateRadiusBilinearV6(const SplineModel* splineModel, const unsigned int startCPIndex,
	std::unordered_map<Eigen::Vector3f*, std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>>& colonRadiiMap,
	std::vector<Eigen::Vector3f>& splinePoints, const unsigned int nSegments)
{
	const float delta = 1.0f / static_cast<float>(nSegments);

	const unsigned int indexP1 = startCPIndex + 1;
	const unsigned int indexP2 = startCPIndex + 2;

	const unsigned int circleSegments = splineModel->controlRadii[indexP1].size();
	unsigned int intervalIndex = 1;

	// need to interpolate positions instead of radi
	float t = delta;

	for (;intervalIndex < nSegments; intervalIndex++)
	{
		colonRadiiMap[&(splinePoints[intervalIndex])] = std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>();
		std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& array =	colonRadiiMap[&(splinePoints[intervalIndex])];

		array.reserve(circleSegments);

		//									0.8 * RadiiPos[P1][0] + 0.2 * RadiiPos[P2][0]
		//									Quanto mais perto de P2, mais influenciado por ele
		Eigen::Vector3f previousRadiusPos = (1-t) * splineModel->controlRadiiPos[indexP1][0] + t * splineModel->controlRadiiPos[indexP2][0];
		Eigen::Vector3f firstPos = previousRadiusPos;
		Eigen::Vector3f radiusPos;

		for (unsigned int i=1; i<circleSegments; i++) // sec. vec
		{
			//					0.8 * RadiiPos[P1][i] + 0.2 * RadiiPos[P2][i]
			//					Quanto mais perto de P2, mais influenciado por ele
			radiusPos = (1-t) * splineModel->controlRadiiPos[indexP1][i] + t * splineModel->controlRadiiPos[indexP2][i];

			//					Carrega o raio anterior e o seu
			array.emplace_back(std::make_pair(previousRadiusPos, radiusPos));
			previousRadiusPos = radiusPos;
		}

		// E o ultimo raio é também o primeiro, pois só pode ser adicionado depois que soubemos qual era o anterior a ele
		array.emplace_back(std::make_pair(previousRadiusPos, firstPos));

		// Raios circulares!!! Que são calculados para todos pontos interpolados!!!

		t += delta;
	}	

	colonRadiiMap[&(splinePoints[0])] = std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>();
	std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& arrayP1 =	colonRadiiMap[&(splinePoints[0])];
	arrayP1.reserve(circleSegments);
	for (unsigned int i=1; i<circleSegments; i++) // sec. vec
	{
		arrayP1.emplace_back(std::make_pair(splineModel->controlRadiiPos[indexP1][i-1], splineModel->controlRadiiPos[indexP1][i]));
	}
	arrayP1.emplace_back(std::make_pair(splineModel->controlRadiiPos[indexP1][0], splineModel->controlRadiiPos[indexP1][circleSegments-1]));

	colonRadiiMap[&(splinePoints[intervalIndex-1])] = std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>();
	std::vector<std::pair<Eigen::Vector3f,Eigen::Vector3f>>& arrayP2 =	colonRadiiMap[&(splinePoints[intervalIndex-1])];
	arrayP2.reserve(circleSegments);
	for (unsigned int i=1; i<circleSegments; i++) // sec. vec
	{
		arrayP2.emplace_back(std::make_pair(splineModel->controlRadiiPos[indexP2][i-1], splineModel->controlRadiiPos[indexP2][i]));
	}
	arrayP2.emplace_back(std::make_pair(splineModel->controlRadiiPos[indexP2][0], splineModel->controlRadiiPos[indexP2][circleSegments - 1]));

}

void SplineCollDet::CollisionCheck(SplineModel endo, SplineModel colon)
{
	endo.TransformPoints();
	colon.TransformPoints();

	r.linePoints.clear();
	r.map.clear();
	r.colonRadiiMap.clear();

	ConstructLeavesBilinearSurfaceV7(endo, leavesA);
	ConstructLeavesBilinearSurfaceV7(colon, leavesB);

	leavesPointer.clear();
	leavesPointer.reserve(leavesA.size());	
	for (NodeV7& pNode : leavesA)
		leavesPointer.push_back(&pNode);	

	NodeV7* rootA = BottomUpBVTreeV7(leavesPointer, nodesA);

	leavesPointer.clear();
	leavesPointer.reserve(leavesB.size());	
	for (NodeV7& pNode : leavesB)
		leavesPointer.push_back(&pNode);	

	NodeV7* rootB = BottomUpBVTreeV7(leavesPointer, nodesB);

	BVHCollisionBilinearSurfaceV7(rootA, rootB, r);

	r.endoRadius = endo.uniformRadius;
	r.CalcCollisionPointsBilinear();
}
