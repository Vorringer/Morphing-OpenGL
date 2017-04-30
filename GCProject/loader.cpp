#include "stdafx.h"
#include "loader.h"
#include"lodepng.h"
#include <assert.h>

using namespace std;
#define BMP_Header_Length 54

vector <Vertex> par_pos =
{ Vertex(1,0,0),Vertex(-1,0,0) ,Vertex(0,0,-1) ,Vertex(0,0,1) ,Vertex(0,1,0) ,Vertex(0,-1,0) };

vector<double>intersect = { -1,-1,-1,-1,-1,-1 };

Index::Index() {};

Index::Index(int x, int y, int z) :v1(x), v2(y), v3(z) {};

Vertex::Vertex() {};

Vertex::Vertex(double x, double y, double z) :X(x), Y(y), Z(z) {};

StepData::StepData() {};

StepData::StepData(Vertex x, Vertex y, Vertex z) :v1(x), v2(y), v3(z) {};

Loader::Loader() {};

Loader::~Loader() {};

Bi_loader::Bi_loader() {};
Bi_loader::Bi_loader(Loader* l1, Loader* l2) : loader1(l1), loader2(l2) {};

double Bi_loader::calculateDistanceSqr(const Vertex& v1, const Vertex& v2)
{
	return (v1.X - v2.X)*(v1.X - v2.X) + (v1.Y - v2.Y)*(v1.Y - v2.Y) + (v1.Z - v2.Z)*(v1.Z - v2.Z);
}

float Q_rrsqrt(float x)
{
		float xhalf = 0.5f*x;
		int i = *(int*)&x;
		i = 0x5f3759df - (i >> 1);
		x = *(float*)&i;
		x = x*(1.5f - xhalf*x*x);
		return x;
}
double Q_rssqrt(double number)
{
	long i;
	double x2, y;
	const double threehalfs = 1.5;

	x2 = number * 0.5;
	y = number;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(double *)&i;
	y = y * (threehalfs - (x2 * y * y));

	return y;
}

double Bi_loader::getInserCos(Vertex v1, Vertex v2)
{
	return (v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z) *1.0* (Q_rrsqrt((v1.X*v1.X + v1.Y*v1.Y + v1.Z*v1.Z)*(v2.X*v2.X + v2.Y*v2.Y + v2.Z*v2.Z)));
}
double Bi_loader::getInserCoss(Vertex v1, Vertex v2)
{
	return (v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z) *1.0* (Q_rssqrt((v1.X*v1.X + v1.Y*v1.Y + v1.Z*v1.Z)*(v2.X*v2.X + v2.Y*v2.Y + v2.Z*v2.Z)));
}


