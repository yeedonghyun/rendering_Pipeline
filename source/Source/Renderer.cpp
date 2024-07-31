#include "Renderer.h"

extern Object g_cow;
extern Object g_floor;
extern Object g_skull;
extern Renderer g_renderer;

void Renderer::readFileImage(char* pFileName)
{
	int i, j;
	FILE* in;

	char in_data[checkImageHeight][checkImageWidth][3];

	in = fopen(pFileName, "rb");

	fread(in_data, sizeof(char), checkImageWidth * checkImageHeight * 3, in);

	fclose(in);

	for (i = 0; i < checkImageHeight; i++)
		for (j = 0; j < checkImageWidth; j++) {
			textureImage[i][j][0] = (GLubyte)in_data[i][j][0];
			textureImage[i][j][1] = (GLubyte)in_data[i][j][1];
			textureImage[i][j][2] = (GLubyte)in_data[i][j][2];
		}
}

void Renderer::readSphereImage(char* pFileName)
{
	int i, j;
	FILE* in;

	char in_data1[checkImageHeight][checkImageWidth][3];

	in = fopen(pFileName, "rb");

	fread(in_data1, sizeof(char), checkImageWidth * checkImageHeight * 3, in);

	fclose(in);

	for (i = 0; i < checkImageHeight; i++)
		for (j = 0; j < checkImageWidth; j++) {
			sphereTextureImage[i][j][0] = (GLubyte)in_data1[i][j][0];
			sphereTextureImage[i][j][1] = (GLubyte)in_data1[i][j][1];
			sphereTextureImage[i][j][2] = (GLubyte)in_data1[i][j][2];
		}
}

void Renderer::render()
{
	//1pass
	clearScreen();
	clearZBuffer();

	m_view = lookAtMatrix(g_renderer.v_light, v_at, v_up);

	Matrix4 MVP = m_screen * m_proj * m_view * g_floor.m_world;
	g_floor.transformVertices(MVP, g_floor.m_world);
	g_floor.makeUV();
	g_floor.makeRVector(v_eye);

	MVP = m_screen * m_proj * m_view * g_cow.m_world;
	g_cow.transformVertices(MVP, g_cow.m_world);
	g_cow.makeUV();
	g_cow.makeRVector(v_eye);

	render(&g_cow);
	render(&g_floor);

	copyshadowMap();

	//2pass
	clearScreen();
	clearZBuffer();

	m_view = lookAtMatrix(g_renderer.v_eye, v_at, v_up);

	MVP = m_screen * m_proj * m_view * g_cow.m_world;
	g_cow.transformVertices(MVP, g_cow.m_world);
	g_cow.makeUV();
	g_cow.makeRVector(v_eye);

	MVP = m_screen * m_proj * m_view * g_floor.m_world;
	g_floor.transformVertices(MVP, g_floor.m_world);
	g_floor.makeUV();
	g_floor.makeRVector(v_eye);

	lightM_view = lookAtMatrix(g_renderer.v_light, v_at, v_up);

	lingtM = m_screen * m_proj * lightM_view;
	inversM = Inverse(m_screen * m_proj * m_view);
	LVP = lingtM * inversM;
	
	render(&g_cow);
	//shadowRender(&g_floor);
}

void Renderer::render(Object* pObject)
{
	for (int i = 0; i < pObject->nNumFace; i++)
	{
		if (!isBackFace(i, pObject))
		{
			clearEdgeTable();
			buildEdgeTable(i, pObject);
			fill(pObject->isLighted);
		}
	}
}

void Renderer::shadowRender(Object* pObject) 
{
	for (int i = 0; i < pObject->nNumFace; i++)
	{
		if (!isBackFace(i, pObject))
		{
			clearEdgeTable();
			shadowBuildET(i, pObject);
			shadowFill();
		}
	}
}

