#include <windows.h>	
#include <gl\GL.h>		// 렌더링기능을제공하는라이브러리
#include <gl\GLU.h>		// GL라이르버리의 도우미 역활
#include <gl\glut.h>	// 사용자입력, 화면제어 라이브러리
#include <math.h>
#include <stdio.h>
#include <atlstr.h>		// cstring
#include <fstream>		// file
#include <iostream>
using namespace std;

struct Point {
	float x, y, z;
}typedef Point;
struct Triangle {
	int i1, i2, i3;
}typedef Triangle;

#define MAX  454
#define TMAX 948
Point v[MAX];		//bunny vertex
Point vO[MAX];		//bunny offset vertex
Point nV[MAX];		//bunny vertex normal vector 
Point nT[TMAX];		//Triangle normal vector
Triangle t[TMAX];	//Triangle idx


GLfloat pointLight[4] = { 0.6f, 0, 0.6f, 1 };
GLfloat directLight[4] = { 0, 0.6f ,0.6f, 0 };
GLfloat spotLight[4] = { 0.5f, 0.5f, 0, 1 };

float spotCutoff = 5.0f;
float spotShininess = 1.0f;
float thetaPoint = 0;
float thetaDirect = 0;
float thetaSpot = 0;

int onPoint = -1;
int onDirect = -1;
int onSpot = -1;
int onCutoff = -1;
int onShin = -1;
int onNormal = -1;

bool flagCutoff = false;
bool flagShin = false;
char key;



void AddNormal(int i) {
	float x1, y1, z1;
	float x2, y2, z2;
	float x, y, z;
	float len;

	x1 = v[t[i].i2].x - v[t[i].i1].x;
	y1 = v[t[i].i2].y - v[t[i].i1].y;
	z1 = v[t[i].i2].z - v[t[i].i1].z;

	x2 = v[t[i].i3].x - v[t[i].i1].x;
	y2 = v[t[i].i3].y - v[t[i].i1].y;
	z2 = v[t[i].i3].z - v[t[i].i1].z;

	//cross product 
	x = y1 * z2 - z1 * y2;
	y = z1 * x2 - x1 * z2;
	z = x1 * y2 - y1 * x2;

	//polygon normal vector
	len = sqrt(x*x + y * y + z * z);
	nT[i].x = x / len;
	nT[i].y = y / len;
	nT[i].z = z / len;

	//vertex normal vector
	nV[t[i].i1].x += x;
	nV[t[i].i1].y += y;
	nV[t[i].i1].z += z;

	nV[t[i].i2].x += x;
	nV[t[i].i2].y += y;
	nV[t[i].i2].z += z;

	nV[t[i].i3].x += x;
	nV[t[i].i3].y += y;
	nV[t[i].i3].z += z;

}
void makeNormal_Offset() {
	float len;

	for (int i = 1; i < MAX; i++) {
		len = sqrt(nV[i].x*nV[i].x + nV[i].y*nV[i].y +nV[i].z*nV[i].z);
		nV[i].x = nV[i].x / len;
		nV[i].y = nV[i].y / len;
		nV[i].z = nV[i].z / len;

		vO[i].x = v[i].x + nV[i].x * 0.001;
		vO[i].y = v[i].y + nV[i].y * 0.001;
		vO[i].z = v[i].z + nV[i].z * 0.001;
	}
	
}

void DrawBunny() {

	//draw fill bunny
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMaterialfv(GL_FRONT, GL_AMBIENT, new GLfloat[4]{0,0,0.5f, 1 });
	glMaterialfv(GL_FRONT, GL_DIFFUSE, new GLfloat[4]{ 0,0,1,1 });
	glMaterialfv(GL_FRONT, GL_SPECULAR, new GLfloat[4]{1,1,1,1 });
	glMaterialf(GL_FRONT, GL_SHININESS, 0);
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < TMAX; i++) {

			glVertex3f(v[t[i].i1].x, v[t[i].i1].y, v[t[i].i1].z);
			glVertex3f(v[t[i].i2].x, v[t[i].i2].y, v[t[i].i2].z);
			glVertex3f(v[t[i].i3].x, v[t[i].i3].y, v[t[i].i3].z);

			AddNormal(i);
		}
	}glEnd();

	makeNormal_Offset();

	//draw line bunny
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, new GLfloat[4]{ 0,0,0,1 });
	glMaterialfv(GL_FRONT, GL_DIFFUSE, new GLfloat[4]{ 0,0,0,1 });
	glMaterialfv(GL_FRONT, GL_SPECULAR, new GLfloat[4]{ 1,1,1,1 });
	glMaterialf(GL_FRONT, GL_SHININESS, 0);
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < TMAX; i++) {

			glVertex3f(vO[t[i].i1].x, vO[t[i].i1].y, vO[t[i].i1].z);
			glVertex3f(vO[t[i].i2].x, vO[t[i].i2].y, vO[t[i].i2].z);
			glVertex3f(vO[t[i].i3].x, vO[t[i].i3].y, vO[t[i].i3].z);

		}
	}glEnd();
}

