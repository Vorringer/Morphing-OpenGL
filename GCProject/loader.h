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
	vector<Vertex> V;//V�������㡣��ʽΪV X Y Z��V�����X Y Z��ʾ�����������ꡣ������
	vector<Texture>  VT;//��ʾ�������ꡣ��ʽΪVT TU TV��������
	vector<NormalV> VN;//VN����������ÿ�������ε��������㶼Ҫָ��һ������������ʽΪVN NX NY NZ��������
	vector<Face> F;//F���档�������ŵ�����ֵ�ֱ������������Ķ��㡢�������ꡢ��������������
				   //��ĸ�ʽΪ��f Vertex1/Texture1/Normal1 Vertex2/Texture2/Normal2 Vertex3/Texture3/Normal3
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

	SRgb m_ambient;//���ʵĻ�����ɫ
	SRgb m_diffuse;//
	SRgb m_specular;
	SRgb m_emission;
	float m_fShiness;//����ָ��
	float m_fTrans;//͸����
	string m_szName;
	string m_szTextureName;
};

typedef struct
{
	GLubyte *imageData;//ͼ������  
	GLuint bpp;//�������  
	GLuint width;//ͼ����  
	GLuint height;//ͼ��߶�  
	GLuint texID;//��Ӧ������ID  
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