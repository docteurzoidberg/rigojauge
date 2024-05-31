#define OLC_PGE_APPLICATION

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 256
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

#define NUM_STARS 750
#define GRID_SIZE 12

//#define ENABLE_FACE_ROTATION 1
//#define ENABLE_STARFIELD 1
//#define ENABLE_GRID 0
#define ENABLE_FPS 1

#include <string>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <iostream>
#include <random>

// Global random number generator and distribution
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0, 1.0);

int triIndex=0;

// Function to get a random float
float getRandomFloat() {
    return dis(gen);
}

struct vec2d {
	float x, y;
};

struct vec3d {
	float x, y, z;
};

struct face {
	int f[3];
};

struct triangleref {
	vec3d* p[3];
};

struct triangle {
	vec3d p[3];
};

struct mesh {
	// Local cache of verts
	std::vector<vec3d> verts; 

	// Local cache of faces
	std::vector<face> faces;

	// Local cache of tris referencing verts
	std::vector<triangleref> tris;

  
	bool LoadFromObjectFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		//std::cout << "{" << std::endl;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				faces.push_back({f[0], f[1], f[2]});
				
				triangleref t;
				t.p[0] = &verts[f[0] - 1];
				t.p[1] = &verts[f[1] - 1];
				t.p[2] = &verts[f[2] - 1];
				tris.push_back(t);
			}
		}
		//std::cout << "}" << std::endl;


		//Output the verts vector to console
		std::cout << "std::vector<vec3d> verts = {" << std::endl;
		for (auto& v : verts)
		{
			std::cout << "  {" << v.x << ", " << v.y << ", " << v.z << "}," << std::endl;
		}
		std::cout << "};" << std::endl;

		//Output the faces vector to console
		std::cout << "std::vector<face> faces = {" << std::endl;	
		for (auto& f : faces)
		{
			std::cout << "  {" << f.f[0] << ", " << f.f[1] << ", " << f.f[2] << "}," << std::endl;
		}
		std::cout << "};" << std::endl;	

    return true;
  }
};

struct mat4x4 {
	float m[4][4] = { 0 };
};

struct FaceMesh {
	std::vector<vec3d> verts = {
		{-0.119486, 0.13388, 1.2632},
		{-0.116373, -0.052441, 1.16916},
		{-0.092378, 0.98376, 0.689843},
		{-0.304605, 0.230392, 0.771424},
		{-0.309036, 1.15845, 0.722323},
		{-0.221806, 0.810647, 0.529052},
		{-1.03143, 1.12304, 0.427065},
		{-1.10621, 0.237401, 0.359309},
		{-0.075977, -0.180438, 0.877235},
		{-0.374356, 0.084104, 0.732422},
		{-0.300686, -0.093373, 0.784912},
		{-1.15905, 0.775762, 0.067383},
		{-1.08231, 1.77302, -0.042465},
		{-0.486244, 2.01793, 0.34961},
		{-0.899232, -0.429866, 0.271792},
		{-0.078171, -0.407055, 1.00258},
		{-0.306486, -0.413877, 0.890231},
		{-0.542953, -0.545303, 0.702881},
		{-0.203802, -0.655114, 0.898147},
		{-0.268492, -0.834577, 0.780437},
		{-0.290718, -1.11718, 0.883348},
		{-0.526123, 0.696689, 0.493042},
		{-0.510646, 0.950842, 0.456529},
		{-0.351405, 0.799255, 0.429694},
		{-0.802036, 0.935402, 0.396395},
		{-0.788894, 0.694305, 0.423358},
		{-0.889436, 0.811903, 0.334968},
		{-0.559025, -1.01292, 0.584976},
		{0.136304, 0.13388, 1.2632},
		{0.133191, -0.052441, 1.16916},
		{0.109196, 0.98376, 0.689843},
		{0.321424, 0.230392, 0.771424},
		{0.325855, 1.15845, 0.722323},
		{0.238624, 0.810647, 0.529052},
		{1.04825, 1.12304, 0.427065},
		{1.12303, 0.237401, 0.359309},
		{0.092796, -0.180438, 0.877235},
		{0.391175, 0.084104, 0.732422},
		{0.317505, -0.093373, 0.784912},
		{1.17587, 0.775762, 0.067383},
		{1.09554, 1.76419, -0.03635},
		{0.503063, 2.01793, 0.34961},
		{0.916051, -0.429866, 0.271792},
		{0.094989, -0.407055, 1.00258},
		{0.323305, -0.413877, 0.890231},
		{0.559772, -0.545303, 0.702881},
		{0.22062, -0.655114, 0.898147},
		{0.28531, -0.834577, 0.780437},
		{0.307536, -1.11718, 0.883348},
		{0.542941, 0.696689, 0.493042},
		{0.527464, 0.950842, 0.456529},
		{0.368223, 0.799255, 0.429694},
		{0.818854, 0.935402, 0.396395},
		{0.805713, 0.694305, 0.423358},
		{0.906254, 0.811903, 0.334968},
		{0.575843, -1.01292, 0.584976},
		{0.008665, -0.658158, 0.952657},
	};

