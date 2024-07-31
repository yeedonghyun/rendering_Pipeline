#pragma once
#include <stdlib.h>
#include <iostream>

using namespace std;

#define  PI 3.1415926

class Vector4 {
public:
	float v[4] ;

public:
	Vector4() {

	}

	Vector4(float array[4])
	{
		for (int i = 0; i < 4; i++)
		{
			v[i] = array[i];
		}
	}

	Vector4(float x, float y, float z, float w)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}

	Vector4(float x, float y, float z)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}

public:
	Vector4 operator -(Vector4 Vertex);
	Vector4 operator +(Vector4 Vertex);
	Vector4 operator -=(Vector4 Vertex);
	Vector4 operator +=(Vector4 Vertex);
	Vector4 operator *(Vector4 Vertex);
	Vector4 operator /=(float num);
	Vector4 operator *(float num);
	Vector4 operator *(int num);
	Vector4 operator +(int num);

public:

	friend Vector4 normalize(Vector4 v);
	friend Vector4 cross(Vector4 v, Vector4 v1); //외적
	friend float dotProduct(Vector4 v, Vector4 v1); //내적
}; 