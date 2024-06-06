#define OLC_PGE_APPLICATION

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 256
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

#include <string>
#include <fstream>
#include <strstream>
#include <random>
#include <string>

// Global random number generator and distribution
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0, 1.0);

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
  olc::Pixel color;
};

struct star {
  float x, y, z, o;
};

struct mat4x4 {
  float m[4][4] = { 0 };
};


struct MeshLoader {

  // Local cache of verts
  std::vector<vec3d> verts;

  // Local cache of faces
  std::vector<face> faces;

  // Local cache of tris referencing verts
  std::vector<triangleref> tris;

  bool LoadFromObjectFile(std::string sFilename, float scale=1.0f)
  {
    std::ifstream f(sFilename);
    if (!f.is_open())
      return false;

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
        v.x *= scale;
        v.y *= scale;
        v.z *= scale;
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

struct trianglepointindex {
  int triIndex;
  int pointIndex;
};

//Base class to load a 3d model
class Model {
  public:
    Model() {
      //load();
    }
    std::vector<vec3d> verts;
    std::vector<face> faces;
    std::vector<triangleref> tris;
};

class ModelViewer : public olc::PixelGameEngine {
  public:
    ModelViewer() {
      sAppName = "Model Viewer";
    }

  private:

    bool bShowFps = true;
    bool bShowDebug = false;
    bool bRotate = false;
    bool bWireFrame = false;

    const float fAspectRatio = static_cast<float>(SCREEN_W) / static_cast<float>(SCREEN_H);
    const float fFov = 90.0f; // Field of view in degrees
    const float fFovRad = 1.0f / tan(fFov * 0.5f / 180.0f * 3.14159f);

    float fAccumulatedTime = 0.0f;
    float fTargetFrameTime = 100/1000.0f;
    float fTheta = 2* 3.14159f; 

    int iDbgTriIndex=0;
  
    mat4x4 matProj;
    vec3d vCamera;

    //Fonts
    std::unique_ptr<olc::Font> pixelFont48;
    std::unique_ptr<olc::Font> computerFont20;
    std::unique_ptr<olc::Font> computerFont28;
    std::unique_ptr<olc::Font> monoFont28;

    MeshLoader meshLoader;
  
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

    //for esp:
    void DrawText(olc::Font* font, const std::string& text, const olc::vf2d& pos, const olc::Pixel& col, const olc::vf2d& scale = {1.0f, 1.0f}) {
      font->DrawStringPropDecal(pos, text, col, scale);
    }

    void DrawFPS(uint16_t fps) {
      auto text = "FPS:"+ std::to_string(fps);
      auto text_size   = computerFont20->GetTextSizeProp( text );
      auto text_centre = text_size / 2.0f;
      auto fScale                 = 1.0f;
      computerFont20->DrawStringPropDecal( {static_cast<float>(SCREEN_W-text_size.x),static_cast<float>(SCREEN_H-text_size.y)}, text, olc::WHITE, {fScale, fScale} );
    }

    void DrawModel() {
    
      // Set up rotation matrices
      mat4x4 matRotZ, matRotX;
      
      if(bRotate) {
        fTheta += 0.001f; //1.0f * fElapsedTime;
      }
      // debug
      if(bShowDebug) {
        auto text = "i: " + std::to_string(iDbgTriIndex);
        auto text_size   = pixelFont48->GetTextSizeProp( text );
        //auto text_centre      = text_size / 2.0f;
        auto fScale                 = 1.0f;
        pixelFont48->DrawStringPropDecal( {0,(float)SCREEN_H-text_size.y}, text, olc::MAGENTA, {fScale, fScale} );
      }

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

      // Store triagles for rastering later
		  std::vector<triangle> vecTrianglesToRaster;
    
       // Draw Triangles
      for (auto tri : meshLoader.tris)
      {
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

        // Use Cross-Product to get surface normal
        vec3d normal, line1, line2;
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

        normal.x = line1.y * line2.z - line1.z * line2.y;
        normal.y = line1.z * line2.x - line1.x * line2.z;
        normal.z = line1.x * line2.y - line1.y * line2.x;

        // It's normally normal to normalise the normal
        float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        normal.x /= l; normal.y /= l; normal.z /= l;

        //if (normal.z < 0)
        if(normal.x * (triTranslated.p[0].x - vCamera.x) + 
          normal.y * (triTranslated.p[0].y - vCamera.y) +
          normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
        {
          // Illumination
          vec3d light_direction = { 0.0f, 0.0f, -1.0f };
          float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
          light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

          // How similar is normal to light direction
          float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

          // Choose console colours as required (much easier with RGB)
          //CHAR_INFO c = GetColour(dp);
          
          //triTranslated.sym = c.Char.UnicodeChar;

          auto trianglecol = olc::Pixel(dp*255,dp*255,dp*255);
          triTranslated.color = trianglecol;

          // Project triangles from 3D --> 2D
          MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
          MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
          MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

          triProjected.color = trianglecol;

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

          // Store triangle for sorting
          vecTrianglesToRaster.push_back(triProjected);
        }

      }

      // Sort triangles from back to front
      sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
      {
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
      });

      for (auto &triProjected : vecTrianglesToRaster)
      {
        if(bWireFrame) {
         DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
        triProjected.p[1].x, triProjected.p[1].y,
        triProjected.p[2].x, triProjected.p[2].y,
        olc::Pixel(255,200,0));
        } else {
        // Rasterize triangle
        FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
          triProjected.p[1].x, triProjected.p[1].y,
          triProjected.p[2].x, triProjected.p[2].y,
          triProjected.color); 
        }

        
      }
    }


    virtual bool OnUserCreate() {

      //Load model's triangles from the file
      meshLoader.LoadFromObjectFile("./models/scout.obj", 3.6f);

      // Load Fonts
      pixelFont48 = std::make_unique<olc::Font>( "./sprites/test3d/font_48.png");
      computerFont20 = std::make_unique<olc::Font>( "./sprites/test3d/font_computer_20.png");
      computerFont28 = std::make_unique<olc::Font>( "./sprites/test3d/font_computer_28.png");
      monoFont28 = std::make_unique<olc::Font>( "./sprites/test3d/font_monocode_28.png");
 
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

      return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {

      fAccumulatedTime += fElapsedTime;

      Clear(olc::BLACK);

      //input to toggle debug text
      if (GetKey(olc::Key::B).bPressed) {
        bShowDebug = !bShowDebug;
      }

      if (GetKey(olc::Key::W).bPressed) {
        bWireFrame = !bWireFrame;
      }

      //input to toggle fps
      if (GetKey(olc::Key::F).bPressed) {
        bShowFps = !bShowFps;
      }

      //input to toggle rotation
      if (GetKey(olc::Key::R).bPressed) {
        bRotate = !bRotate;
      }
      
      //select next triangle
      if (GetKey(olc::Key::UP).bPressed) {
        iDbgTriIndex++;
        if(iDbgTriIndex > meshLoader.tris.size()-1) 
          iDbgTriIndex = meshLoader.tris.size()-1;
      }

      //select previous triangle
      if (GetKey(olc::Key::DOWN).bPressed) {
        iDbgTriIndex--;
        if(iDbgTriIndex < 0) 
          iDbgTriIndex = 0;
      }

      // Draw the model
      DrawModel();
      
      DrawFPS(GetFPS());
    
      return true;
    }
};

int main() {
  ModelViewer app;
  if (app.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
    app.Start();
}
