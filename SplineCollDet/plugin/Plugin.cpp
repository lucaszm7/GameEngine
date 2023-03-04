#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <vector>

#include "Plugin.h"

#include <SplineHelper.h>
#include <AccelerationStructures.h>

void RegisterDebugCallback(DebugCallback callback) {
	if (callback) {
		gDebugCallback = callback;
	}
}


void init() {
  m_result = std::unique_ptr<CollisionResult>(new CollisionResult());
  m_detector = std::unique_ptr<SplineCollDet>(new SplineCollDet());
  m_detector->r.controlPointSegments = 5;

  m_colon = std::unique_ptr<SplineModel>(new SplineModel());
  m_endo = std::unique_ptr<SplineModel>(new SplineModel());
}

void loadSplineModel(const char* filePath) {
  LoadSplineModel(filePath, *m_colon);
  GenerateSurface(*m_colon);
  LoadSplineModel(filePath, *m_endo);
  TransformSplineModelToUniformRadius(*m_endo, 0.005f);
  GenerateSurface(*m_endo);
}

int getControlPointsCount() {
  return m_endo->controlPoints.size();
}

int getVectorsCount() {
  return m_endo->controlRadii[0].size();
}

Vector3 getEndoControlPoint(int index) {
  return m_endo->controlPoints[index];
}

Vector3 getColonControlPoint(int index) {
  return m_colon->controlPoints[index];
}

Vector3 getEndoVectorDir(int iControlPoint, int iVector) {
  return m_endo->controlRadii[iControlPoint][iVector];
}

Vector3 getColonVectorDir(int iControlPoint, int iVector) {
  return m_colon->controlRadii[iControlPoint][iVector];
}

Vector3 getEndoVectorPos(int iControlPoint, int iVector) {
  return m_endo->controlRadiiPos[iControlPoint][iVector];
}

Vector3 getColonVectorPos(int iControlPoint, int iVector) {
  return m_colon->controlRadiiPos[iControlPoint][iVector];
}

void animate(float dt) {
  static float time = 0.0f;
  static Eigen::Vector3f baseCP = m_endo->controlPoints[50];
  float angleSpeed = 3.141592654f/16.0f;
  time += dt * 1000.0f * angleSpeed;
  static Eigen::Vector3f d = Eigen::Vector3f(1,0,0) * 0.01f;
  m_endo->controlPoints[50] = baseCP + d * std::sin(time * 10.0f);
  GenerateSurface(*m_endo);
}

int checkCollision() {
  m_detector->CollisionCheck(*m_endo, *m_colon);

  return m_detector->r.linePoints.size();
}

int getCollisionLinePointsCount() {
  return m_detector->r.linePoints.size();  
}

Vector3 getCollisionLinePoint(int index) {
  return m_detector->r.linePoints[index];
}

void getResult(Spline *spline, int *size) {
  *size = m_endo->controlPoints.size();
  memcpy(spline->controlPoints, m_endo->controlPoints.data(), (*size)*sizeof(Vector3));
}