void Bi_loader::enter_pos(const int& dstIndex,vector<Vertex>&curr_pos, vector<Vertex>&shoot_dir)
{
	Face f2 = loader2->model.F[dstIndex];
	Vertex v1 = loader2->model.V[f2.V[0]];
	Vertex v2 = loader2->model.V[f2.V[1]];
	Vertex v3 = loader2->model.V[f2.V[2]];
	Vertex mid2 = Vertex((v1.X + v2.X + v3.X) / 3.0, (v1.Y + v2.Y + v3.Y) / 3.0, (v1.Z + v2.Z + v3.Z) / 3.0);
	v2 = Vertex(mid2.X - loader2->model.center.X, mid2.Y - loader2->model.center.Y, mid2.Z - loader2->model.center.Z);
	for (int i = 0; i < 6; i++)
	{
		double curr = getInserCos(v2, par_pos[i]);
		//assert(ori < 1 && curr < 1);
		if (curr > intersect[i])
		{
			intersect[i] = curr;
			curr_pos[i] = mid2;
			shoot_dir[i] = v2;
		}
	}
}
TriangleEdge* Bi_loader::getFaceDistance(const int& srcIndex, const int& dstIndex)
{
	Face f1 = loader1->model.F[srcIndex];
	Face f2 = loader2->model.F[dstIndex];
	vector<vector<int> >cIndex = { {0,1,2},{1,2,0},{2,0,1} };
	double minDistance = INT_MAX;
	int srcVertexOrder = 0;
	int dstVertexOrder = 0;
	
	/*for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			double currDistance = 0.0;
			Vertex v1 = loader1->model.V[f1.V[cIndex[i][0]]];
			Vertex v2= loader2->model.V[f2.V[cIndex[j][0]]];
			currDistance+= calculateDistanceSqr(v1, v2);
			v1 = loader1->model.V[f1.V[cIndex[i][1]]];
			v2 = loader2->model.V[f2.V[cIndex[j][1]]];
			currDistance += calculateDistanceSqr(v1, v2);
			v1 = loader1->model.V[f1.V[cIndex[i][2]]];
			v2 = loader2->model.V[f2.V[cIndex[j][2]]];
			currDistance += calculateDistanceSqr(v1, v2);
			if (currDistance < minDistance)
			{
				minDistance = currDistance;
				srcVertexOrder = i;
				dstVertexOrder = j;
			}
		}
	}*/
	

	Vertex v1 = loader1->model.V[f1.V[0]];
	Vertex v2 = loader1->model.V[f1.V[1]];
	Vertex v3 = loader1->model.V[f1.V[2]];
	Vertex mid1 = Vertex((v1.X + v2.X + v3.X) / 3.0, (v1.Y + v2.Y + v3.Y) / 3.0, (v1.Z + v2.Z + v3.Z) / 3.0);
	v1 = loader2->model.V[f2.V[0]];
	v2 = loader2->model.V[f2.V[1]];
	v3 = loader2->model.V[f2.V[2]];
	Vertex mid2 = Vertex((v1.X + v2.X + v3.X) / 3.0, (v1.Y + v2.Y + v3.Y) / 3.0, (v1.Z + v2.Z + v3.Z) / 3.0);
	v1 = Vertex(mid1.X - loader1->model.center.X, mid1.Y - loader1->model.center.Y, mid1.Z - loader1->model.center.Z);
	v2 = Vertex(mid2.X - loader2->model.center.X, mid2.Y - loader2->model.center.Y, mid2.Z - loader2->model.center.Z);
	TriangleEdge* triEdge = new TriangleEdge();
	triEdge->distance = getInserCoss(v1, v2);
	triEdge->srcIndex = srcIndex;
	triEdge->dstIndex = dstIndex;
	triEdge->srcVertexOrder = srcVertexOrder;
	triEdge->dstVertexOrder = dstVertexOrder;
	return triEdge;
}