void Renderer::buildEdgeTable(int nFace, Object* pObject)
{
	int i, j;
	float scrVertices[2][3];
	float rVertices[2][3];
	float wrd_Nor_Vertices[2][3];
	float verticesUV[2][2];

	for (i = 0; i < pObject->face[nFace].m_nNumVertex; i++)
	{
		float xPerY, uPerY, vPerY, zPerY, savedY;
		float nxPerY, nyPerY, nzPerY;
		float rxPerY, ryPerY, rzPerY;
		int yMin;

		for (j = 0; j < 3; j++) {
			scrVertices[0][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[i] - 1][j];
			scrVertices[1][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[(i + 1) % pObject->face[nFace].m_nNumVertex] - 1][j];
			
			wrd_Nor_Vertices[0][j] = pObject->worldVertexNormal[pObject->face[nFace].m_vertex[i] - 1][j];
			wrd_Nor_Vertices[1][j] = pObject->worldVertexNormal[pObject->face[nFace].m_vertex[(i + 1) % pObject->face[nFace].m_nNumVertex] - 1][j];

			rVertices[0][j] = pObject->rVector[pObject->face[nFace].m_vertex[i] - 1][j];
			rVertices[1][j] = pObject->rVector[pObject->face[nFace].m_vertex[(i + 1) % pObject->face[nFace].m_nNumVertex] - 1][j];
			}

		for (j = 0; j < 2; j++) {
			verticesUV[0][j] = pObject->uv[pObject->face[nFace].m_vertex[i] - 1][j];
			verticesUV[1][j] = pObject->uv[pObject->face[nFace].m_vertex[(i + 1) % pObject->face[nFace].m_nNumVertex] - 1][j];
		}

		if (scrVertices[1][1] == scrVertices[0][1]) {
			continue;
		}

		xPerY = (scrVertices[1][0] - scrVertices[0][0]) / (scrVertices[1][1] - scrVertices[0][1]);
		zPerY = (scrVertices[1][2] - scrVertices[0][2]) / (scrVertices[1][1] - scrVertices[0][1]);

		uPerY = (verticesUV[1][0] - verticesUV[0][0]) / (scrVertices[1][1] - scrVertices[0][1]);
		vPerY = (verticesUV[1][1] - verticesUV[0][1]) / (scrVertices[1][1] - scrVertices[0][1]);

		nxPerY = (wrd_Nor_Vertices[1][0] - wrd_Nor_Vertices[0][0]) / (scrVertices[1][1] - scrVertices[0][1]);
		nyPerY = (wrd_Nor_Vertices[1][1] - wrd_Nor_Vertices[0][1]) / (scrVertices[1][1] - scrVertices[0][1]);
		nzPerY = (wrd_Nor_Vertices[1][2] - wrd_Nor_Vertices[0][2]) / (scrVertices[1][1] - scrVertices[0][1]);

		rxPerY = (rVertices[1][0] - rVertices[0][0]) / (scrVertices[1][1] - scrVertices[0][1]);
		ryPerY = (rVertices[1][1] - rVertices[0][1]) / (scrVertices[1][1] - scrVertices[0][1]);
		rzPerY = (rVertices[1][2] - rVertices[0][2]) / (scrVertices[1][1] - scrVertices[0][1]);

		if (scrVertices[0][1] < scrVertices[1][1])
		{
			savedY = scrVertices[0][1];
			yMin = ceil(scrVertices[0][1]);

			yMin = max(0, yMin);
			yMin = min(479, yMin);

			ET[yMin][edgeCount[yMin]].x = scrVertices[0][0];
			ET[yMin][edgeCount[yMin]].yMax = scrVertices[1][1];
			ET[yMin][edgeCount[yMin]].z = scrVertices[0][2];

			ET[yMin][edgeCount[yMin]].u = verticesUV[0][0];
			ET[yMin][edgeCount[yMin]].v = verticesUV[0][1];

			ET[yMin][edgeCount[yMin]].nx = wrd_Nor_Vertices[0][0];
			ET[yMin][edgeCount[yMin]].ny = wrd_Nor_Vertices[0][1];
			ET[yMin][edgeCount[yMin]].nz = wrd_Nor_Vertices[0][2];

			ET[yMin][edgeCount[yMin]].rx = rVertices[0][0];
			ET[yMin][edgeCount[yMin]].ry = rVertices[0][1];
			ET[yMin][edgeCount[yMin]].rz = rVertices[0][2];

			if (yMin - savedY != 0)
			{
				ET[yMin][edgeCount[yMin]].x += (yMin - savedY) * xPerY;
				ET[yMin][edgeCount[yMin]].u += (yMin - savedY) * uPerY;
				ET[yMin][edgeCount[yMin]].v += (yMin - savedY) * vPerY;
				ET[yMin][edgeCount[yMin]].z += (yMin - savedY) * zPerY;

				ET[yMin][edgeCount[yMin]].nx += (yMin - savedY) * nxPerY;
				ET[yMin][edgeCount[yMin]].ny += (yMin - savedY) * nyPerY;
				ET[yMin][edgeCount[yMin]].nz += (yMin - savedY) * nzPerY;

				ET[yMin][edgeCount[yMin]].rx += (yMin - savedY) * rxPerY;
				ET[yMin][edgeCount[yMin]].ry += (yMin - savedY) * ryPerY;
				ET[yMin][edgeCount[yMin]].rz += (yMin - savedY) * rzPerY;
			}

			ET[yMin][edgeCount[yMin]].xPerY = xPerY;
			ET[yMin][edgeCount[yMin]].zPerY = zPerY;

			ET[yMin][edgeCount[yMin]].uPerY = uPerY;
			ET[yMin][edgeCount[yMin]].vPerY = vPerY;

			ET[yMin][edgeCount[yMin]].nxPerY = nxPerY;
			ET[yMin][edgeCount[yMin]].nyPerY = nyPerY;
			ET[yMin][edgeCount[yMin]].nzPerY = nzPerY;

			ET[yMin][edgeCount[yMin]].rxPerY = rxPerY;
			ET[yMin][edgeCount[yMin]].ryPerY = ryPerY;
			ET[yMin][edgeCount[yMin]].rzPerY = rzPerY;
		}

		else {
			savedY = scrVertices[1][1];
			yMin = ceil(scrVertices[1][1]);

			yMin = max(0, yMin);
			yMin = min(479, yMin);

			ET[yMin][edgeCount[yMin]].x = scrVertices[1][0];
			ET[yMin][edgeCount[yMin]].yMax = scrVertices[0][1];
			ET[yMin][edgeCount[yMin]].z = scrVertices[1][2];

			ET[yMin][edgeCount[yMin]].u = verticesUV[1][0];
			ET[yMin][edgeCount[yMin]].v = verticesUV[1][1];

			ET[yMin][edgeCount[yMin]].nx = wrd_Nor_Vertices[1][0];
			ET[yMin][edgeCount[yMin]].ny = wrd_Nor_Vertices[1][1];
			ET[yMin][edgeCount[yMin]].nz = wrd_Nor_Vertices[1][2];

			ET[yMin][edgeCount[yMin]].rx = rVertices[1][0];
			ET[yMin][edgeCount[yMin]].ry = rVertices[1][1];
			ET[yMin][edgeCount[yMin]].rz = rVertices[1][2];

			if (yMin - savedY != 0)
			{
				ET[yMin][edgeCount[yMin]].x += (yMin - savedY) * xPerY;
				ET[yMin][edgeCount[yMin]].u += (yMin - savedY) * uPerY;
				ET[yMin][edgeCount[yMin]].v += (yMin - savedY) * vPerY;
				ET[yMin][edgeCount[yMin]].z += (yMin - savedY) * zPerY;

				ET[yMin][edgeCount[yMin]].nx += (yMin - savedY) * nxPerY;
				ET[yMin][edgeCount[yMin]].ny += (yMin - savedY) * nyPerY;
				ET[yMin][edgeCount[yMin]].nz += (yMin - savedY) * nzPerY;

				ET[yMin][edgeCount[yMin]].rx += (yMin - savedY) * rxPerY;
				ET[yMin][edgeCount[yMin]].ry += (yMin - savedY) * ryPerY;
				ET[yMin][edgeCount[yMin]].rz += (yMin - savedY) * rzPerY;
			}

			ET[yMin][edgeCount[yMin]].xPerY = xPerY;
			ET[yMin][edgeCount[yMin]].zPerY = zPerY;

			ET[yMin][edgeCount[yMin]].uPerY = uPerY;
			ET[yMin][edgeCount[yMin]].vPerY = vPerY;

			ET[yMin][edgeCount[yMin]].nxPerY = nxPerY;
			ET[yMin][edgeCount[yMin]].nyPerY = nyPerY;
			ET[yMin][edgeCount[yMin]].nzPerY = nzPerY;

			ET[yMin][edgeCount[yMin]].rxPerY = rxPerY;
			ET[yMin][edgeCount[yMin]].ryPerY = ryPerY;
			ET[yMin][edgeCount[yMin]].rzPerY = rzPerY;
		}
		edgeCount[yMin]++;
	}
}

