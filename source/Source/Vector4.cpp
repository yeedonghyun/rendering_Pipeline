#include "Matrix4.h"

Vector4 Vector4::operator-(Vector4 Vertex) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] - Vertex.v[i];
	}
	return *this;
}

Vector4 Vector4::operator-=(Vector4 Vertex) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] - Vertex.v[i];
	}
	return *this;
}

Vector4 Vector4::operator+=(Vector4 Vertex) {


	for (int i = 0; i < 4; i++) {
		v[i] = v[i] + Vertex.v[i];
	}
	return *this;
}

Vector4 Vector4::operator/=(float num) {
	
	for (int i = 0; i < 4; i++) {
		v[i] = v[i] /num;
	}
	return *this;

}

Vector4 Vector4::operator+(Vector4 Vertex) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] + Vertex.v[i];
	}
	return *this;
}

Vector4 Vector4::operator*(float num) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] * num;
	}
	return *this;
}

Vector4 Vector4::operator*(int num) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] * num;
	}
	return *this;
}

Vector4 Vector4::operator*(Vector4 Vertex) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] * Vertex.v[i];
	}

	return *this;
}

Vector4 Vector4::operator+(int num) {

	for (int i = 0; i < 4; i++) {
		v[i] = v[i] + num;
	}

	return *this;
}

Vector4 cross(Vector4 v, Vector4 v1) {
	Vector4 res;

	res.v[0] = v.v[1] * v1.v[2] - v.v[2] * v1.v[1];
	res.v[1] = v.v[2] * v1.v[0] - v.v[0] * v1.v[2];
	res.v[2] = v.v[0] * v1.v[1] - v.v[1] * v1.v[0];

	return res;
}

float dotProduct(Vector4 v, Vector4 v1) {
	float res;

	res = v.v[0] * v1.v[0] + v.v[1] * v1.v[1] + v.v[2] * v1.v[2];

	return res;
}

Vector4 normalize(Vector4 v) {

	float vectorLength;

	vectorLength = sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
	
	for (int i = 0; i < 3; i++) {
		if (v.v[i] != 0) {
			v.v[i] = v.v[i] / vectorLength;
		}
	}

	return v;
}