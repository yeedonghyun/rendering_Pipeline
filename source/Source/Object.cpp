#include <vector>
#include <fstream>
#include "Renderer.h"

extern Renderer g_renderer;

void getNextLine(char* buff, int count, FILE* pFile)
{
	fgets(buff, count, pFile);

	while (buff[0] == '#' || buff[0] == '\n')
		VERIFY(fgets(buff, count, pFile));
}

void Object::readFile(char* pFileName) //º¯°æ
{
	char buff[512], buff2[512];
	int i, j;

	FILE* pFile = fopen(pFileName, "rt");
	getNextLine(buff, 512, pFile);
	sscanf(buff, "%s %d", buff2, &nNumVertex);
	ASSERT(stricmp(buff2, "$Vertex") == 0);

	getNextLine(buff, 512, pFile);
	sscanf(buff, "%s %d", buff2, &nNumFace);
	ASSERT(stricmp(buff2, "$Faces") == 0);

	for (i = 0; i < nNumVertex; i++)
	{
		int nNum;
		getNextLine(buff, 512, pFile);
		sscanf(buff, "%s %d %f %f %f", buff2, &nNum, &vertex[i][0], &vertex[i][1], &vertex[i][2]);
		sscanf(buff, "%s ", buff2);

		ASSERT(stricmp(buff2, "Vertex") == 0);
		ASSERT((nNum - 1) == i);
	}

	for (i = 0; i < nNumFace; i++)
	{
		int nNum;
		int nCurrPos;

		getNextLine(buff, 512, pFile);
		sscanf(buff, "%s %d", buff2, &nNum);
		face[i].m_nNumVertex = 3;

		ASSERT(stricmp(buff2, "Face") == 0);
		ASSERT((nNum - 1) == i);
		nCurrPos = 0;
		for (j = 0; j < 1; j++)
		{
			nCurrPos += strcspn(buff + nCurrPos, " \t");
			nCurrPos += strspn(buff + nCurrPos, " \t");
		}

		for (j = 0; j < face[i].m_nNumVertex; j++)
		{
			nCurrPos += strcspn(buff + nCurrPos, " \t");
			nCurrPos += strspn(buff + nCurrPos, " \t");
			sscanf(buff + nCurrPos, "%d", &face[i].m_vertex[j]);
		}
	}
}

bool Object::readObjFIle(char* pFileName)
{
	char text[256];

	int faceVertex[15], UV, nomal;
	char Num;

	vector<string>line;

	ifstream pFile(pFileName);

	if (!pFile)
		return false;

	while (!pFile.eof())
	{
		pFile.getline(text, 256);
		line.push_back(text);
	}

	for(int i = 0; i < line.size(); i++)
	{
		if (line[i][0] == 'v' && line[i][1] == ' ') {
			sscanf_s(line[i].c_str(), "v %f %f %f", &vertex[nNumVertex][0], &vertex[nNumVertex][1], &vertex[nNumVertex][2]);
			nNumVertex++;
		}

		else if (line[i][0] == 'v' && line[i][1] == 't') {
			sscanf_s(line[i].c_str(), "vt %f %f", &uv[nNumUV][0], &uv[nNumUV][1]);
			nNumUV++;
			bHasUV = true;

		}
		else if (line[i][0] == 'v' && line[i][1] == 'n') {
			sscanf_s(line[i].c_str(), "vn %f %f %f", &vertexNormal[nNumNormal][0], &vertexNormal[nNumNormal][1], &vertexNormal[nNumNormal][2]);
			nNumNormal++;
			bHasNormal = true;
		}

		else if (i < line.size() && line[i][0] == 'f') {

			int j = 0, numVertex = 0;

			for (; j < line[i].size(); j++) {
				if (line[i][j] == ' ') {
					numVertex++;
				}
			}

			if (line[i][j-1] == ' ') {
				numVertex--;
			}

			face[nNumFace].m_nNumVertex = numVertex;
			int faceVerNum = 0, faceVerNorNum = 0, faceUVNum =0;

			if (line[i][1] == ' ' && bHasUV && bHasNormal) {

				for (int j = 1; j < line[i].size(); j++) {
					if (line[i][j] == ' ') {
						sscanf_s(&line[i][j + 1], "%d/%d/%d", &faceVertex[faceVerNum], &UV, &nomal);
						faceVerNum++;
					}
				}
				for (int j = 0; j < face[nNumFace].m_nNumVertex; j++) {
					face[nNumFace].m_vertex[j] = faceVertex[j];
				}
			}

			else if (line[i][1] == ' ' && !bHasUV && bHasNormal) {
				for (int j = 1; j < line[i].size(); j++) {
					if (line[i][j] == ' ') {
						sscanf_s(&line[i][j + 1], "%d//%d", &faceVertex[faceVerNum], &nomal);
						faceVerNum++;
					}
				}
				for (int j = 0; j < face[nNumFace].m_nNumVertex; j++) {
					face[nNumFace].m_vertex[j] = faceVertex[j];
				}
			}

			else {
				for (int j = 1; j < line[i].size(); j++) {
					if (line[i][j] == ' ') {
						sscanf_s(&line[i][j + 1], "%d", &faceVertex[faceVerNum]);
						faceVerNum++;
					}
				}
				for (int j = 0; j < face[nNumFace].m_nNumVertex; j++) {
					face[nNumFace].m_vertex[j] = faceVertex[j];
				}
			}



			nNumFace++;
		}
	}
	pFile.close();

	return true;
}