	std::vector<face> faces = {
		{6, 3, 5},
		{27, 8, 26},
		{7, 14, 13},
		{6, 22, 4},
		{7, 13, 12},
		{9, 2, 11},
		{8, 10, 4},
		{15, 18, 10},
		{10, 2, 1},
		{9, 17, 16},
		{11, 18, 17},
		{18, 15, 28},
		{19, 20, 57},
		{18, 20, 19},
		{18, 11, 10},
		{7, 23, 5},
		{7, 27, 25},
		{26, 4, 22},
		{23, 6, 5},
		{20, 18, 28},
		{21, 20, 28},
		{1, 4, 10},
		{3, 4, 1},
		{34, 33, 31},
		{36, 55, 54},
		{35, 42, 33},
		{50, 34, 32},
		{35, 40, 41},
		{37, 39, 30},
		{38, 36, 32},
		{43, 38, 46},
		{30, 38, 29},
		{37, 45, 39},
		{39, 45, 46},
		{46, 56, 43},
		{47, 57, 48},
		{46, 47, 48},
		{20, 48, 57},
		{46, 38, 39},
		{51, 35, 33},
		{35, 55, 40},
		{32, 54, 50},
		{51, 34, 52},
		{48, 56, 46},
		{49, 56, 48},
		{29, 38, 32},
		{32, 31, 29},
		{9, 44, 37},
		{2, 37, 30},
		{30, 1, 2},
		{29, 3, 1},
		{31, 5, 3},
		{33, 14, 5},
		{20, 49, 48},
		{27, 12, 8},
		{7, 5, 14},
		{6, 24, 22},
		{8, 15, 10},
		{10, 11, 2},
		{9, 11, 17},
		{7, 25, 23},
		{7, 12, 27},
		{26, 8, 4},
		{23, 24, 6},
		{3, 6, 4},
		{36, 40, 55},
		{35, 41, 42},
		{50, 52, 34},
		{38, 43, 36},
		{30, 39, 38},
		{37, 44, 45},
		{51, 53, 35},
		{35, 53, 55},
		{32, 36, 54},
		{51, 33, 34},
		{32, 34, 31},
		{9, 16, 44},
		{2, 9, 37},
		{30, 29, 1},
		{29, 31, 3},
		{31, 33, 5},
		{33, 42, 14},
		{20, 21, 49},
	};

	std::vector<triangleref> tris;

	void LoadModel() {
		for (auto& f : faces)
		{
			triangleref t;
			t.p[0] = &verts[f.f[0] - 1];
			t.p[1] = &verts[f.f[1] - 1];
			t.p[2] = &verts[f.f[2] - 1];
			tris.push_back(t);
		}
	}