void Loader::load(const string& filePath, int dx, int dy, int dz)
{
	string s;
	Face *f;
	Vertex *v;
	NormalV *vn;
	Texture *vt;
	float xsum = 0.0;
	float ysum = 0.0;
	float zsum = 0.0;
	int currMtl = -1;
	int i = -1;
	for (i = filePath.size() - 1; i >= 0; i--)
	{
		if (filePath[i] == '.') break;
	}
	string mtlPath;
	if (i != -1)
		mtlPath = filePath.substr(0, i) + ".mtl";
	else
		mtlPath = filePath + ".mtl";
	ifstream iff(mtlPath);
	CMaterial* mtlTmp = new CMaterial();
	SRgb* srgbTmp;
	while (getline(iff, s))
	{
		if (s[0] == 'n')
		{
			istringstream in(s);
			string head;
			string currMtlStr;
			in >> head >> currMtlStr;
			mtlTmp->m_szName = currMtlStr;
		}
		else if (s[0] == 'K')
		{
			istringstream in(s);
			string head;
			srgbTmp = new SRgb();
			in >> head >> srgbTmp->fR >> srgbTmp->fG >> srgbTmp->fB;
			srgbTmp->fA = 1;
			if (s[1] == 'd')
			{
				mtlTmp->m_diffuse = *srgbTmp;
			}
			else if (s[1] == 'a')
			{
				mtlTmp->m_ambient = *srgbTmp;
			}
			else if (s[1] == 's')
			{
				mtlTmp->m_specular = *srgbTmp;
			}
			else if (s[1] == 'e')
			{
				mtlTmp->m_emission = *srgbTmp;
			}
		}
		else if (s[0] == 'N')
		{
			istringstream in(s);
			string head;
			float shine;
			in >> head >> shine;
			mtlTmp->m_fShiness = shine;
		}
		else if (s[0] == 'd')
		{
			istringstream in(s);
			string head;
			float tran;
			in >> head >> tran;
			mtlTmp->m_fTrans = tran;
		}
		else if (s[0] == 'm')
		{
			istringstream in(s);
			string head;
			string texName;
			in >> head >> texName;
			mtlTmp->m_szTextureName = texName;
			material.push_back(*mtlTmp);
			mtlTmp = new CMaterial();
		}
	}

	ifstream ifs(filePath);
	float maxX = -1, maxY = -1, maxZ = -1;
	float minX = INT_MAX, minY = INT_MAX, minZ = INT_MAX;
	while (getline(ifs, s))
	{
		if (s.length()<2)continue;
		if (s[0] == 'u')
		{
			istringstream in(s);
			string head;
			string currMtlStr;
			in >> head >> currMtlStr;
			for (int i = 0; i < material.size(); i++)
			{
				if (material[i].m_szName == currMtlStr)
				{
					currMtl = i;
				}
			}
		}
		else if (s[0] == 'v') {
			if (s[1] == 't') {
				istringstream in(s);
				vt = new Texture();
				string head;
				in >> head >> vt->TU >> vt->TV;
				model.VT.push_back(*vt);
			}
			else if (s[1] == 'n') {
				istringstream in(s);
				vn = new NormalV();
				string head;
				in >> head >> vn->NX >> vn->NY >> vn->NZ;
				model.VN.push_back(*vn);
			}
			else {
				istringstream in(s);
				v = new Vertex();
				string head;
				in >> head >> v->X >> v->Y >> v->Z;
				v->X += dx;
				v->Y += dy;
				v->Z += dz;
				if (v->X > maxX) maxX = v->X;
				if (v->Y > maxY) maxY = v->Y;
				if (v->Z > maxZ) maxZ = v->Z;
				if (v->X < minX) minX = v->X;
				if (v->Y < minY) minY = v->Y;
				if (v->Z < minZ) minZ = v->Z;
				xsum += v->X;
				ysum += v->Y;
				zsum += v->Z;
				model.V.push_back(*v);
			}
		}
		else if (s[0] == 'f')
		{
			for (int k = s.size() - 1; k >= 0; k--)
			{
				if (s[k] == '/')s[k] = ' ';
			}
			istringstream in(s);
			f = new Face();
			string head;
			in >> head;
			int i = 0;
			while (i<3)
			{
				if (model.V.size() != 0)
				{
					in >> f->V[i];
					f->V[i] -= 1;
				}
				if (model.VT.size() != 0)
				{
					in >> f->T[i];
					f->T[i] -= 1;
				}
				if (model.VN.size() != 0)
				{
					in >> f->N[i];
					f->N[i] -= 1;
				}
				i++;
			}
			f->mtlIndex = currMtl;
			model.F.push_back(*f);
		}
	}
	int ssize = model.V.size();
	model.max = Vertex(maxX, maxY, maxZ);
	model.min = Vertex(minX, minY, minZ);
	model.center = Vertex(xsum*1.0 / ssize, ysum*1.0 / ssize, zsum*1.0 / ssize);
	ifs.close();
	

}

GLuint Loader::loadTexture(const char* filename)
{
	GLuint texIndex;
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 1;
	}
	glGenTextures(1, &texIndex);                    // Generate OpenGL texture IDs  

	glBindTexture(GL_TEXTURE_2D, texIndex);         // Bind Our Texture  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Linear Filtered  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Linear Filtered  

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	return texIndex;
}