void DrawNormalVector() {		//key=v
	if (onNormal == -1)
		return;
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, new GLfloat[4]{ 0,0,0,1 });
	glMaterialfv(GL_FRONT, GL_DIFFUSE, new GLfloat[4]{ 0,0,0,1 });
	glMaterialfv(GL_FRONT, GL_SPECULAR, new GLfloat[4]{ 1,1,1,1 });
	glMaterialf(GL_FRONT, GL_SHININESS, 0);

	glBegin(GL_LINES);
	{
		for (int i = 1; i < MAX; i++) {
			glColor3f(0, 0, 0);
			glVertex3f(vO[i].x, vO[i].y, vO[i].z);
			glVertex3f(vO[i].x + nV[i].x*0.2, vO[i].y + nV[i].y*0.2, vO[i].z + nV[i].z*0.2);
		}
	}glEnd();

}

void Light() {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GLUquadric *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);


	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);
	glLightfv(GL_LIGHT0, GL_SPECULAR, new GLfloat[4]{ 0,0,0,1 });

	glLightfv(GL_LIGHT1, GL_AMBIENT, new GLfloat[4]{ 0,0,0,1 });
	glLightfv(GL_LIGHT1, GL_DIFFUSE, new GLfloat[4]{ 1,1,1,1 });
	glLightfv(GL_LIGHT1, GL_SPECULAR, new GLfloat[4]{0,0,0,1 });
	GLfloat dirDirecion[4];
	dirDirecion[0] = directLight[0] * -1.0f;
	dirDirecion[1] = directLight[1] * -1.0f;
	dirDirecion[2] = directLight[2] * -1.0f;
	dirDirecion[3] = directLight[3];

	GLfloat specular[4];
	specular[0] = spotShininess;
	specular[1] = spotShininess;
	specular[2] = spotShininess;
	specular[3] = 1;
	glLightfv(GL_LIGHT2, GL_AMBIENT, new GLfloat[4]{ 1,1,1,1 });
	glLightfv(GL_LIGHT2, GL_DIFFUSE, new GLfloat[4]{ 1,1,1,1 });
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotCutoff);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1);
	GLfloat spotDirecion[3];
	spotDirecion[0] = spotLight[0] * -1.0f;
	spotDirecion[1] = spotLight[1] * -1.0f;
	spotDirecion[2] = spotLight[2] * -1.0f;

	glEnable(GL_LIGHTING);	//조명활성화
	glEnable(GL_LIGHT0);	//point light
	glEnable(GL_LIGHT1);	//directional light
	glEnable(GL_LIGHT2);	//spot light



	//point light
	glPushMatrix();
		glRotatef(thetaPoint, 1, 1, 1);
		glTranslatef(pointLight[0], pointLight[1], pointLight[2]);
		glLightfv(GL_LIGHT0, GL_POSITION, pointLight);

		glDisable(GL_LIGHTING);
		glColor3f(1, 0.8, 0);
		gluSphere(quad, 0.01, 36, 18);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	//directional light
	glPushMatrix();
		glRotatef(thetaDirect, 1, 1, 1);
		glTranslatef(directLight[0], directLight[1], directLight[2]);
		glRotatef(135, 0.8, 0, 0);
		glLightfv(GL_LIGHT1, GL_POSITION, dirDirecion);

		glDisable(GL_LIGHTING);
		glColor3f(0.7, 0, 0.5);
		gluCylinder(quad, 0.02, 0.02, 0.05, 36, 18);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	//spot light
	glPushMatrix();
		glRotatef(thetaSpot, 1, 1, 1);
		glTranslatef(spotLight[0], spotLight[1], spotLight[2]);
		glRotatef(5, 1, 1, 0);
		glLightfv(GL_LIGHT2, GL_POSITION, spotLight);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDirecion);
	
		glDisable(GL_LIGHTING);
		glColor3f(0, 0.5, 0.5);
		gluCylinder(quad, 0, spotCutoff/500.0f, 0.1, 20, 1);
		glEnable(GL_LIGHTING);
	glPopMatrix();


	DrawBunny();
	DrawNormalVector();
	glDisable(GL_LIGHTING);
}

