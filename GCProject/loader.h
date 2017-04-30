#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include <GL/glut.h>  
using namespace std;

struct Vertex {
	double X;
	double Y;
	double Z;
	Vertex();
	Vertex(double x, double y, double z);
};
struct Texture {
	double TU;
	double TV;
};
struct NormalV {
	double NX;
	double NY;
	double NZ;
};
struct Face {
	int V[3];
	int T[3];
	int N[3];
	int mtlIndex;
};

struct Index {
	int v1;
	int v2;
	int v3;
	Index();
	Index(int x, int y, int z);
};

struct TriangleEdge
{
	int srcIndex;
	int dstIndex;
	int srcVertexOrder;
	int dstVertexOrder;
	double distance;
};

struct StepData
{
	Vertex v1;
	Vertex v2;
	Vertex v3;
	StepData();
	StepData(Vertex x, Vertex y, Vertex z);
};
class Model
{
public:
	vector<Vertex> V;//V：代表顶点。格式为V X Y Z，V后面的X Y Z表示三个顶点坐标。浮点型
	vector<Texture>  VT;//表示纹理坐标。格式为VT TU TV。浮点型
	vector<NormalV> VN;//VN：法向量。每个三角形的三个顶点都要指定一个法向量。格式为VN NX NY NZ。浮点型
	vector<Face> F;//F：面。面后面跟着的整型值分别是属于这个面的顶点、纹理坐标、法向量的索引。
				   //面的格式为：f Vertex1/Texture1/Normal1 Vertex2/Texture2/Normal2 Vertex3/Texture3/Normal3
	Vertex center;
	Vertex max;
	Vertex min;
};

struct SRgb
{
	float fR;
	float fG;
	float fB;
	float fA;
};

class CMaterial
{
	friend class Model;
public:
	CMaterial()
	{
		m_emission.fR = m_emission.fG = m_emission.fB = 0.0;
		m_fTrans = 1.0;
	}

	SRgb m_ambient;//材质的环境颜色
	SRgb m_diffuse;//
	SRgb m_specular;
	SRgb m_emission;
	float m_fShiness;//镜面指数
	float m_fTrans;//透明度
	string m_szName;
	string m_szTextureName;
};

typedef struct
{
	GLubyte *imageData;//图像数据  
	GLuint bpp;//像素深度  
	GLuint width;//图像宽度  
	GLuint height;//图像高度  
	GLuint texID;//对应的纹理ID  
}TextureImage;

class Loader
{
public:
	Loader();
	virtual ~Loader();
	void load(const string& filePath, int dx = 0, int dy = 0, int dz = 0);
	static GLuint loadTexture(const char* filename);
	Model model;
	vector<CMaterial> material;
};

class Bi_loader
{
public:
	Bi_loader();
	Bi_loader(Loader* l1, Loader* l2);
	TriangleEdge* getFaceDistance(const int& srcIndex, const int& dstIndex);
	void enter_pos(const int& dstIndex, vector<Vertex>&curr_pos, vector<Vertex>&shoot_dir);
	double calculateDistanceSqr(const Vertex& v1, const Vertex& v2);
private:
	Loader* loader1;
	Loader* loader2;
	double getInserCos(Vertex v1, Vertex v2);
	double getInserCoss(Vertex v1, Vertex v2);
	//double Q_rsqrt(double number);
};