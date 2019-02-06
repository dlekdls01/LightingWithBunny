#include <windows.h>	
#include <gl\GL.h>		// 렌더링기능을제공하는라이브러리
#include <gl\GLU.h>		// GL라이르버리의 도우미 역활
#include <gl\glut.h>	// 사용자입력, 화면제어 라이브러리
#include <gl\GLAUX.H>
#include <math.h>
#include <iostream>
#include <atlstr.h>
using namespace std;

#define PI 3.141592f

struct  Point	//점
{
	float x, y, z;
}typedef Point;


Point p[36][18];	//point
CString textureFile[3];
AUX_RGBImageRec *texRec[3];
GLuint texID[3];
GLuint selectID = NULL;
					
//------ Draw 36x18 data point (key=1) -------------//
void Draw_Point() {
	glPointSize(2);
	float x = 0, y = 0, z = 0;
	float radius = 0.4;			//반지름
	float radian = PI / 9;		//각도(360/18도) xy평면에서의원
	float radian1 = PI / 18;	//각도 (360/36도) y축회전하는원

	glColor3f(0, 0, 0);
	glBegin(GL_POINTS);
	{
		///xy평면에원그리기
		for (int j = 0; j < 18; j++) {
			radian += PI / 9;
			p[0][j].x = cos(radian)*radius + 1;
			p[0][j].y = sin(radian)*radius + 1;
			p[0][j].z = 0;
			glVertex3f(p[0][j].x, p[0][j].y, p[0][j].z);
		}
		///y축회전
		for (int i = 1; i < 36; i++) {
			for (int j = 0; j < 18; j++) {
				x = p[i - 1][j].x;
				y = p[i - 1][j].y;
				z = p[i - 1][j].z;

				p[i][j].x = x * cos(radian1) - z * sin(radian1);
				p[i][j].y = y;
				p[i][j].z = x * sin(radian1) + z * cos(radian1);

				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			}
		}
	}
	glEnd();

}

void Draw_Quad() {
	int i, j;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	{
		for (int ti = 0; ti < 36; ti++) {
			for (int tj = 0; tj < 18; tj++) {

				i = ti; j = tj;
				glTexCoord2f(ti/36.0, tj/18.0);
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);

				i = (ti + 1) % 36;
				glTexCoord2f((ti+1) / 36.0, tj / 18.0);
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);

				j = (tj + 1) % 18;
				glTexCoord2f((ti + 1) / 36.0, (tj + 1) / 18.0);
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);

				i = ti;
				glTexCoord2f(ti / 36.0, (tj + 1) / 18.0);
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			}
		}
	}
	glEnd();
}

void LoadTexture() {
	int w=512,  h=512;

	for (int i = 0; i < 3; i++) {

		FILE *file;
		unsigned char *data;
		fopen_s(&file, textureFile[i], "rb");
		if (file == NULL) return;

		data = new unsigned char[w*h * 3];
		fread(data, w*h * 3, 1, file);

		//texRec = auxDIBImageLoad(path);
		glGenTextures(1, &texID[i]);

		glBindTexture(GL_TEXTURE_2D, texID[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		free(data);
		fclose(file);
	}
}
void RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3, 3, 3, 0, 0, 0, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	// x,y,z축
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0); glVertex3f(50, 0, 0);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0); glVertex3f(0, 50, 0);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0); glVertex3f(0, 0, 50);
	}
	glEnd();


	if (selectID == NULL) {
		Draw_Point();
	}
	else if (selectID != NULL) {
		glBindTexture(GL_TEXTURE_2D, selectID);
		Draw_Quad();
	}
	glutSwapBuffers();
}

void SetupRc(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// get path
	CString path;
	char cpath[256];
	int pidx;
	GetModuleFileName(NULL, cpath, 256);
	path = cpath;
	pidx = path.ReverseFind('\\');
	textureFile[0] = path.Left(pidx + 1) + "marble.raw";
	textureFile[1] = path.Left(pidx + 1) + "wood.raw";
	textureFile[2] = path.Left(pidx + 1) + "check.raw";


	LoadTexture();
}
void ChangeSize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio = 1.0*w / h;
	glViewport(0, 0, w, h);			// 렌더링영역 : (0,0)~(w,h) 
	glMatrixMode(GL_PROJECTION);	// 초기화
	glLoadIdentity();
	gluPerspective(60, ratio, 1.0, 400);

}

void keyboard(unsigned char input, int x, int y) {
	switch (input)
	{
	case '1':
		selectID = texID[0];
		break;
	case '2':
		selectID = texID[1];
		break;
	case'3':
		selectID = texID[2];
		break;
	default:
		return;
	}
	glutPostRedisplay();
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("2015726044");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	glutKeyboardFunc(keyboard);

	SetupRc();
	glutMainLoop();
}
