#include"stdafx.h"
#include <math.h>  
#include <stdio.h>  
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<time.h>
#include <iomanip>
#include<queue>
#include <unordered_set>   
#include <vector>
#include <unordered_set>

#include "particle.h"

#define BMP_Header_Length 54
#define PI 3.14159
#define YU 1

TextureImage texture[10];
static float c = PI / 180.0f; //弧度和角度转换参数  
static int  oldmy = -1, oldmx = -1; //du是视点绕y轴的角度,opengl里默认y轴是上方向  
static float dv = 0;
static float du = PI / 2;
static float radius = 0.5;
static float r = 1.5f, h = 0.0f; //r是视点绕y轴的半径,h是视点高度即在y轴上的坐标  
static float viewConst = 70;
static float dx = 0.0;
static float dy = 0.0;
static float oldViewX = 0.0;
static float oldViewY = 0.0;
static float viewPointConst = 0.5;
static int moveStep = 50;
static bool morphStart = false;
static int currStep = 0;
static bool hehe = true;
static bool finishedMorph = false;
Loader loader;
Loader loader2;
Bi_loader bi_loader;
vector<Index>srcIndex;
vector<Vertex>src;
vector<StepData>singleMove;
vector<Vertex>interm;
vector<Index>dstIndex;
unordered_set<int>check;
vector<GLuint>tex;
vector<GLuint>tex2;
vector<TriangleEdge*>mappin;
vector<Vertex> curr_pos=
{ Vertex(0,0,0),Vertex(0,0,0) ,Vertex(0,0,0) ,Vertex(0,0,0) ,Vertex(0,0,0) ,Vertex(0,0,0) };
vector<Vertex> shoot_dir =
{ Vertex(0,0,0),Vertex(0,0,0) ,Vertex(0,0,0) ,Vertex(0,0,0) ,Vertex(0,0,0) ,Vertex(0,0,0) };
#define CAL (dx*dx+dy*dy+viewConst*viewConst*r*r)
#define NEWR (viewConst*r)




Vertex getMiddlePoint(Vertex v1, Vertex v2)
{
	return Vertex((v1.X + v2.X) / 2, (v1.Y + v2.Y) / 2, (v1.Z + v2.Z) / 2);
}

float getdistance(Vertex v1, Vertex v2)
{
	return sqrtf((v1.X - v2.X)*(v1.X - v2.X) + (v1.Y - v2.Y)*(v1.Y - v2.Y) + (v1.Z - v2.Z)*(v1.Z - v2.Z));
}

float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));

	return y;
}

Vertex get_unit_vertex(Vertex v)
{
	float sss = Q_rsqrt(v.X*v.X + v.Y*v.Y + v.Z*v.Z);
	return Vertex(v.X*sss, v.Y*sss, v.Z*sss);
}

int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}