void Renderer::fill(bool isLighted)
{
	int i, j, k;
	//copy
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < numEdgeInAET; j++)
		{
			AET[j].x += AET[j].xPerY;
			AET[j].u += AET[j].uPerY;
			AET[j].v += AET[j].vPerY;
			AET[j].z += AET[j].zPerY;

			AET[j].nx += AET[j].nxPerY;
			AET[j].ny += AET[j].nyPerY;
			AET[j].nz += AET[j].nzPerY;

			AET[j].rx += AET[j].rxPerY;
			AET[j].ry += AET[j].ryPerY;
			AET[j].rz += AET[j].rzPerY;
		}

		//add new Edge
		for (j = 0; j < edgeCount[i]; j++)
		{
			AET[numEdgeInAET + j] = ET[i][j];
		}
		numEdgeInAET += edgeCount[i];

		//delete
		for (j = 0; j < numEdgeInAET; j++)
		{
			if (AET[j].yMax < i)
			{
				for (k = j; k < numEdgeInAET; k++)
					AET[k] = AET[k + 1];
				j--;
				numEdgeInAET--;
			}
		}

		//sort
		Edge temp;
		for (int j = 0; j < numEdgeInAET - 1; j++)
		{
			for (int k = j + 1; k < numEdgeInAET; k++)
			{
				if (AET[j].x > AET[k].x)
				{
					temp = AET[j];
					AET[j] = AET[k];
					AET[k] = temp;
				}
			}
		}

		//fill
		for (j = 0; j < numEdgeInAET; j += 2) {
			int xMin, xMax;
			float uPerX, vPerX, zPerX;
			float deltaU = 0, deltaV = 0, deltaZ = 0;
			float nxPerX, nyPerX, nzPerX;
			float deltaNX = 0, deltaNY = 0, deltaNZ = 0;
			float rxPerX, ryPerX, rzPerX;
			float deltaRX = 0, deltaRY = 0, deltaRZ = 0;

			if (AET[j + 1].x == AET[j].x) {
				continue;
			}

			xMin = floor(AET[j].x);
			xMax = floor(AET[j + 1].x);
			xMin = max(xMin, 0);
			xMax = min(xMax, 639);

			uPerX = (AET[j + 1].u - AET[j].u) / (AET[j + 1].x - AET[j].x);
			vPerX = (AET[j + 1].v - AET[j].v) / (AET[j + 1].x - AET[j].x);
			zPerX = (AET[j + 1].z - AET[j].z) / (AET[j + 1].x - AET[j].x);

			nxPerX = (AET[j + 1].nx - AET[j].nx) / (AET[j + 1].x - AET[j].x);
			nyPerX = (AET[j + 1].ny - AET[j].ny) / (AET[j + 1].x - AET[j].x);
			nzPerX = (AET[j + 1].nz - AET[j].nz) / (AET[j + 1].x - AET[j].x);

			rxPerX = (AET[j + 1].rx - AET[j].rx) / (AET[j + 1].x - AET[j].x);
			ryPerX = (AET[j + 1].ry - AET[j].ry) / (AET[j + 1].x - AET[j].x);
			rzPerX = (AET[j + 1].rz - AET[j].rz) / (AET[j + 1].x - AET[j].x);

			for (k = xMin; k < xMax; k++) {
				if (AET[j].z + deltaZ < g_renderer.zDepthBuffer[i][k][0])
				{
					Vector4 normal;
					normal.v[0] = AET[j].nx + deltaNX;
					normal.v[1] = AET[j].ny + deltaNY;
					normal.v[2] = AET[j].nz + deltaNZ;

					float light;
					light = max(dotProduct(normal, normalize(v_light)), 0.f);

					float s, t, z;
					Vector4 r;
					r.v[0] = AET[j].rx + deltaRX;
					r.v[1] = AET[j].ry + deltaRY;
					r.v[2] = AET[j].rz + deltaRZ;

					z = sqrt(pow(r.v[0], 2) + pow(r.v[1], 2) + pow((r.v[2] + 1), 2));

					s = (r.v[0] / z + 1) * 320;
					t = (r.v[1] / z + 1) * 240;

					if (isLighted) {

						//checkImage[i][k][0] = (GLubyte)255;
						//checkImage[i][k][1] = (GLubyte)0;
						//checkImage[i][k][2] = (GLubyte)0;

						//light = light * 255;
						//checkImage[i][k][0] = (GLubyte)light;
						//checkImage[i][k][1] = (GLubyte)light;
						//checkImage[i][k][2] = (GLubyte)light;

						checkImage[i][k][0] = (GLubyte)(sphereTextureImage[(int)(t)][(int)(s)][0]) ;
						checkImage[i][k][1] = (GLubyte)(sphereTextureImage[(int)(t)][(int)(s)][1]) ;
						checkImage[i][k][2] = (GLubyte)(sphereTextureImage[(int)(t)][(int)(s)][2]) ;

						//checkImage[i][k][0] = (GLubyte)(textureImage[(int)(AET[j].v + deltaV)][(int)(AET[j].u + deltaU)][0]) * max(light, 0.15f);
						//checkImage[i][k][1] = (GLubyte)(textureImage[(int)(AET[j].v + deltaV)][(int)(AET[j].u + deltaU)][1]) * max(light, 0.15f);
						//checkImage[i][k][2] = (GLubyte)(textureImage[(int)(AET[j].v + deltaV)][(int)(AET[j].u + deltaU)][2]) * max(light, 0.15f);

						//checkImage[i][k][0] = (GLubyte)((AET[j].z + deltaZ) * 255);
						//checkImage[i][k][1] = (GLubyte)((AET[j].z + deltaZ) * 255);
						//checkImage[i][k][2] = (GLubyte)((AET[j].z + deltaZ) * 255);

						//normal = (normal + 1) * 127;
						//checkImage[i][k][0] = (GLubyte)normal.v[0];
						//checkImage[i][k][1] = (GLubyte)normal.v[1];
						//checkImage[i][k][2] = (GLubyte)normal.v[2];
					}
					g_renderer.zDepthBuffer[i][k][0] = AET[j].z + deltaZ;
				}
				deltaU += uPerX;
				deltaV += vPerX;
				deltaZ += zPerX;

				deltaNX += nxPerX;
				deltaNY += nyPerX;
				deltaNZ += nzPerX;

				deltaRX += rxPerX;
				deltaRY += ryPerX;
				deltaRZ += rzPerX;
			}
		}
	}
}

