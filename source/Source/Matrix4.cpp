#include "Renderer.h"

extern Renderer g_renderer;

Matrix4 trnasLateMatrix(float tx, float ty, float tz) {

	float m[4][4] = { {1,0,0,tx}, {0,1,0,ty}, {0,0,1,tz}, {0,0,0,1} };
	Matrix4 res(m);
	return res;
}

Matrix4 ndcToScreenMatrix(int w, int h) {
	Matrix4 m;

	m.m_m[0][0] = -w / 2;
	m.m_m[0][3] = w / 2;

	m.m_m[1][1] = -h / 2;
	m.m_m[1][3] = h / 2;

	return m;
}

Matrix4 rotateZMatrix(float degree) {

	Matrix4 m;

	float c, s;

	c = cos((PI * degree) / 180);
	s = sin((PI * degree) / 180);

	m.m_m[0][0] = c;
	m.m_m[0][1] = -s;

	m.m_m[1][0] = s;
	m.m_m[1][1] = c;

	return m;
}

Matrix4 rotateXMatrix(float degree) {

	Matrix4 m;

	float c, s;

	c = cos((PI * degree) / 180);
	s = sin((PI * degree) / 180);

	m.m_m[1][1] = c;
	m.m_m[1][2] = -s;

	m.m_m[2][1] = s;
	m.m_m[2][2] = c;

	return m;
}

Matrix4 rotateYMatrix(float degree) {

	Matrix4 m;

	float c, s;

	c = cos((PI * degree) / 180);
	s = sin((PI * degree) / 180);

	m.m_m[0][0] = c;
	m.m_m[0][2] = s;

	m.m_m[2][0] = -s;
	m.m_m[2][2] = c;

	return m;
}

Matrix4 rotationMatrix(Vector4 u, Vector4 v, Vector4 n)
{
	Matrix4 m;

	for (int i = 0; i < 3; i++) {
		m.m_m[0][i] = u.v[i];
		m.m_m[1][i] = v.v[i];
		m.m_m[2][i] = n.v[i];
	}
	return m;
}

Vector4 Matrix4::operator*(Vector4 Vertex) { //Matrix * vertex

	Vector4 v;

	for (int i = 0; i < 4; i++) {
		v.v[i] = 0;
		for (int j = 0; j < 4; j++) {
			v.v[i] += (m_m[i][j] * Vertex.v[j]);
		}

	}
	return v;
}

Matrix4 Matrix4::operator*(Matrix4 Matrix) { //Matrix * Matrix

	Matrix4 m;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m.m_m[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				m.m_m[i][j] += m_m[i][k] * Matrix.m_m[k][j];
			}
		}
	}
	return m;
}

Matrix4 translationMatrix(Vector4 eye, Vector4 n, Vector4 u, Vector4 v) {
	Matrix4 m;

	m.m_m[0][3] = dotProduct(eye, u);
	m.m_m[1][3] = dotProduct(eye, v);
	m.m_m[2][3] = dotProduct(eye, n);

	return m;
}

Matrix4 lookAtMatrix(Vector4 eye, Vector4 at, Vector4 up)
{
	Matrix4 r, t;
	Vector4 n, u, v;

	n = eye - at; //eye(카메라 좌표)
	n = normalize(n);

	u = cross(up, n); //side x축의 방향을 나타내는 벡터
	u = normalize(u);

	v = cross(n, u); //up(카메라의 위쪽방향)

	r = rotationMatrix(u, v, n); //뷰행렬
	t = trnasLateMatrix(-eye.v[0], -eye.v[1], -eye.v[2]);

	return r * t;
}

Matrix4 projectionMatrix(float degree, float aspect, float nearZ, float farZ) {
	Matrix4 m;

	float fovY;
	fovY = (degree * PI) / 180;

	m.m_m[0][0] = (1 / tan(fovY / 2)) / aspect;
	m.m_m[1][1] = 1 / tan(fovY / 2);

	m.m_m[2][2] = -farZ / (farZ - nearZ);

	m.m_m[2][3] = -(nearZ * farZ) / (farZ - nearZ);
	m.m_m[3][2] = -1;
	m.m_m[3][3] = 0;

	return m;
}

Matrix4 Inverse(Matrix4 m)
{
	float Result[4][4];
	float tmp[12];
	float src[16];
	float det;
	
	Matrix4 res;

    for (int i = 0; i < 4; i++)
    {
        src[i + 0] = m.m_m[i][0];
        src[i + 4] = m.m_m[i][1];
        src[i + 8] = m.m_m[i][2];
        src[i + 12] = m.m_m[i][3];
    }

    tmp[0] = src[10] * src[15];
    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];

    Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
    Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
    Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
    Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
    Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
    Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
    Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
    Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
    Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
    Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
    Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
    Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
    Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
    Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
    Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
    Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];

    tmp[0] = src[2] * src[7];
    tmp[1] = src[3] * src[6];
    tmp[2] = src[1] * src[7];
    tmp[3] = src[3] * src[5];
    tmp[4] = src[1] * src[6];
    tmp[5] = src[2] * src[5];

    tmp[6] = src[0] * src[7];
    tmp[7] = src[3] * src[4];
    tmp[8] = src[0] * src[6];
    tmp[9] = src[2] * src[4];
    tmp[10] = src[0] * src[5];
    tmp[11] = src[1] * src[4];

    Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
    Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
    Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
    Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
    Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
    Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
    Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
    Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
    Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
    Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
    Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
    Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
    Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
    Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
    Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
    Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];

    det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
    det = 1.0f / det;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
			res.m_m[i][j] = Result[i][j] * det;
        }
    }
    return res;
}