void GLCube(Loader& loader, vector<GLuint>& tex)
{

	bool flag = false;
	if (dstIndex.size() != 0) flag = true;
	for (int i = 0; i<loader.model.F.size(); i++)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (loader.model.F[i].mtlIndex >= 0)
		{
			CMaterial mtlTmp = loader.material[loader.model.F[i].mtlIndex];
			GLfloat _glfMatAmbient[] = { mtlTmp.m_ambient.fR,mtlTmp.m_ambient.fG,mtlTmp.m_ambient.fB,mtlTmp.m_ambient.fA };
			GLfloat _glfMatDiffuse[] = { mtlTmp.m_diffuse.fR,mtlTmp.m_diffuse.fG,mtlTmp.m_diffuse.fB,mtlTmp.m_diffuse.fA };
			GLfloat _glfMatSpecular[] = { mtlTmp.m_specular.fR,mtlTmp.m_specular.fG,mtlTmp.m_specular.fB,mtlTmp.m_specular.fA };
			GLfloat _glfMatEmission[] = { mtlTmp.m_emission.fR,mtlTmp.m_emission.fG,mtlTmp.m_emission.fB,mtlTmp.m_emission.fA };
			GLfloat _fShininess = mtlTmp.m_fShiness;
			glMaterialfv(GL_FRONT, GL_AMBIENT, _glfMatAmbient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, _glfMatDiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, _glfMatSpecular);
			glMaterialfv(GL_FRONT, GL_EMISSION, _glfMatEmission);
			glMaterialf(GL_FRONT, GL_SHININESS, _fShininess);
			glBindTexture(GL_TEXTURE_2D, tex[loader.model.F[i].mtlIndex]);
		}
		glBegin(GL_TRIANGLES);	// 绘制三角形
		if (loader.model.VT.size() != 0)glTexCoord2f(loader.model.VT[loader.model.F[i].T[0]].TU, loader.model.VT[loader.model.F[i].T[0]].TV);  //纹理	
		if (loader.model.VN.size() != 0)glNormal3f(loader.model.VN[loader.model.F[i].N[0]].NX, loader.model.VN[loader.model.F[i].N[0]].NY, loader.model.VN[loader.model.F[i].N[0]].NZ);//法向量
		if (!flag) dstIndex.push_back(Index(loader.model.F[i].V[0], loader.model.F[i].V[1], loader.model.F[i].V[2]));
		glVertex3f(loader.model.V[loader.model.F[i].V[0]].X / YU, loader.model.V[loader.model.F[i].V[0]].Y / YU, loader.model.V[loader.model.F[i].V[0]].Z / YU);		// 上顶点

		if (loader.model.VT.size() != 0)glTexCoord2f(loader.model.VT[loader.model.F[i].T[1]].TU, loader.model.VT[loader.model.F[i].T[1]].TV);  //纹理
		if (loader.model.VN.size() != 0)glNormal3f(loader.model.VN[loader.model.F[i].N[1]].NX, loader.model.VN[loader.model.F[i].N[1]].NY, loader.model.VN[loader.model.F[i].N[1]].NZ);//法向量
		glVertex3f(loader.model.V[loader.model.F[i].V[1]].X / YU, loader.model.V[loader.model.F[i].V[1]].Y / YU, loader.model.V[loader.model.F[i].V[1]].Z / YU);		// 左下

		if (loader.model.VT.size() != 0)glTexCoord2f(loader.model.VT[loader.model.F[i].T[2]].TU, loader.model.VT[loader.model.F[i].T[2]].TV);  //纹理
		if (loader.model.VN.size() != 0)glNormal3f(loader.model.VN[loader.model.F[i].N[2]].NX, loader.model.VN[loader.model.F[i].N[2]].NY, loader.model.VN[loader.model.F[i].N[2]].NZ);//法向量
		glVertex3f(loader.model.V[loader.model.F[i].V[2]].X / YU, loader.model.V[loader.model.F[i].V[2]].Y / YU, loader.model.V[loader.model.F[i].V[2]].Z / YU);		// 右下
		glEnd();	
		glPopAttrib();
		glPopMatrix();// 三角形绘制结束	
	}
}

float normRe(Vertex v)
{
	return Q_rsqrt(v.X*v.X + v.Y*v.Y + v.Z*v.Z);
}

Vertex getMiddle(Vertex v1, Vertex v2)
{
	float normRe1 = normRe(v1);
	float normRe2 = normRe(v2);
	Vertex tmp(v1.X*normRe1 + v2.X*normRe2, v1.Y*normRe1 + v2.Y*normRe2, v1.Z*normRe1 + v2.Z*normRe2);
	float con = normRe(tmp)*radius;
	return Vertex(con*tmp.X, con*tmp.Y, con*tmp.Z);
}

void balance()
{
	int srcSize = loader.model.F.size();
	int dstSize = loader2.model.F.size();
	if (srcSize > dstSize)
	{
		for (int i = dstSize; i < srcSize; i++)
		{
			Face f = loader2.model.F[0];
			f.V[0] = 0;
			f.V[1] = 0;
			f.V[2] = 0;
			loader2.model.F.push_back(f);
		}
	}
	else if (srcSize < dstSize)
	{
		for (int i = srcSize; i < dstSize; i++)
		{
			Face f = loader.model.F[0];
			f.V[0] = 0;
			f.V[1] = 0;
			f.V[2] = 0;
			loader.model.F.push_back(f);
		}
	}
}

