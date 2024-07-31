#pragma once
#include "Matrix4.h"

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(p) assert(p)
#define VERIFY(p) assert(p)
#else
#define ASSERT(p)
#define VERIFY(p) p
#endif

#include "glut.h"

#define NUM_MAX_VERTEX_PER_FACE 3
#define NUM_MAX_VERTEX 10000
#define NUM_MAX_FACE 10000
#define	checkImageWidth 640
#define	checkImageHeight 480

typedef struct {
	int m_nNumVertex;
	unsigned char m_color[3];
	int m_vertex[NUM_MAX_VERTEX_PER_FACE];
	int m_normal[NUM_MAX_VERTEX_PER_FACE];
	int m_uv[NUM_MAX_VERTEX_PER_FACE];
} Face_t;

class Object {
public:

	Object() {
		nNumVertex = 0, nNumFace = 0, nNumUV = 0, nNumNormal = 0;
		m_vertexCount = 0, m_indexCount = 0;
		bHasNormal = false;
		bHasUV = false;
	}

	Matrix4 m_world;

	bool isLighted;

	bool bHasNormal, bHasUV;
	int m_vertexCount, m_indexCount;

	int	nNumVertex, nNumFace, nNumUV, nNumNormal;
	float vertex[NUM_MAX_VERTEX][3];
	float uv[NUM_MAX_VERTEX][3];
	float normal[NUM_MAX_VERTEX][3];
	float worldVertex[NUM_MAX_VERTEX][3];
	float screenVertex[NUM_MAX_VERTEX][4];

	float faceNormal[NUM_MAX_FACE][3];
	float vertexNormal[NUM_MAX_VERTEX][3];
	float worldVertexNormal[NUM_MAX_VERTEX][3];

	float rVector[NUM_MAX_VERTEX][3];
	Face_t face[NUM_MAX_FACE];

	float lightDepth[NUM_MAX_VERTEX][3];	

public:
	void readFile(char* pFileName);
	bool readObjFIle(char* pFileName);

	void makeFaceNormal();
	void makeVertexNormal();
	void makeRVector(Vector4 eye);
	Vector4 makeRVector(Vector4 normal, Vector4 v);
	void makeUV();

	void transformVertices(Matrix4 MVP, Matrix4 m_world);
};