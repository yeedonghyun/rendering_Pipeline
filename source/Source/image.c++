#include "glut.h"
//#include <stdlib.h>
#include <stdio.h>

/*	Create checkerboard image	*/
#define	checkImageWidth 640
#define	checkImageHeight 480
GLubyte checkImage[checkImageHeight][checkImageWidth][3];
float Vertex1[2] = { 0, 0 }, Vertex2[2] = { 639,479 }, Vertex3[2] = { 0, 479 }, Vertex4[2] = { 300.5, 70.4 }, Vertex5[2] = { 500.8,50.5 }, Vertex6[2] = { 400.8,450.1 };
static GLdouble zoomFactor = 1.0;
static GLint height;

class Edge
{
public:
	int yMax ;
	int x;
	float inverseOfSlope;
};

Edge ET[checkImageHeight][2];
int numEdge[checkImageHeight] = { 0 };

void makeCheckImage(void)
{
	float fTriangle[2][3] = { {0, 639, 0}, {0, 479, 479} }, dSquare[2][4] = { {0, 300.5, 500.8, 400.8}, {0, 70.4, 50.5, 450.1} }; //수정
	float fTXstart = 1000, fTXmax = 0, fTYmax = 0, fTYmin = 1000;
	float fnclination[2];

	for (int i = 0; i < 3; i++) {
		if (fTYmin > fTriangle[1][i]) {
			fTYmin = fTriangle[1][i];
			fTXstart = fTriangle[0][i];
		}
		if (fTYmax < fTriangle[1][i]) {
			fTYmax = fTriangle[1][i];
		}
		if (fTXmax < fTriangle[0][i]) {
			fTXmax = fTriangle[0][i];
		}
	}

	fnclination[0] = (Vertex2[1] - Vertex1[1]) / (Vertex2[0] - Vertex1[0]); //기울기
	fnclination[0] = round(fnclination[0] * 100) / 100; //반올림
	fnclination[1] = 1;

	//첫번째 Et 시작점에서 마지막 x점까지 색을 넣는다
	//첫번째 Et에 들어갈 정보는 X시작점과 다음에 더할 기울기와 Y마지막값이다
	//다음 Et는 x점에 기울기를 더한수고 마지막 x점은 마지막 x점에 기울기를 더한 수다
	//Et에는 Y최대값이 저장되며 Y가 최대값이 될때까지 이걸 반복한다.
	
	for (int i = 0; i < checkImageHeight; i++) {
		
		ET[i][0].yMax = fTYmax;
		ET[i][0].x = fTXstart;
		ET[i][0].inverseOfSlope = fnclination[0];
		
		ET[i][1].yMax = fTYmax;
		ET[i][1].x = fTXmax;
		ET[i][1].inverseOfSlope = fnclination[1];

		fTXstart += fnclination;
	}
	//2개면 될거같은데 많이 생성해야 하는 이유
}
//for (int i = 0; i < 480; i++) {
//	for (int j = 0; j < 640; j++) {
//		 	checkImage[i][j][0] = (GLubyte)0;
//			checkImage[i][j][1] = (GLubyte)0;
//			checkImage[i][j][2] = (GLubyte)255;}}
//		

void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);

   makeCheckImage();
   glRasterPos2i(0, 480);
   glPixelZoom (1.f, -1.f);
   glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB, 
                GL_UNSIGNED_BYTE, checkImage);
   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   height = (GLint) h;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void motion(int x, int y)
{
   static GLint screeny;
   
   screeny = height - (GLint) y;
   glRasterPos2i (x, screeny);
   glPixelZoom (zoomFactor, zoomFactor);
   glCopyPixels (0, 0, checkImageWidth, checkImageHeight, GL_COLOR);
   glPixelZoom (1.0, 1.0);
   glFlush ();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'r':
      case 'R':
         zoomFactor = 1.0;
         glutPostRedisplay();
         printf ("zoomFactor reset to 1.0\n");

         break;
      case 'z':
         zoomFactor += 0.5;
         if (zoomFactor >= 3.0) 
            zoomFactor = 3.0;
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case 'Z':
         zoomFactor -= 0.5;
         if (zoomFactor <= 0.5) 
            zoomFactor = 0.5;
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(p) assert(p)
#define VERIFY(p) assert(p)
#else
#define ASSERT(p)
#define VERIFY(p) p
#endif

#include <string.h>
#define NUM_MAX_VERTEX_PER_FACE 10
#define NUM_MAX_VERTEX 10000
#define NUM_MAX_FACE 1000

typedef struct {
	int m_nNumVertex;
	unsigned char m_color[3];
	int m_vertex[NUM_MAX_VERTEX_PER_FACE];
} Face_t;

void getNewLine(char * buff, int count, FILE* pFile)
{
	fgets(buff, count, pFile);

	while(buff[0] =='#')
		VERIFY(fgets(buff, count, pFile));
}
	
void readFile(char * pFileName)
{

	char buff[512], buff2[512];
	int nNumVertex, nNumFace;
	float vertex[NUM_MAX_VERTEX][3];
	Face_t face[NUM_MAX_FACE];

	int i, j;

	FILE * pFile = fopen(pFileName, "rt");
	getNewLine(buff, 512, pFile);
	sscanf(buff, "%s %d", buff2, &nNumVertex);
	ASSERT(stricmp(buff2, "$Vertex")==0);

	getNewLine(buff, 512, pFile);
	sscanf(buff, "%s %d", buff2, &nNumFace);
	ASSERT(stricmp(buff2, "$Faces")==0);


	for(i=0; i < nNumVertex; i++)
	{
		int nNum;
		getNewLine(buff, 512, pFile);
		sscanf(buff, "%s %d %f %f %f", buff2, &nNum,  &vertex[i][0], &vertex[i][1], &vertex[i][2]);
		sscanf(buff, "%s ", buff2);

		ASSERT(stricmp(buff2, "Vertex")==0);
		ASSERT((nNum-1)==i);
	}


	for(i=0; i < nNumFace; i++)
	{
		int nNum;
		int nCurrPos;

		getNewLine(buff, 512, pFile);
		sscanf(buff, "%s %d %d %d %d %d", buff2, &nNum,  &face[i].m_color[0], &face[i].m_color[1], &face[i].m_color[2], &face[i].m_nNumVertex);
		ASSERT(stricmp(buff2, "Face")==0);
		ASSERT((nNum-1)==i);
		nCurrPos = 0;
		for(j=0; j < 5; j++)
		{
			nCurrPos += strcspn(buff+nCurrPos, " \t");
			nCurrPos += strspn(buff+nCurrPos, " \t");
		}
		for(j=0; j < face[i].m_nNumVertex; j++)
		{
            nCurrPos += strcspn(buff+nCurrPos, " \t");
			nCurrPos += strspn(buff+nCurrPos,  " \t");
			sscanf(buff+nCurrPos, "%d", &face[i].m_vertex[j]);
		}
	}
	

}



int main(int argc, char** argv)
{
	readFile("input.msh");
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutInitWindowSize(640, 480);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMotionFunc(motion);
   glutMainLoop();
   return 0; 
}