struct cmp
{
	bool operator()(TriangleEdge* t1, TriangleEdge* t2) { return t1->distance > t2->distance; }
};

bool compare(TriangleEdge* t1, TriangleEdge* t2) 
{ return t1->distance < t2->distance; }


void mapping()
{
	/*
	balance();
	int srcSize = loader.model.F.size();
	int dstSize = loader2.model.F.size();
	unordered_set<int>srcVertex;
	for (int i = 0; i < srcSize; i++) srcVertex.insert(i);
	unordered_set<int>dstVertex;
	for (int i = 0; i < dstSize; i++) dstVertex.insert(i);
	mappin.clear();
	priority_queue<TriangleEdge*,vector<TriangleEdge*>, cmp>order;
	while (mappin.size() < srcSize)
	{
		for (unordered_set<int>::iterator iter1=srcVertex.begin();iter1!=srcVertex.end();iter1++)
		{
			for (unordered_set<int>::iterator iter2 = dstVertex.begin(); iter2 != dstVertex.end(); iter2++)
			{
				order.push(bi_loader.getFaceDistance(*iter1, *iter2));
			}
		}
		while (!order.empty())
		{
			TriangleEdge* triEdge = order.top();
			order.pop();
			if (srcVertex.find(triEdge->srcIndex) == srcVertex.end()
				|| dstVertex.find(triEdge->dstIndex) == dstVertex.end()) continue;
			srcVertex.erase(triEdge->srcIndex);
			dstVertex.erase(triEdge->dstIndex);
			mappin.push_back(triEdge);
		}
	}
	*/
	balance();
	int srcSize = loader.model.F.size();
	int dstSize = loader2.model.F.size();
	unordered_set<int>srcVertex;
	for (int i = 0; i < srcSize; i++) srcVertex.insert(i);
	unordered_set<int>dstVertex;
	for (int i = 0; i < dstSize; i++) dstVertex.insert(i);
	mappin.clear();
	long maxSize = srcSize*dstSize;
	TriangleEdge** order = new TriangleEdge* [maxSize];
	for (unordered_set<int>::iterator iter2 = dstVertex.begin(); iter2 != dstVertex.end(); iter2++)
	{
		bi_loader.enter_pos(*iter2, curr_pos,shoot_dir);
	}
	for (int i = 0; i < 6; i++)
	{
		shoot_dir[i] = get_unit_vertex(shoot_dir[i]);
	}
	while (mappin.size() < srcSize)
	{
		for (unordered_set<int>::iterator iter1 = srcVertex.begin(); iter1 != srcVertex.end(); iter1++)
		{
			for (unordered_set<int>::iterator iter2 = dstVertex.begin(); iter2 != dstVertex.end(); iter2++)
			{
				order[*iter1*srcSize + *iter2] = bi_loader.getFaceDistance(*iter1, *iter2);
			}
		}
		sort(order, order + maxSize, compare);
		int index = 0;
		while (index<maxSize)
		{
			TriangleEdge* triEdge = order[index];
			index++;
			if (srcVertex.find(triEdge->srcIndex) == srcVertex.end()
				|| dstVertex.find(triEdge->dstIndex) == dstVertex.end()) continue;
			srcVertex.erase(triEdge->srcIndex);
			dstVertex.erase(triEdge->dstIndex);
			mappin.push_back(triEdge);
		}
	}
	delete[] order;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	Vertex centerV((loader.model.center.X + loader2.model.center.X) / 2,
		(loader.model.center.Y + loader2.model.center.Y) / 2,
		(loader.model.center.Z + loader2.model.center.Z) / 2);
	gluLookAt(centerV.X +NEWR*sin(du)*sin(dv), centerV.Y + NEWR*cos(du), centerV.Z+ NEWR*sin(du)*cos(dv), centerV.X, centerV.Y, centerV.Z, 0, sin(du) > 0 ? 1 : -1 , 0); //从视点看远点,y轴方向(0,1,0)是上方向  
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLCube(loader,tex);
	glPopMatrix();
	glPopAttrib();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLCube(loader2,tex2);
	glPopMatrix();
	glPopAttrib();
	//GLfloat glfLight[] = { centerV.X,centerV.Y,  centerV.Z, 0.0f };
	//glLightfv(GL_LIGHT0, GL_POSITION, glfLight);

	if (finishedMorph)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		Draw(curr_pos);
		glPopMatrix();
		glPopAttrib();
	}
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	if (morphStart)
	{
		finishedMorph = false;
		generate_particle_pos(curr_pos);
		vector<vector<int> >cIndex = { { 0,1,2 },{ 1,2,0 },{ 2,0,1 } };
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glLoadIdentity();
		for (int i = 0; i < mappin.size(); i++)
		{
			Vertex v1, v2, v3;
			TriangleEdge* triEdge = mappin[i];
			vector<Vertex> src = { loader.model.V[loader.model.F[triEdge->srcIndex].V[cIndex[triEdge->srcVertexOrder][0]]],
				loader.model.V[loader.model.F[triEdge->srcIndex].V[cIndex[triEdge->srcVertexOrder][1]]],
				loader.model.V[loader.model.F[triEdge->srcIndex].V[cIndex[triEdge->srcVertexOrder][2]]] };
			vector<Vertex> dst = { loader2.model.V[loader2.model.F[triEdge->dstIndex].V[cIndex[triEdge->dstVertexOrder][0]]],
				loader2.model.V[loader2.model.F[triEdge->dstIndex].V[cIndex[triEdge->dstVertexOrder][1]]],
				loader2.model.V[loader2.model.F[triEdge->dstIndex].V[cIndex[triEdge->dstVertexOrder][2]]] };
			double x = currStep*1.0 / moveStep;
			double alpha = 1-x*x*(3 - 2 * x);
			v1 = Vertex(src[0].X*alpha + dst[0].X*(1 - alpha),
				src[0].Y*alpha + dst[0].Y*(1 - alpha),
				src[0].Z*alpha + dst[0].Z*(1 - alpha));
			v2 = Vertex(src[1].X*alpha + dst[1].X*(1 - alpha),
				src[1].Y*alpha + dst[1].Y*(1 - alpha),
				src[1].Z*alpha + dst[1].Z*(1 - alpha));
			v3 = Vertex(src[2].X*alpha + dst[2].X*(1 - alpha),
				src[2].Y*alpha + dst[2].Y*(1 - alpha),
				src[2].Z*alpha + dst[2].Z*(1 - alpha));
			/*if (i == 0)
			{
				cout << alpha << "\n";
				cout << "v1: " << v1.X << " " << v1.Y << " " << v1.Z << "\n";
				cout << "v2: " << v2.X << " " << v2.Y << " " << v2.Z << "\n";
				cout << "v3: " << v3.X << " " << v3.Y << " " << v3.Z << "\n";
			}*/
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_TRIANGLES);
				glVertex3f(v1.X, v1.Y, v1.Z);
				glVertex3f(v2.X, v2.Y, v2.Z);
				glVertex3f(v3.X, v3.Y, v3.Z);
			glEnd();
		}
		currStep++;
		if (currStep == moveStep)
		{
			morphStart = false;
			finishedMorph = true;
			currStep = 0;
		}
	}
	glPopMatrix();
	glPopAttrib();
	// Position and rotate the camera
	glFlush();
	glutSwapBuffers();
}
void Mouse(int button, int state, int x, int y) //处理鼠标点击  
{
	if (button == GLUT_RIGHT_BUTTON)
	{
		
			interm = src;
		morphStart = true;
	}
    if (state == GLUT_DOWN) //第一次鼠标按下时,记录鼠标在窗口中的初始坐标  
		oldmx = x, oldmy = y;

}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q')
	{
		viewConst += 1;
	}
	else if (key == 'w')
	{
		viewConst -= 1;
	}
}

