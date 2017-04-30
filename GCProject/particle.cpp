#include "stdafx.h"
#pragma comment(lib,"GLAUX.LIB")
#include <iostream>
#include <time.h>
#include <GL/glut.h> 
#include <GL/glaux.h>
using namespace std;
#include "particle.h"

#define MAX_SURROUNDING 6
#define MAX_PARTICLES 1000
#define PI 3.14159

//winter is coming
bool rainbow = true;
bool sp, rp;//space button and return button pressed or not?
float slowdown = 2.0f;
float xspeed, yspeed;
float zoom = 0.0f;
bool gkeys[256];
float dx[MAX_SURROUNDING];
float dy[MAX_SURROUNDING];
float dz[MAX_SURROUNDING];
int spillNum = 12;

GLuint loop, col, delay, texture[1];

particles paticle[MAX_PARTICLES];
Vertex initV;
vector<Vertex>shootDir;

vector <Vertex> particle_pos =
{ Vertex(20,0,0),Vertex(-20,0,0) ,Vertex(0,0,-20) ,Vertex(0,0,20) ,Vertex(0,35,0) ,Vertex(0,-35,0) };

static GLfloat colors[12][3] =     // Rainbow Of Colors 
{
	{ 1.0f,0.5f,0.5f },{ 1.0f,0.75f,0.5f },{ 1.0f,1.0f,0.5f },{ 0.75f,1.0f,0.5f },
	{ 0.5f,1.0f,0.5f },{ 0.5f,1.0f,0.75f },{ 0.5f,1.0f,1.0f },{ 0.5f,0.75f,1.0f },
	{ 0.5f,0.5f,1.0f },{ 0.75f,0.5f,1.0f },{ 1.0f,0.5f,1.0f },{ 1.0f,0.5f,0.75f }
};

AUX_RGBImageRec *LoadBMP(char *Filename) // Loads A Bitmap Image 
{
	FILE *File = NULL;        // File Handle 
	if (!Filename)        // Make Sure A Filename Was Given 
	{
		return NULL;        // If Not Return NULL 
	}
	fopen_s(&File, Filename, "r");      // Check To See If The File Exists 
	if (File)           // Does The File Exist? 
	{
		fclose(File);         // Close The Handle 
		return auxDIBImageLoad(TEXT("particle.bmp"));   // Load The Bitmap And Return A Pointer 
	}
	return NULL;          // If Load Failed Return NULL 
}

int LoadGLTextures()        // Load Bitmaps And Convert To Textures 
{
	int Status = FALSE;        // Status Indicator 
	AUX_RGBImageRec *TextureImage[1];    // Create Storage Space For The Texture 

	memset(TextureImage, 0, sizeof(void *) * 1); // Set The Pointer To NULL 
	if (TextureImage[0] = LoadBMP("particle.bmp"))  // Load Particle Texture 
	{
		Status = TRUE;          // Set The Status To TRUE 
		glGenTextures(1, &texture[0]);    // Create One Textures 

		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3,
			TextureImage[0]->sizeX, TextureImage[0]->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0])        // If Texture Exists 
	{
		if (TextureImage[0]->data)      // If Texture Image Exists 
		{
			free(TextureImage[0]->data);    // Free The Texture Image Memory 
		}
		free(TextureImage[0]);       // Free The Image Structure 
	}
	return Status;          // Return The Status 
}

//XD
void init_particle(Vertex initPos, vector<Vertex> shoot_dir)
{
	texture[0] = -1;

	shootDir = shoot_dir;
	for (int i = 0; i < 6; i++)
	{
		cout << shootDir[i].X << " " << shootDir[i].Y << " " << shootDir[i].Z << "\n";
	}
	for (loop = 0; loop < MAX_PARTICLES; ++loop)
	{
		paticle[loop].active = true;
		paticle[loop].life = 1.0f;//full life is 1.0f
								  //life decrease rate(a random value add 0.003f : never equals 0)

		paticle[loop].x = initV.X = initPos.X;
		paticle[loop].y = initV.Y = initPos.Y;
		paticle[loop].z = initV.Z = initPos.Z;

		paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;
		paticle[loop].r = colors[int(12 * (loop*1.0 / MAX_PARTICLES))][0];
		paticle[loop].g = colors[int(12 * (loop*1.0 / MAX_PARTICLES))][1];
		paticle[loop].b = colors[int(12 * (loop*1.0 / MAX_PARTICLES))][2];

		paticle[loop].xi = float((rand() % 50) - 26.0f) * 8.0f;
		paticle[loop].yi = float((rand() % 50) - 25.0f) * 8.0f;
		paticle[loop].zi = float((rand() % 50) - 25.0f) * 8.0f;

		paticle[loop].xg = 0.0f;
		paticle[loop].yg = -0.8f;
		paticle[loop].zg = 0.0f;
	}
}

