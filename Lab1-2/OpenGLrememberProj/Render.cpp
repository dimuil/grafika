#include "Render.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("kasochka.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





 void DrawFigure(double Figure[8][3]); //� ���������
double* CalculateNorm(double v1[], double v2[], double v3[]);

void DrawPolygon(double v1[], double v2[], double v3[], double v4[], int flag=0)
{
	glColor3d(0.1488, 0.228, 0.322);
	glBegin(GL_POLYGON);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glVertex3dv(v3);
	glVertex3dv(v4);
	glEnd();
}

void DrawSquare(double v1[], double v2[], double v3[], double v4[], int flag=0, int i=0, int n=0)
{
	
	glBegin(GL_QUADS);
	if(flag==0)
		glTexCoord2d(1, 1);
	else
		glTexCoord2d((i+0.0)/n,1);
	glVertex3dv(v1);
	if (flag == 0)
		glTexCoord2d(0, 1);
	else
		glTexCoord2d((i -1.0)/ n +0.0, 1);
	glVertex3dv(v2);
	if (flag == 0)
		glTexCoord2d(0, 0);
	else
		glTexCoord2d((i-1.0) / n +0.0, 0);
	glVertex3dv(v3);
	if (flag == 0)
		glTexCoord2d(1, 0);
	else
		glTexCoord2d((i +0.0) / n +0.0, 0);
	glVertex3dv(v4);
	glEnd();
}

void DrawWall(double v1[], double v2[], int flag = 0, int i=0,int n=0)
{
	double tmp1[3];
	double tmp2[3];
	tmp1[0] = v1[0];
	tmp1[1] = v1[1];
	tmp1[2] = v1[2] - 4;
	tmp2[0] = v2[0];
	tmp2[1] = v2[1];
	tmp2[2] = v2[2] - 4;
	glNormal3d(CalculateNorm(v1, v2, tmp1)[0], CalculateNorm(v1, v2, tmp1)[1], CalculateNorm(v1, v2, tmp1)[2]);
	DrawSquare(v1, v2, tmp2, tmp1, flag,i,n);
}

void DrawWalls(double Figure[8][3], int n)
{
	for (int i = 0; i <= n-1; i++)
	{
		
			if (i == n-1)
				DrawWall(Figure[i], Figure[0]);
			else
				DrawWall(Figure[i], Figure[i + 1]);

	}
}


void DrawTriangle(double v1[], double v2[], double v3[], int flag = 0)
{
	glBegin(GL_TRIANGLES);
	if(flag!=0)
		glTexCoord2d(v1[0]/11.0, v1[1]/11.0);
	glVertex3dv(v1);
	if (flag != 0)
		glTexCoord2d(v2[0] / 11.0, v2[1] / 11.0);
	glVertex3dv(v2);
	if (flag != 0)
		glTexCoord2d(v3[0] / 11.0, v3[1] / 11.0);
	glVertex3dv(v3);
	glEnd();
}

void DrawHalfOfCircle(double v2[], double v3[], int m, double l)
{
	double v1[3];
	double v4[3];
	double r;
	double n = 1000;
	double k;
	if ((v2[0] - v3[0]) != 0)
		k = (v2[1] - v3[1]) / (v2[0] - v3[0]);
	else
		k = tan(M_PI / 2.0);
	v1[0] = (v3[0] + v2[0]) / 2;
	v1[1] = (v3[1] + v2[1]) / 2;
	v1[2] = (v3[2] + v2[2]) / 2;
	r = sqrt(pow(v3[0] - v2[0], 2) + pow(v3[1] - v2[1], 2) + pow(v3[2] - v2[2], 2)) / 2;
	for (int i = 1; i <= n; i++)
	{
		v4[0] = r * cos((atan(k) + l * ((M_PI) / n) * i)) + v1[0];
		v4[1] = r * sin((atan(k) + l * ((M_PI) / n) * i)) + v1[1];
		v4[2] = m;
		glNormal3d(CalculateNorm(v2, v1, v4)[0], CalculateNorm(v2, v1, v4)[1], CalculateNorm(v2, v1, v4)[2]);
		DrawTriangle(v2, v1, v4,1);
		DrawWall(v4, v2,1,i,n);
		v1[2] = v1[2] - m;
		v2[2] = v2[2] - m;
		v4[2] = v4[2] - m;
		glNormal3d(CalculateNorm(v2, v4, v1)[0], CalculateNorm(v2, v4, v1)[1], CalculateNorm(v2, v4, v1)[2]);
		DrawTriangle(v2, v1, v4,1);
		v1[2] = v1[2] + m;
		v2[2] = v2[2] + m;
		v4[2] = v4[2] + m;
		v2[0] = v4[0];
		v2[1] = v4[1];
		v2[2] = v4[2];
	}
}