void Renderer::shadowBuildET(int nFace, Object* pObject) {
	int i, j;
	float floorVertices[2][3];

	for (i = 0; i < pObject->face[nFace].m_nNumVertex; i++)
	{
		float xPerY, zPerY, savedY;
		int yMin = 0;

		for (j = 0; j < 3; j++) {
			floorVertices[0][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[i] - 1][j];
			floorVertices[1][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[(i + 1) % pObject->face[nFace].m_nNumVertex] - 1][j];
		}

		if (floorVertices[1][1] == floorVertices[0][1]) {
			continue;
		}

		xPerY = (floorVertices[1][0] - floorVertices[0][0]) / (floorVertices[1][1] - floorVertices[0][1]);
		zPerY = (floorVertices[1][2] - floorVertices[0][2]) / (floorVertices[1][1] - floorVertices[0][1]);

		if (floorVertices[0][1] < floorVertices[1][1])
		{
			savedY = floorVertices[0][1];
			yMin = ceil(floorVertices[0][1]);

			yMin = max(0, yMin);
			yMin = min(479, yMin);

			ET[yMin][edgeCount[yMin]].x = floorVertices[0][0];
			ET[yMin][edgeCount[yMin]].yMax = floorVertices[1][1];
			ET[yMin][edgeCount[yMin]].z = floorVertices[0][2];

			if (yMin - savedY != 0)
			{
				ET[yMin][edgeCount[yMin]].x += (yMin - savedY) * xPerY;
				ET[yMin][edgeCount[yMin]].z += (yMin - savedY) * zPerY;
			}
			ET[yMin][edgeCount[yMin]].xPerY = xPerY;
			ET[yMin][edgeCount[yMin]].zPerY = zPerY;
		}

		else {
			savedY = floorVertices[1][1];
			yMin = ceil(floorVertices[1][1]);

			yMin = max(0, yMin);
			yMin = min(479, yMin);

			ET[yMin][edgeCount[yMin]].x = floorVertices[1][0];
			ET[yMin][edgeCount[yMin]].yMax = floorVertices[0][1];
			ET[yMin][edgeCount[yMin]].z = floorVertices[1][2];

			if (yMin - savedY != 0)
			{
				ET[yMin][edgeCount[yMin]].x += (yMin - savedY) * xPerY;
				ET[yMin][edgeCount[yMin]].z += (yMin - savedY) * zPerY;
			}
			ET[yMin][edgeCount[yMin]].xPerY = xPerY;
			ET[yMin][edgeCount[yMin]].zPerY = zPerY;
		}
		edgeCount[yMin]++;
	}
}

