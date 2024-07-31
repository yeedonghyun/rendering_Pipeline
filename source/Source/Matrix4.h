#pragma once
#include <cmath>
#include "Vector4.h"

using namespace std;

class Matrix4
{
public:
	float m_m[4][4];

public:
	Vector4 operator *(Vector4 Vertex);
	Matrix4 operator *(Matrix4 Matrix);

public:

	Matrix4() {
		
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
					m_m[i][j] = 1;
				else
					m_m[i][j] = 0;
			}
	}

	Matrix4(float array[4][4])
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				m_m[i][j] = array[i][j];
			}
	}

public:
	friend Matrix4 Inverse(Matrix4 m);

	friend Matrix4 lookAtMatrix(Vector4 eye, Vector4 at, Vector4 up);
	friend Matrix4 rotateXMatrix(float degree);
	friend Matrix4 rotateYMatrix(float degree);
	friend Matrix4 rotateZMatrix(float degree);

	friend Matrix4 rotationMatrix(Vector4 forword, Vector4 side, Vector4 upVector);
	friend Matrix4 trnasLateMatrix(float tx, float ty, float tz);

	friend Matrix4 projectionMatrix(float degree, float aspect, float nearZ, float farZ);
	friend Matrix4 ndcToScreenMatrix(int w, int h);
};
