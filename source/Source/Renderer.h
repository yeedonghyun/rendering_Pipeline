#pragma once
#include <stdio.h>
#include <string.h>
#include "Object.h"

class  Edge
{
public:
	Edge() :yMax(0), x(0), xPerY(0), z(0), v(0), u(0), uPerY(0), vPerY(0), zPerY(0), nx(0), ny(0), nz(0), nxPerY(0), nyPerY(0), nzPerY(0), rx(0), ry(0), rz(0), rxPerY(0), ryPerY(0), rzPerY(0) {}
	float yMax;
	float x;
	float xPerY;
	float u, v, z;
	float uPerY, vPerY, zPerY;
	float nx, ny, nz;
	float nxPerY, nyPerY, nzPerY;
	float rx, ry, rz;
	float rxPerY, ryPerY, rzPerY;
};

class Renderer {

public:

	float degree = 100;
	float aspect = checkImageWidth / checkImageHeight;
	float nearZ = 0.1;
	float farZ = 10;

	Renderer()
	{
		m_proj = projectionMatrix(degree, aspect, nearZ, farZ);
		m_screen = ndcToScreenMatrix(checkImageWidth, checkImageHeight);

		v_eye.v[0] = -1.f;
		v_eye.v[1] = 0.f;
		v_eye.v[2] = 1.f;
		v_eye.v[3] = 1.f;

		v_at.v[0] = 0.f;
		v_at.v[1] = 0.f;
		v_at.v[2] = 0.f;
		v_at.v[3] = 1.f;

		v_up.v[0] = 0.f;
		v_up.v[1] = 1.f;
		v_up.v[2] = 0.f;
		v_up.v[3] = 1.f;

		v_light.v[0] = 0.f;
		v_light.v[1] = 0.f;
		v_light.v[2] = 3.f;
		v_light.v[3] = 1.f;
	}

public:

	int numEdge[checkImageHeight] = { 0 };
	Edge ET[checkImageHeight][NUM_MAX_VERTEX];
	Edge AET[NUM_MAX_VERTEX];
	int edgeCount[checkImageHeight] = { 0 };
	int numEdgeInAET = 0;

	float zDepthBuffer[checkImageHeight][checkImageWidth][1];
	float shadowMap[checkImageHeight][checkImageWidth][1];

	Vector4 v_eye, v_at, v_up;
	Vector4 v_light;

	Matrix4 m_view;
	Matrix4 m_proj;
	Matrix4 m_screen;

	Matrix4 lightM_view;
	
	Matrix4 LVP;
	Matrix4 lingtM;
	Matrix4 inversM;

	GLubyte checkImage[checkImageHeight][checkImageWidth][3];
	GLubyte textureImage[checkImageHeight][checkImageWidth][3];
	GLubyte sphereTextureImage[checkImageHeight][checkImageWidth][3];

public:

	void readFileImage(char* pFileName);
	void readSphereImage(char* pFileName);

	void buildEdgeTable(int nFace, Object* pObject);
	void render();
	void render(Object* pObject);
	void translate(float tx, float ty, float tz);
	void rotateX(float degree);
	void rotateY(float degree);
	void rotateZ(float degree);
	void scale(float scale);
	void fill(bool isLighted);
	void shadowRender(Object* pObjec);
	void shadowBuildET(int nFace, Object* pObjec);
	void shadowFill();
	void copyshadowMap();

	void clearEdgeTable();
	void clearZBuffer();
	void clearScreen();

	bool isBackFace(int nFace, Object* pObject);
};