void Renderer::shadowFill()
{
	int i, j, k;
	//copy
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < numEdgeInAET; j++)
		{
			AET[j].x += AET[j].xPerY;
			AET[j].z += AET[j].zPerY;

		}

		//add new Edge
		for (j = 0; j < edgeCount[i]; j++)
		{
			AET[numEdgeInAET + j] = ET[i][j];
		}
		numEdgeInAET += edgeCount[i];

		//delete
		for (j = 0; j < numEdgeInAET; j++)
		{
			if (AET[j].yMax < i)
			{
				for (k = j; k < numEdgeInAET; k++)
					AET[k] = AET[k + 1];
				j--;
				numEdgeInAET--;
			}
		}

		//sort
		Edge temp;
		for (int j = 0; j < numEdgeInAET - 1; j++)
		{
			for (int k = j + 1; k < numEdgeInAET; k++)
			{
				if (AET[j].x > AET[k].x)
				{
					temp = AET[j];
					AET[j] = AET[k];
					AET[k] = temp;
				}
			}
		}

		//fill
		for (j = 0; j < numEdgeInAET; j += 2) {
			int xMin, xMax;
			float zPerX;
			float deltaZ = 0;

			if (AET[j + 1].x == AET[j].x) {
				continue;
			}

			xMin = floor(AET[j].x);
			xMax = floor(AET[j + 1].x);
			xMin = max(xMin, 0);
			xMax = min(xMax, 639);

			zPerX = (AET[j + 1].z - AET[j].z) / (AET[j + 1].x - AET[j].x);

			for (k = xMin; k < xMax; k++) {
				if (AET[j].z + deltaZ < g_renderer.zDepthBuffer[i][k][0])
				{
					Vector4 depth;
					int x, y, w;
					float z ;
					depth.v[0] = k;
					depth.v[1] = i;
					depth.v[2] = AET[j].z + deltaZ;
					depth.v[3] = 1.f;

					depth = LVP * depth;

					x = depth.v[0] / depth.v[3];
					y = depth.v[1] / depth.v[3];
					z = depth.v[2] / depth.v[3];
					w = depth.v[3] / depth.v[3];

					if (g_renderer.shadowMap[y][x][0]+0.002 < z ) {
						checkImage[i][k][0] = (GLubyte)0;
						checkImage[i][k][1] = (GLubyte)0;
						checkImage[i][k][2] = (GLubyte)0;
					}

					else {
						checkImage[i][k][0] = (GLubyte)255;
						checkImage[i][k][1] = (GLubyte)255;
						checkImage[i][k][2] = (GLubyte)255;
					}
					g_renderer.zDepthBuffer[i][k][0] = AET[j].z + deltaZ;
				}
				deltaZ += zPerX;
			}
		}
	}
}