	/*
	std::vector<triangle> tris = {
		{
			{{ {-0.221806,0.810647,0.529052}, {-0.092378,0.98376,0.689843}, {-0.309036,1.15845,0.722323} }},
			{{ {-0.889436,0.811903,0.334968}, {-1.10621,0.237401,0.359309}, {-0.788894,0.694305,0.423358} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.486244,2.01793,0.34961}, {-1.08231,1.77302,-0.042465} }},
			{{ {-0.221806,0.810647,0.529052}, {-0.526123,0.696689,0.493042}, {-0.304605,0.230392,0.771424} }},
			{{ {-1.03143,1.12304,0.427065}, {-1.08231,1.77302,-0.042465}, {-1.15905,0.775762,0.067383} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.116373,-0.052441,1.16916}, {-0.300686,-0.093373,0.784912} }},
			{{ {-1.10621,0.237401,0.359309}, {-0.374356,0.084104,0.732422}, {-0.304605,0.230392,0.771424} }},
			{{ {-0.899232,-0.429866,0.271792}, {-0.542953,-0.545303,0.702881}, {-0.374356,0.084104,0.732422} }},
			{{ {-0.374356,0.084104,0.732422}, {-0.116373,-0.052441,1.16916}, {-0.119486,0.13388,1.2632} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.306486,-0.413877,0.890231}, {-0.078171,-0.407055,1.00258} }},
			{{ {-0.300686,-0.093373,0.784912}, {-0.542953,-0.545303,0.702881}, {-0.306486,-0.413877,0.890231} }},
			{{ {-0.542953,-0.545303,0.702881}, {-0.899232,-0.429866,0.271792}, {-0.559025,-1.01292,0.584976} }},
			{{ {-0.203802,-0.655114,0.898147}, {-0.268492,-0.834577,0.780437}, {0.008665,-0.658158,0.952657} }},
			{{ {-0.542953,-0.545303,0.702881}, {-0.268492,-0.834577,0.780437}, {-0.203802,-0.655114,0.898147} }},
			{{ {-0.542953,-0.545303,0.702881}, {-0.300686,-0.093373,0.784912}, {-0.374356,0.084104,0.732422} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.510646,0.950842,0.456529}, {-0.309036,1.15845,0.722323} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.889436,0.811903,0.334968}, {-0.802036,0.935402,0.396395} }},
			{{ {-0.788894,0.694305,0.423358}, {-0.304605,0.230392,0.771424}, {-0.526123,0.696689,0.493042} }},
			{{ {-0.510646,0.950842,0.456529}, {-0.221806,0.810647,0.529052}, {-0.309036,1.15845,0.722323} }},
			{{ {-0.268492,-0.834577,0.780437}, {-0.542953,-0.545303,0.702881}, {-0.559025,-1.01292,0.584976} }},
			{{ {-0.290718,-1.11718,0.883348}, {-0.268492,-0.834577,0.780437}, {-0.559025,-1.01292,0.584976} }},
			{{ {-0.119486,0.13388,1.2632}, {-0.304605,0.230392,0.771424}, {-0.374356,0.084104,0.732422} }},
			{{ {-0.092378,0.98376,0.689843}, {-0.304605,0.230392,0.771424}, {-0.119486,0.13388,1.2632} }},
			{{ {0.238624,0.810647,0.529052}, {0.325855,1.15845,0.722323}, {0.109196,0.98376,0.689843} }},
			{{ {1.12303,0.237401,0.359309}, {0.906254,0.811903,0.334968}, {0.805713,0.694305,0.423358} }},
			{{ {1.04825,1.12304,0.427065}, {0.503063,2.01793,0.34961}, {0.325855,1.15845,0.722323} }},
			{{ {0.542941,0.696689,0.493042}, {0.238624,0.810647,0.529052}, {0.321424,0.230392,0.771424} }},
			{{ {1.04825,1.12304,0.427065}, {1.17587,0.775762,0.067383}, {1.09554,1.76419,-0.03635} }},
			{{ {0.092796,-0.180438,0.877235}, {0.317505,-0.093373,0.784912}, {0.133191,-0.052441,1.16916} }},
			{{ {0.391175,0.084104,0.732422}, {1.12303,0.237401,0.359309}, {0.321424,0.230392,0.771424} }},
			{{ {0.916051,-0.429866,0.271792}, {0.391175,0.084104,0.732422}, {0.559772,-0.545303,0.702881} }},
			{{ {0.133191,-0.052441,1.16916}, {0.391175,0.084104,0.732422}, {0.136304,0.13388,1.2632} }},
			{{ {0.092796,-0.180438,0.877235}, {0.323305,-0.413877,0.890231}, {0.317505,-0.093373,0.784912} }},
			{{ {0.317505,-0.093373,0.784912}, {0.323305,-0.413877,0.890231}, {0.559772,-0.545303,0.702881} }},
			{{ {0.559772,-0.545303,0.702881}, {0.575843,-1.01292,0.584976}, {0.916051,-0.429866,0.271792} }},
			{{ {0.22062,-0.655114,0.898147}, {0.008665,-0.658158,0.952657}, {0.28531,-0.834577,0.780437} }},
			{{ {0.559772,-0.545303,0.702881}, {0.22062,-0.655114,0.898147}, {0.28531,-0.834577,0.780437} }},
			{{ {-0.268492,-0.834577,0.780437}, {0.28531,-0.834577,0.780437}, {0.008665,-0.658158,0.952657} }},
			{{ {0.559772,-0.545303,0.702881}, {0.391175,0.084104,0.732422}, {0.317505,-0.093373,0.784912} }},
			{{ {0.527464,0.950842,0.456529}, {1.04825,1.12304,0.427065}, {0.325855,1.15845,0.722323} }},
			{{ {1.04825,1.12304,0.427065}, {0.906254,0.811903,0.334968}, {1.17587,0.775762,0.067383} }},
			{{ {0.321424,0.230392,0.771424}, {0.805713,0.694305,0.423358}, {0.542941,0.696689,0.493042} }},
			{{ {0.527464,0.950842,0.456529}, {0.238624,0.810647,0.529052}, {0.368223,0.799255,0.429694} }},
			{{ {0.28531,-0.834577,0.780437}, {0.575843,-1.01292,0.584976}, {0.559772,-0.545303,0.702881} }},
			{{ {0.307536,-1.11718,0.883348}, {0.575843,-1.01292,0.584976}, {0.28531,-0.834577,0.780437} }},
			{{ {0.136304,0.13388,1.2632}, {0.391175,0.084104,0.732422}, {0.321424,0.230392,0.771424} }},
			{{ {0.321424,0.230392,0.771424}, {0.109196,0.98376,0.689843}, {0.136304,0.13388,1.2632} }},
			{{ {-0.075977,-0.180438,0.877235}, {0.094989,-0.407055,1.00258}, {0.092796,-0.180438,0.877235} }},
			{{ {-0.116373,-0.052441,1.16916}, {0.092796,-0.180438,0.877235}, {0.133191,-0.052441,1.16916} }},
			{{ {0.133191,-0.052441,1.16916}, {-0.119486,0.13388,1.2632}, {-0.116373,-0.052441,1.16916} }},
			{{ {0.136304,0.13388,1.2632}, {-0.092378,0.98376,0.689843}, {-0.119486,0.13388,1.2632} }},
			{{ {0.109196,0.98376,0.689843}, {-0.309036,1.15845,0.722323}, {-0.092378,0.98376,0.689843} }},
			{{ {0.325855,1.15845,0.722323}, {-0.486244,2.01793,0.34961}, {-0.309036,1.15845,0.722323} }},
			{{ {-0.268492,-0.834577,0.780437}, {0.307536,-1.11718,0.883348}, {0.28531,-0.834577,0.780437} }},
			{{ {-0.889436,0.811903,0.334968}, {-1.15905,0.775762,0.067383}, {-1.10621,0.237401,0.359309} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.309036,1.15845,0.722323}, {-0.486244,2.01793,0.34961} }},
			{{ {-0.221806,0.810647,0.529052}, {-0.351405,0.799255,0.429694}, {-0.526123,0.696689,0.493042} }},
			{{ {-1.10621,0.237401,0.359309}, {-0.899232,-0.429866,0.271792}, {-0.374356,0.084104,0.732422} }},
			{{ {-0.374356,0.084104,0.732422}, {-0.300686,-0.093373,0.784912}, {-0.116373,-0.052441,1.16916} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.300686,-0.093373,0.784912}, {-0.306486,-0.413877,0.890231} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.802036,0.935402,0.396395}, {-0.510646,0.950842,0.456529} }},
			{{ {-1.03143,1.12304,0.427065}, {-1.15905,0.775762,0.067383}, {-0.889436,0.811903,0.334968} }},
			{{ {-0.788894,0.694305,0.423358}, {-1.10621,0.237401,0.359309}, {-0.304605,0.230392,0.771424} }},
			{{ {-0.510646,0.950842,0.456529}, {-0.351405,0.799255,0.429694}, {-0.221806,0.810647,0.529052} }},
			{{ {-0.092378,0.98376,0.689843}, {-0.221806,0.810647,0.529052}, {-0.304605,0.230392,0.771424} }},
			{{ {1.12303,0.237401,0.359309}, {1.17587,0.775762,0.067383}, {0.906254,0.811903,0.334968} }},
			{{ {1.04825,1.12304,0.427065}, {1.09554,1.76419,-0.03635}, {0.503063,2.01793,0.34961} }},
			{{ {0.542941,0.696689,0.493042}, {0.368223,0.799255,0.429694}, {0.238624,0.810647,0.529052} }},
			{{ {0.391175,0.084104,0.732422}, {0.916051,-0.429866,0.271792}, {1.12303,0.237401,0.359309} }},
			{{ {0.133191,-0.052441,1.16916}, {0.317505,-0.093373,0.784912}, {0.391175,0.084104,0.732422} }},
			{{ {0.092796,-0.180438,0.877235}, {0.094989,-0.407055,1.00258}, {0.323305,-0.413877,0.890231} }},
			{{ {0.527464,0.950842,0.456529}, {0.818854,0.935402,0.396395}, {1.04825,1.12304,0.427065} }},
			{{ {1.04825,1.12304,0.427065}, {0.818854,0.935402,0.396395}, {0.906254,0.811903,0.334968} }},
			{{ {0.321424,0.230392,0.771424}, {1.12303,0.237401,0.359309}, {0.805713,0.694305,0.423358} }},
			{{ {0.527464,0.950842,0.456529}, {0.325855,1.15845,0.722323}, {0.238624,0.810647,0.529052} }},
			{{ {0.321424,0.230392,0.771424}, {0.238624,0.810647,0.529052}, {0.109196,0.98376,0.689843} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.078171,-0.407055,1.00258}, {0.094989,-0.407055,1.00258} }},
			{{ {-0.116373,-0.052441,1.16916}, {-0.075977,-0.180438,0.877235}, {0.092796,-0.180438,0.877235} }},
			{{ {0.133191,-0.052441,1.16916}, {0.136304,0.13388,1.2632}, {-0.119486,0.13388,1.2632} }},
			{{ {0.136304,0.13388,1.2632}, {0.109196,0.98376,0.689843}, {-0.092378,0.98376,0.689843} }},
			{{ {0.109196,0.98376,0.689843}, {0.325855,1.15845,0.722323}, {-0.309036,1.15845,0.722323} }},
			{{ {0.325855,1.15845,0.722323}, {0.503063,2.01793,0.34961}, {-0.486244,2.01793,0.34961} }},
			{{ {-0.268492,-0.834577,0.780437}, {-0.290718,-1.11718,0.883348}, {0.307536,-1.11718,0.883348} }}
		}
	};
	*/
};
 