void init_parti(int i)
{
	for (loop = 0; loop < MAX_PARTICLES; ++loop)
	{
		paticle[loop].active = true;
		paticle[loop].life = 1.0f;//full life is 1.0f
								  //life decrease rate(a random value add 0.003f : never equals 0)

		paticle[loop].x = initV.X;
		paticle[loop].y = initV.Y;
		paticle[loop].z = initV.Z;

		paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;
		paticle[loop].r = colors[int(12 * (loop*1.0 / MAX_PARTICLES))][0];
		paticle[loop].g = colors[int(12 * (loop*1.0 / MAX_PARTICLES))][1];
		paticle[loop].b = colors[int(12 * (loop*1.0 / MAX_PARTICLES))][2];

		paticle[loop].xi = float((rand() % 50) - 26.0f) * 80.0f*shootDir[i].X;
		paticle[loop].yi = float((rand() % 50) - 25.0f) * 80.0f*shootDir[i].Y;
		paticle[loop].zi = float((rand() % 50) - 25.0f) * 80.0f*shootDir[i].Z;

		paticle[loop].xg = 0.0f;
		paticle[loop].yg = -0.8f;
		paticle[loop].zg = 0.0f;
	}
}

void generate_particle_pos(vector<Vertex>& curr_pos)
{
	for (int i = 0; i < MAX_SURROUNDING; i++)
	{
		dx[i] = curr_pos[i].X - initV.X; //particle_pos[i].X;//curr_pos[i].X - initV.X;
		dy[i] = curr_pos[i].Y - initV.Y; //particle_pos[i].Y;//curr_pos[i].Y - initV.Y;
		dz[i] = curr_pos[i].Z - initV.Z; //particle_pos[i].Z;//curr_pos[i].Z - initV.Z;
	}
}

bool Draw(const vector<Vertex>& curr_pos) {
	LoadGLTextures();
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//really nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	for (int surr = 0; surr < MAX_SURROUNDING; surr++)
	{
		//if (surr==0)init_parti(surr);
		for (loop = 0; loop < MAX_PARTICLES; ++loop)
		{
			if (paticle[loop].active)//the particle is alive
			{
				float x = paticle[loop].x + dx[surr];
				float y = paticle[loop].y + dy[surr];
				//our scene is moved into the screen
				float z = paticle[loop].z + dz[surr];

				glColor4f(paticle[loop].r,
					paticle[loop].g,
					paticle[loop].r,
					//use life as alpha value:
					//as particle dies,it becomes more and more transparent
					paticle[loop].life);

				float dd = 0.5f;
				//draw particles : use triangle strip instead of quad to speed
				glBegin(GL_TRIANGLE_STRIP);
				//top right
				glTexCoord2d(1, 1);
				glVertex3f(x + dd, y + dd, z);
				//top left
				glTexCoord2d(0, 1);
				glVertex3f(x - dd, y + dd, z);
				//bottom right
				glTexCoord2d(1, 0);
				glVertex3f(x + dd, y - dd, z);
				//bottom left
				glTexCoord2d(0, 0);
				glVertex3f(x - dd, y - dd, z);
				glEnd();

				//Move On The X Axis By X Speed 
				paticle[loop].x += paticle[loop].xi / (slowdown * 1000);
				//Move On The Y Axis By Y Speed 
				paticle[loop].y += paticle[loop].yi / (slowdown * 1000);
				//Move On The Z Axis By Z Speed 
				paticle[loop].z += paticle[loop].zi / (slowdown * 1000);

				//add gravity or resistance : acceleration
				paticle[loop].xi += paticle[loop].xg;
				paticle[loop].yi += paticle[loop].yg;
				paticle[loop].zi += paticle[loop].zg;

				//decrease particles' life
				paticle[loop].life -= paticle[loop].fade;

				//if particle is dead,rejuvenate it
				if (paticle[loop].life < 0.0f)
				{
					paticle[loop].life = 1.0f;//alive again
					paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;

					//reset its position
					paticle[loop].x = initV.X;
					paticle[loop].y = initV.Y;
					paticle[loop].z = initV.Z;

					//X Axis Speed And Direction 
					paticle[loop].xi = (xspeed + float((rand() % 60) - 32.0f))*10.0;
					// Y Axis Speed And Direction 
					paticle[loop].yi = yspeed + float((rand() % 60) - 30.0f)*5.0;
					// Z Axis Speed And Direction 
					paticle[loop].zi = float((rand() % 60) - 30.0f)*15.0;

					int ccol = rand() % 12;
					paticle[loop].r = colors[ccol][0];		// Select Red From Color Table
					paticle[loop].g = colors[ccol][1];			// Select Green From Color Table
					paticle[loop].b = colors[ccol][2];			// Select Blue From Color Table
				}
			}
		}
	}
	//glutSwapBuffers();
	return true;
}
