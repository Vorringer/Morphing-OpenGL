#pragma once

#include <iostream>
#include "loader.h"

bool Draw(const vector<Vertex>& curr_pos);
void init_particle(Vertex initPos,const vector<Vertex> shoot_dir);
void generate_particle_pos(vector<Vertex>& curr_pos);
struct particles//structrue for particle
{
	bool active;//active or not
	float life;//last time
	float fade;//describe the decreasing of life
	float r, g, b;//color
	float x, y, z;//the position
	float xi, yi, zi;//what direction to move
	float xg, yg, zg;//gravity
};