double* CalculateNorm(double v1[], double v2[], double v3[])
{
	
	double a[] = { v1[0] - v2[0],v1[1] - v2[1],v1[2] - v2[2] };
	double b[] = { v3[0] - v2[0],v3[1] - v2[1],v3[2] - v2[2] };;
	double Result[] = {a[1]*b[2]-b[1]*a[2],(-1.0*a[0]*b[2]+b[0]*a[2]),a[0]*b[1]-b[0]*a[1]};
	for (int i = 0; i < 3; i++)
		Result[i] = Result[i] / (sqrt(pow(Result[0], 2) + pow(Result[1], 2) + pow(Result[2], 2)));
	return &Result[0];
}

void DrawCutted(double v1[], double v2[], double v3[])
{
	int n = 1000;
	double count = 1000 / 4;
	int k = 1;
	double tmp1[3];
	double tmp2[3];
	double O[] = { 3,4,(v1[2] + v2[2]) / 2 }; //���������� ����������� �� ������� ��������� ����������� �� ������ �������� ���������
	double r = 3.162277660168379;//������ ����������� �� ������ �������� ���������
	tmp1[2] = (v1[2] + v2[2]) / 2;
	tmp2[2] = (v1[2] + v2[2]) / 2;
	for(int i=1; i<=n;i++)
	{
		tmp1[0] = r * cos((2.0 * M_PI / n) * i) + O[0];
		tmp1[1] = r * sin((2.0 * M_PI / n) * i) + O[1];
		tmp2[0] = r * cos((2.0 * M_PI / n) * (i + 1.0)) + O[0];
		tmp2[1] = r * sin((2.0 * M_PI / n) * (i + 1.0)) + O[1];
		if ((tmp1[1] >= -0.5 * tmp1[0] + 8) && (tmp2[1] >= -0.5 * tmp2[0] + 8))
		{
			if (((tmp1[2] + tmp2[2]) / 2) == 4)
				glNormal3d(CalculateNorm(tmp1, tmp2,v3)[0], CalculateNorm(tmp1, tmp2, v3)[1], CalculateNorm(tmp1, tmp2, v3)[2]);
			else
				glNormal3d(CalculateNorm(tmp1,v3, tmp2)[0], CalculateNorm(tmp1, v3, tmp2)[1], CalculateNorm(tmp1, v3, tmp2)[2]);
			DrawTriangle(tmp1, tmp2, v3,1);
			if (((tmp1[2] + tmp2[2]) / 2) == 4)
			{
				DrawWall(tmp1, tmp2, 1, k, count);
				k++;
			}
		}
	}
}

void DrawFigure(double Figure[8][3])
{
	glNormal3d(CalculateNorm(Figure[0], Figure[1],Figure[2])[0], CalculateNorm(Figure[0], Figure[1], Figure[2])[1], CalculateNorm(Figure[0], Figure[1], Figure[2])[2]);
	glNormal3d(CalculateNorm(Figure[0], Figure[1], Figure[2])[0], CalculateNorm(Figure[0], Figure[1], Figure[2])[1], CalculateNorm(Figure[0], Figure[1], Figure[2])[2]);
	DrawPolygon(Figure[2], Figure[3], Figure[4], Figure[5]);
	DrawTriangle(Figure[1], Figure[2], Figure[5]);
	DrawTriangle(Figure[0], Figure[1], Figure[5]);
}




void DrawUpperFigure(double Figure[8][3])
{
	for (int i = 0; i < 6; i++)
		Figure[i][2] = Figure[i][2] + 4;
	glNormal3d(-1.0 * CalculateNorm(Figure[0], Figure[1], Figure[2])[0], -1.0 * CalculateNorm(Figure[0], Figure[1], Figure[2])[1],-1.0* CalculateNorm(Figure[0], Figure[1], Figure[2])[2]);
	DrawPolygon(Figure[2], Figure[3], Figure[4], Figure[5]);
	DrawTriangle(Figure[1], Figure[2], Figure[5]);
	DrawTriangle(Figure[0], Figure[1], Figure[5]);

}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

    double Figure[8][3] = { { 8,3,0 },{ 16,8,0 },{ 10,12,0 },{ 4,8,0 },{ 4,5,0 },{ 11,9,0 } };   //� ���������
	double M[3] = { 4,7,0 };
	//glBindTexture(GL_TEXTURE_2D, texId);
	DrawFigure(Figure);
	DrawUpperFigure(Figure);
	DrawWalls(Figure, 6);
	//DrawHalfOfCircle(Figure[5], Figure[4], 4, 1);
	//������ ��������� ���������� ��������



	
		
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}