void timer(int value) {
	key = '3';
	if ((value == 1) && (onPoint ==1)) {
		thetaPoint += 4.0;
		if (thetaPoint >= 360.0)
			thetaPoint -= 360.0;
		glutTimerFunc(1000 / 30, timer, 1);
	}
	else if ((value == 2) && (onDirect == 1)) {
		thetaDirect += 4.0;
		if (thetaDirect >= 360.0)
			thetaDirect -= 360.0;
		glutTimerFunc(1000 / 30, timer, 2);
	}
	else if ((value == 3) && (onSpot == 1)) {
		thetaSpot += 4.0;
		if (thetaSpot >= 360.0)
			thetaSpot -= 360.0;
		glutTimerFunc(1000 / 30, timer, 3);
	}
	else if ((value == 4) && (onCutoff == 1)) {
		if (flagCutoff == false) {
			if ((spotCutoff >= 5.0f) && (spotCutoff <= 25.0f))
				spotCutoff += 0.5f;
			else {
				spotCutoff -= 0.5f;
				flagCutoff = true;
			}
		}
		else {
			if ((spotCutoff >= 5.0f) && (spotCutoff <= 25.0f))
				spotCutoff -= 0.5f;
			else {
				spotCutoff += 0.5f;
				flagCutoff = false;
			}
		}
		glutTimerFunc(1000 / 30, timer, 4);
	}
	else if ((value == 5) && (onShin == 1)) {
		if (flagShin == false) {
			if ((spotShininess >= 0.0f) && (spotShininess <= 1.0f))
				spotShininess += 0.025f;
			else {
				spotShininess -= 0.025f;
				flagShin = true;
			}
		}
		else {
			if ((spotShininess >= 0.0f) && (spotShininess <= 1.0f))
				spotShininess -= 0.025f;
			else {
				spotShininess += 0.025f;
				flagShin = false;
			}
		}
		glutTimerFunc(1000 / 30, timer, 5);
	}
	glutPostRedisplay();
}
void RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1.0f, 1.0f, 1.0f, 0, 0, 0, 0, 1.0f, 0);

	
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


	switch (key)
	{
	case 'v':
		onNormal *= -1;
		Light();
		break;
	case '3':
		Light();
		break;
	case 'p':
		onPoint *= -1;
		Light();
		glutTimerFunc(1000 / 30, timer, 1);
		break;
	case 'd':
		onDirect *= -1;
		Light();
		glutTimerFunc(1000 / 30, timer, 2);
		break;
	case 's':
		onSpot *= -1;
		Light();
		glutTimerFunc(1000 / 30, timer, 3);
		break;
	case 'c':
		onCutoff *= -1;
		Light();
		glutTimerFunc(1000 / 30, timer, 4);
		break;
	case 'n':
		onShin *= -1;
		Light();
		glutTimerFunc(1000 / 30, timer, 5);
		break;
	default:
		break;
	}
	Light();


	glutSwapBuffers();
	glFlush();
}
void SetupRc(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	char cpath[256];
	CString path;
	int pidx;
	ifstream stream;

	// get path
	GetModuleFileName(NULL, cpath, 256);
	path = cpath;
	pidx = path.ReverseFind('\\');
	path = path.Left(pidx + 1) + "bunny_origin.txt";

	// read file
	stream.open(path);
	stream >> t[0].i1 >> t[0].i2;
	for (int i = 1; i < MAX; i++) {
		stream >> v[i].x >> v[i].y >> v[i].z;
	}
	for (int i = 0; i < TMAX; i++) {
		stream >> t[i].i1 >> t[i].i2 >> t[i].i3;
	}
	stream.close();

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
	case 'v':
	case '3':
	case 'p':
	case 'd':
	case 's':
	case 'c':
	case 'n':
		key = input;
		break;
	default:
		return;
	}
	glutPostRedisplay();
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("2015726044");
	SetupRc();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}