struct Star {
	float x, y, z, o;
};

class ThreeDimensionRenderer : public olc::PixelGameEngine {
public:
  ThreeDimensionRenderer() {
    sAppName = "VanAssistant";
  }

private:
  float fAccumulatedTime = 0.0f;
  float fTargetFrameTime = 100/1000.0f;

  bool bShowFps = true;
  bool bShowDebug = false;
	
	//Fonts
  std::unique_ptr<olc::Font> pixelFont48;
	std::unique_ptr<olc::Font> computerFont20;
	std::unique_ptr<olc::Font> computerFont28;

	//3D FACE
  FaceMesh meshFace;

	//to load the model's obj, and generate the tri vector initalization from console output, uncomment:
	mesh meshLoader;
	
	mat4x4 matProj;
	float fTheta = 2* 3.14159f;

	//STARFIELD
	int focalLength = SCREEN_W*2;
	bool warp = false;
	float centerX = SCREEN_W / 2;
	float centerY = SCREEN_H / 2;
	std::vector<Star> stars;

	//GRID
	const float gridDepth = 1000.0f; // Depth of the grid in the Z direction
	const float gridHeight = 120.0f; // Height of the grid in the Y direction
	const float fov = 90.0f; // Field of view in degrees
	const float aspectRatio = static_cast<float>(SCREEN_W) / static_cast<float>(SCREEN_H);
	const float fovRad = 1.0f / tan(fov * 0.5f / 180.0f * 3.14159f);

