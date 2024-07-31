#include "Renderer.h"

static GLdouble zoomFactor = 1.0;
static GLint height;

Renderer g_renderer;
Object g_floor;
Object g_cow;

void makeCheckImage(void)
{
	g_renderer.render();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	makeCheckImage();
	glRasterPos2i(0, 480);
	glPixelZoom(1.f, -1.f);
	glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB,
		GL_UNSIGNED_BYTE, g_renderer.checkImage);
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	height = (GLint)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void motion(int x, int y)
{
	static GLint screeny;

	screeny = height - (GLint)y;
	glRasterPos2i(x, screeny);
	glPixelZoom(zoomFactor, zoomFactor);
	glCopyPixels(0, 0, checkImageWidth, checkImageHeight, GL_COLOR);
	glPixelZoom(1.0, 1.0);
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	Vector4 y_down(0.f, 1.f, 0.f);
	Vector4 y_up(0.f, -1.f, 0.f);

	Vector4 x_left(1.f, 0.f, 0.f);
	Vector4 x_rigth(-1.f, 0.f, 0.f);

	Vector4 z_up(0.f, 0.f, -0.1f);
	Vector4 z_down(0.f, 0.f, 0.1f);

	switch (key) {

	case 'q':
		g_renderer.v_eye = g_renderer.v_light;
		glutPostRedisplay();
		break;

	case 'w':
		g_renderer.v_eye += y_down;
		glutPostRedisplay();
		break;
	
	case 'W' :
		g_renderer.v_eye += y_down;
		glutPostRedisplay();
		break;

	case 'a':
		g_renderer.v_eye += x_left;
		glutPostRedisplay();
		break;

	case 'A':
		g_renderer.v_eye += x_left;
		glutPostRedisplay();
		break;

	case 's':
		g_renderer.v_eye += y_up;
		glutPostRedisplay();
		break;

	case 'S':
		g_renderer.v_eye += y_up;
		glutPostRedisplay();
		break;

	case 'd':
		g_renderer.v_eye += x_rigth;
		glutPostRedisplay();
		break;

	case 'D': 
		g_renderer.v_eye += x_rigth;
		glutPostRedisplay();
		break;

	case '+':
		g_renderer.v_eye += z_up;
		glutPostRedisplay();
		break;

	case '-':
		g_renderer.v_eye += z_down;
		glutPostRedisplay();
		break;

	case 'x': g_renderer.rotateX(10.f);
		glutPostRedisplay();
		break;

	case 'X': g_renderer.rotateX(-10.f);
		glutPostRedisplay();
		break;

	case 'y': g_renderer.rotateY(10.f);
		glutPostRedisplay();
		break;

	case 'Y': g_renderer.rotateY(-10.f);
		glutPostRedisplay();
		break;

	case 'z': g_renderer.rotateZ(10.f);
		glutPostRedisplay();
		break;

	case 'Z': g_renderer.rotateZ(-10.f);
		glutPostRedisplay();
		break;

	case '2':
		g_renderer.translate(0.f, -0.1, 0.f);
		glutPostRedisplay();
		break;

	case '4':
		g_renderer.translate(0.1, 0.f, 0.f);
		glutPostRedisplay();
		break;

	case '6':
		g_renderer.translate(-0.1, 0.f, 0.f);
		glutPostRedisplay();
		break;

	case '8':
		g_renderer.translate(0.f, 0.1, 0.f);
		glutPostRedisplay();
		break;

	case '1':
		g_renderer.translate(0.f, 0.f, 0.1);
		glutPostRedisplay();
		break;

	case '3':
		g_renderer.translate(0.f, 0.f, -0.1);
		glutPostRedisplay();
		break;

	case 'f':
		g_renderer.scale(0.5);
		glutPostRedisplay();
		break;

	case 'F':
		g_renderer.scale(2);
		glutPostRedisplay();
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);

	g_renderer.readFileImage("Resource/cow1.raw");
	g_renderer.readSphereImage("Resource/sphere3.raw");

	g_cow.readFile("Resource/cow.msh");
	g_cow.makeFaceNormal();
	g_cow.makeVertexNormal();
	g_cow.isLighted = true;

	g_floor.readFile("Resource/bottom.msh");
	g_floor.isLighted = false;

	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}