void Object::makeFaceNormal() {

	Vector4 v, v1, v2;
	for (int i = 0; i < nNumFace; i++) {

		for (int j = 0; j < 3; j++) {
			v.v[j] = vertex[face[i].m_vertex[0] - 1][j] - vertex[face[i].m_vertex[1] - 1][j];
			v1.v[j] = vertex[face[i].m_vertex[0] - 1][j] - vertex[face[i].m_vertex[2] - 1][j];
		}

		v2 = cross(v, v1);
		v2 = normalize(v2);

		for (int j = 0; j < 3; j++) {
			faceNormal[i][j] = v2.v[j];
		}
	}
}

void Object::makeVertexNormal() {
	Vector4 v;

	for (int i = 0; i < nNumFace; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				vertexNormal[face[i].m_vertex[j] - 1][k] += faceNormal[i][k];
			}
		}
	}

	for (int i = 0; i < nNumVertex; i++) {
		v = vertexNormal[i];
		v = normalize(v);

		for (int j = 0; j < 3; j++) {
			vertexNormal[i][j] = v.v[j];
		}
	}
}

void Object::makeRVector(Vector4 eye) {
	Vector4 normal, v, e, r;
	e = eye;
	for (int i = 0; i < nNumVertex; i++) {

		normal = worldVertexNormal[i];
		v = normalize(e - worldVertex[i]);

		r = makeRVector(normal, v);

		for (int j = 0; j < 3; j++) {
			rVector[i][j] = r.v[j];
		}
	}
}

Vector4 Object::makeRVector(Vector4 normal, Vector4 v) {
	float cos;
	Vector4 s, r;

	cos = dotProduct(normal, v);
	s = normal * cos;
	r = (s * 2) - v;

	return r;
}

void Object::makeUV() {
	for (int i = 0; i < nNumVertex; i++) {
		Vector4 v1(vertex[i]);

		uv[i][0] = (v1.v[0] + 1) * 320;
		uv[i][1] = (v1.v[1] + 1) * 240;
	}
}

void Object::transformVertices(Matrix4 MVP, Matrix4 m_world) {

	for (int i = 0; i < nNumVertex; i++) {

		Vector4 v_scr(vertex[i]);
		Vector4 v_normal(vertexNormal[i]);
		Vector4 v_world(vertex[i]);

		v_scr.v[3] = 1.f;
		v_normal.v[3] = 1.f;
		v_world.v[3] = 1.f;

		v_scr = MVP * v_scr;
		v_normal = m_world * v_normal;
		v_world = m_world * v_world;

		worldVertex[i][0] = v_world.v[0];
		worldVertex[i][1] = v_world.v[1];
		worldVertex[i][2] = v_world.v[2];

		worldVertexNormal[i][0] = v_normal.v[0];
		worldVertexNormal[i][1] = v_normal.v[1];
		worldVertexNormal[i][2] = v_normal.v[2];

		screenVertex[i][0] = v_scr.v[0] / v_scr.v[3];
		screenVertex[i][1] = v_scr.v[1] / v_scr.v[3];
		screenVertex[i][2] = v_scr.v[2] / v_scr.v[3];
		screenVertex[i][3] = v_scr.v[3] / v_scr.v[3];
	}
}