	void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) {
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}

	// Function to project 3D coordinates to 2D screen coordinates
	vec2d Project(float x, float y, float z ) {
		float screenX = (x / (z * aspectRatio)) * fovRad * SCREEN_W + SCREEN_W / 2.0f;
		float screenY = (y / z) * fovRad * SCREEN_H + SCREEN_H / 2.0f;
		return {screenX, screenY};
	}

	//for esp:
	void DrawText(olc::Font* font, const std::string& text, const olc::vf2d& pos, const olc::Pixel& col, const olc::vf2d& scale = {1.0f, 1.0f}) {
		font->DrawStringPropDecal(pos, text, col, scale);
	}

	void InitStartField() {
		stars.clear();
		for(int i = 0; i < NUM_STARS; i++){
			stars.push_back({
				x: (float)getRandomFloat() * SCREEN_W,
				y: (float)getRandomFloat() * SCREEN_H,
				z: (float)getRandomFloat() * SCREEN_W,
				o: ((float)floor(getRandomFloat() * 99) + 1) / 10
			});
		}
	}

	void DrawGrid() {
		// Draw the one-axis perspective grid (Z-axis lines)
		for (int i = -GRID_SIZE; i <= GRID_SIZE; ++i) {
			for (int j = 1; j < gridDepth; j += GRID_SIZE) {
				
				// Lines parallel to the X-axis
				vec2d startX = Project(-GRID_SIZE * GRID_SIZE, gridHeight, j);
				vec2d endX = Project(GRID_SIZE * GRID_SIZE, gridHeight, j);
				DrawLine(startX.x, startX.y, endX.x, endX.y, olc::WHITE);

				// Lines parallel to the Z-axis
				vec2d startZ = Project(i * GRID_SIZE, gridHeight, j);
				vec2d endZ = Project(i * GRID_SIZE, gridHeight, j + GRID_SIZE);
				DrawLine(startZ.x, startZ.y, endZ.x, endZ.y, olc::WHITE);
			}
		}
	}

	void DrawStarfield() {

		int pixelX;
		int pixelY;
		int pixelRadius;

		for(auto &star : stars){
			star.z-= 0.1f;
			if(star.z <= 0){
				star.z = SCREEN_W;
			}
			pixelX = (star.x - centerX) * (focalLength / star.z);
			pixelX += centerX;
			pixelY = (star.y - centerY) * (focalLength / star.z);
			pixelY += centerY;
			pixelRadius = 1 * (focalLength / star.z);
			uint8_t brightness = star.o * 255;
			auto pixelColor = olc::Pixel(brightness,brightness,brightness);
			FillRect(pixelX, pixelY, pixelRadius, pixelRadius, pixelColor);
		}
	}

	void DrawTitle() {
		auto text = "Van Assistant 2000";
		auto text_size   = computerFont28->GetTextSizeProp( text );
		auto text_centre = text_size / 2.0f;
		auto fScale                 = 1.0f;
		computerFont28->DrawStringPropDecal( {static_cast<float>(SCREEN_W-text_size.x) /2 ,5.0f}, text, olc::WHITE, {fScale, fScale} );
	}

	void DrawFPS(uint16_t fps) {
		auto text = "FPS:"+ std::to_string(fps);
		auto text_size   = computerFont20->GetTextSizeProp( text );
		auto text_centre = text_size / 2.0f;
		auto fScale                 = 1.0f;
		computerFont20->DrawStringPropDecal( {static_cast<float>(SCREEN_W-text_size.x),static_cast<float>(SCREEN_H-text_size.y)}, text, olc::WHITE, {fScale, fScale} );
	}

	void Draw3dFace() {
	  
		 // Set up rotation matrices
		mat4x4 matRotZ, matRotX;
		
		#ifdef ENABLE_FACE_ROTATION
    	fTheta += 0.001f; //1.0f * fElapsedTime;
		#endif
		
    // debug
    if(bShowDebug) {
      auto text = "i: " + std::to_string(triIndex);
      auto text_size   = pixelFont48->GetTextSizeProp( text );
      //auto text_centre      = text_size / 2.0f;
      auto fScale                 = 1.0f;
      pixelFont48->DrawStringPropDecal( {0,SCREEN_H-text_size.y}, text, olc::MAGENTA, {fScale, fScale} );
    }

		//DrZoid: test point movement
		//meshLoader.verts[0].x += 0.0001f;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;
		
		// Draw Triangles
		int index=0;
		for (triangleref tri : meshFace.tris) {
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate in Z-Axis
			MultiplyMatrixVector(*tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(*tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(*tri.p[2], triRotatedZ.p[2], matRotZ);

			// Rotate in X-Axis
			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			// Offset into the screen
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			// Project triangles from 3D --> 2D
			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

			// Scale into view
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
			triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
			triProjected.p[2].y *= 0.5f * (float)ScreenHeight();
 
			// Rasterize triangle
			if(index == triIndex) {
				FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y, olc::RED);
				FillCircle(triProjected.p[0].x, triProjected.p[0].y, 4, olc::RED);
				FillCircle(triProjected.p[1].x, triProjected.p[1].y, 4, olc::GREEN);
				FillCircle(triProjected.p[2].x, triProjected.p[2].y, 4, olc::BLUE);

			} else {
				DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y, olc::WHITE);
			}
			index++;
		}
	}

  virtual bool OnUserCreate() {

		//meshLoader.LoadFromObjectFile("./models/face2x.obj");
		meshFace.LoadModel();

		// Load Fonts
    pixelFont48 = std::make_unique<olc::Font>( "./sprites/test3d/font_48.png");
		computerFont20 = std::make_unique<olc::Font>( "./sprites/test3d/font_computer_20.png");
		computerFont28 = std::make_unique<olc::Font>( "./sprites/test3d/font_computer_28.png");

    // Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		InitStartField();

    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime) {

    fAccumulatedTime += fElapsedTime;

    Clear(olc::BLACK);

    //input to toggle debug text
    if (GetKey(olc::Key::B).bPressed) {
      bShowDebug = !bShowDebug;
    }

    //input to toggle fps
    if (GetKey(olc::Key::F).bPressed) {
      bShowFps = !bShowFps;
    }

		
    if (GetKey(olc::Key::UP).bPressed) {
      triIndex++;
			if(triIndex > meshFace.tris.size()) 
				triIndex = meshFace.tris.size();
    }

		 //input to toggle fps
    if (GetKey(olc::Key::DOWN).bPressed)
    {
      triIndex--;
			if(triIndex < 0) triIndex = 0;
    }

		DrawTitle();
	
		#ifdef ENABLE_STARFIELD
			DrawStarfield();
		#endif

		#ifdef ENABLE_GRID
			DrawGrid();
		#endif

		Draw3dFace();
		
		#ifdef ENABLE_FPS
			DrawFPS(GetFPS());
		#endif
  
    if(fAccumulatedTime>fTargetFrameTime) {
      fAccumulatedTime=0.0f;
    }
    return true;
  }
};

int main() {
	ThreeDimensionRenderer renderer;
	if (renderer.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
		renderer.Start();
	return 0;
}