void onMouseMove(int x, int y) //处理鼠标拖动  
{
	//printf("%d\n",du);  
	dv -= (x - oldmx) / NEWR/5; 
	du -= (y - oldmy) / NEWR/5; 
	oldmx = x, oldmy = y; //把此时的鼠标坐标作为旧值，为下一次计算增量做准备  
}
void init()
{
	//string fileHead = "006_00_0/";
	string fileHead = "";
	string fileHead2 = "";
	string filePath;
	string filePath2;
	cout << "请输入第一个obj模型的相对路径: \n";
	cin >> filePath;
	cout << "请输入第二个obj模型的相对路径: \n";
	cin >> filePath2;
	int i = -1;
	for (i = filePath.size() - 1; i >= 0; i--)
	{
		if (filePath[i] == '/') break;
	}
	if (i != -1)
	{
		fileHead = filePath.substr(0, i + 1);
	}
	loader.load(filePath, 0, 0, -80);
	for (i = filePath2.size() - 1; i >= 0; i--)
	{
		if (filePath2[i] == '/') break;
	}
	if (i != -1)
	{
		fileHead2 = filePath2.substr(0, i + 1);
	}
	loader2.load(filePath2);
	int texNum = loader.material.size();
	for (int i = 0; i < texNum; i++)
	{
		tex.push_back(loader.loadTexture((fileHead + loader.material[i].m_szTextureName).c_str()));
	}
	int texNum2 = loader2.material.size();
	for (int i = 0; i < texNum2; i++)
	{
		tex2.push_back(loader2.loadTexture((fileHead2 + loader2.material[i].m_szTextureName).c_str()));
	}

	glClearColor(1.000f, 1.000f, 1.000f, 1.0f); //Background color
	// Activate lighting and a light source
	//用于启用各种功能。具体功能由参数决定。与glDisable相对应。glDisable用以关闭各项功能。
    glEnable(GL_LIGHT0);//启用0号灯到7号灯(光源)　　光源要求由函数glLight函数来完成
	glEnable(GL_LIGHTING);//启用灯源
	glEnable(GL_DEPTH_TEST);//启用深度测试。　　根据坐标的远近自动隐藏被遮住的图形（材料）
	glEnable(GL_TEXTURE_2D);   // 启用二维纹理
	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS); //所作深度测试的类型  
	glShadeModel(GL_SMOOTH); 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glClearColor(0.0f, 0.0f, 0.0f,0.0f);
							   // Define material parameters
	//static GLfloat glfMatAmbient[] = { 0.000f, 0.450f, 1.000f, 1.0f };
	static GLfloat glfMatAmbient[] = { 0.20f, 0.20f, 0.200f, 1.0f };
	static GLfloat glfMatDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static GLfloat glfMatSpecular[] = { 1.000f, 1.000f, 1.000f, 1.0f };
	static GLfloat glfMatEmission[] = { 0.000f, 0.0000f, 0.000f, 1.0f };
	static GLfloat fShininess = 128.000f;

	glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	// Set material parameters
	//指定用于光照计算的当前材质属性。参数face的取值可以是GL_FRONT、GL_BACK或GL_FRONT_AND_BACK，指出材质属性将应用于物体的哪面。
	//void glMaterial{if}(GLenum face, GLenum pname, TYPE param);
	glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, fShininess);


	bi_loader = Bi_loader(&loader, &loader2);
	mapping();
	init_particle(loader2.model.center,shoot_dir/*getMiddlePoint(loader.model.center,loader2.model.center)*/);
}
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, (float)w / h, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("OpenGL");
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);  //设置不断调用显示函数  
	glutMouseFunc(Mouse);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(onMouseMove);
	glutMainLoop();
	return 0;
}