void Renderer::copyshadowMap() {
	for (int i = 0; i < checkImageHeight; i++) {
		for (int j = 0; j < checkImageWidth; j++) {
			g_renderer.shadowMap[i][j][0] = g_renderer.zDepthBuffer[i][j][0];
		}
	}
}
	
bool Renderer::isBackFace(int nFace, Object* pObject) {
	float vertex[3][2];
	float res;

	for (int j = 0; j < 2; j++) {
		vertex[0][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[0] - 1][j];
		vertex[1][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[1 % pObject->face[nFace].m_nNumVertex] - 1][j];
		vertex[2][j] = pObject->screenVertex[pObject->face[nFace].m_vertex[2 % pObject->face[nFace].m_nNumVertex] - 1][j];
	}
	res = (((vertex[1][0] - vertex[0][0]) * (vertex[2][1] - vertex[0][1])) - ((vertex[1][1] - vertex[0][1]) * (vertex[2][0] - vertex[0][0])));

	if (res >= 0) {
		return 0;
	}
	else
		return 1;
}

void Renderer::clearEdgeTable() {
	int i;
	for (i = 0; i < 480; i++)
	{
		edgeCount[i] = 0;
	}
	numEdgeInAET = 0;
}

void Renderer::clearZBuffer() {
	for (int i = 0; i < checkImageHeight; i++)
	{
		for (int j = 0; j < checkImageWidth; j++) {
			g_renderer.zDepthBuffer[i][j][0] = 1;
		}
	}
}

void Renderer::clearScreen() {
	for (int i = 0; i < checkImageHeight; i++) {
		for (int j = 0; j < checkImageWidth; j++) {

			checkImage[i][j][0] = 206;
			checkImage[i][j][1] = 242;
			checkImage[i][j][2] = 121;
		}
	}
}

void Renderer::translate(float tx, float ty, float tz) {
	g_cow.m_world = trnasLateMatrix(tx, ty, tz) * g_cow.m_world;
}

void Renderer::rotateX(float degree) {
	Matrix4 m = rotateXMatrix(degree);
	g_cow.m_world = m * g_cow.m_world;
}

void Renderer::rotateY(float degree) {
	Matrix4 m = rotateYMatrix(degree);
	g_cow.m_world = m * g_cow.m_world;
}

void Renderer::rotateZ(float degree) {
	Matrix4 m = rotateZMatrix(degree);
	g_cow.m_world = m * g_cow.m_world;
}

void Renderer::scale(float scale) {
	Matrix4 m;

	m.m_m[0][0] = scale;
	m.m_m[1][1] = scale;
	m.m_m[2][2] = scale;

	g_cow.m_world = m * g_cow.m_world;
}