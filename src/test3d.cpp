#define OLC_PGE_APPLICATION

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 256
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

#define NUM_STARS 750
#define GRID_SIZE 12

//#define ENABLE_MODEL_LOADER
//#define ENABLE_FACE_ROTATION
//#define ENABLE_STARFIELD
//#define ENABLE_GRID
#define ENABLE_FPS 

#include <string>

#ifdef ENABLE_MODEL_LOADER
#include <fstream>
#include <strstream>
#endif

#include <random>
#include <queue>
#include <string>
#include <chrono>

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
};

struct star {
  float x, y, z, o;
};

struct mat4x4 {
  float m[4][4] = { 0 };
};

struct StarField {
  std::vector<star> stars;
  int focalLength = SCREEN_W*2;
  bool warp = false;
  float centerX = (float)SCREEN_W / 2;
  float centerY = (float)SCREEN_H / 2;
  void Init() {
    for(int i = 0; i < NUM_STARS; i++){
      stars.push_back({
        .x = (float)getRandomFloat() * SCREEN_W,
        .y = (float)getRandomFloat() * SCREEN_H,
        .z = (float)getRandomFloat() * SCREEN_W,
        .o = ((float)floor(getRandomFloat() * 99) + 1) / 10
      });
    }
  }
};

struct Grid {
  const float gridDepth = 1000.0f; // Depth of the grid in the Z direction
  const float gridHeight = 120.0f; // Height of the grid in the Y direction
};

#ifdef ENABLE_MESH_LOADER
struct MeshLoader {

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
#endif

struct trianglepointindex {
  int triIndex;
  int pointIndex;
};

/*
struct LeftEye {
  //associate the vertices of the left eye with the corresponding triangles + point indexes
  std::vector<trianglepointindex> tps = {
    {15,1},
    {16,1},
    {16,2},
    {17,0},
    {17,2},
    {18,0}, 
    {56,1}
  };
  //store the vertices references for the left eye
  std::vector<vec3d*> verts;

  void load() {
    for (auto& b : tps) {
      //verts.push_back(&tris[b.triIndex].p[b.pointIndex]);
    }
  }
};

struct RightEye {
  //associate the vertices of the right eye with the corresponding triangles + point indexes
  std::vector<trianglepointindex> tps = {
    {24,1},
    {24,2},
    {26,0},
    {39,0},
    {42,0},
    {42,2},
    {71,1},
  };
  //store the vertices references for the right eye
  std::vector<vec3d*> verts;

  void load() {
    for (auto& b : tps) {
      //verts.push_back(&tris[b.triIndex].p[b.pointIndex]);
    }
  }
};
*/

struct keyduration {
  uint16_t index;
  std::chrono::duration<float> duration;
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

//Base class for model animated parts
class AnimatedObject {
  public:
    AnimatedObject(Model* model)
      : model(model), currentAnimationIndex(0), isAnimating(false) {
        load();
      }

    void QueueAnimation(uint16_t index, float duration) {
      animQueue.push({index, std::chrono::duration<float>(duration)});
    }

    void Update() {
      if (isAnimating) {
        _updateAnimation();
      } else {
        if (!animQueue.empty()) {
          auto kd = animQueue.front();
          currentAnimationIndex = kd.index;
          currentAnimationDuration = kd.duration;
          std::cout << "Starting animation " << currentAnimationIndex << " at speed " << currentAnimationDuration.count() << std::endl;
          animQueue.pop();
          isAnimating = true;
          animationStartTime = std::chrono::high_resolution_clock::now();
        } 
      }
    }
    //associate the vertices with the corresponding triangles + point indexes
    std::vector<trianglepointindex> tps;

    //store the vertices references
    std::vector<vec3d*> verts;

    //store the keyframes and their corresponding verts
    std::vector<std::vector<vec3d>> kf_points;

    void load() {
      for (auto b : tps) {
        //std::cout << "triIndex: " << b.triIndex << " pointIndex: " << b.pointIndex << std::endl;
        verts.push_back(model->tris[b.triIndex].p[b.pointIndex]);
      }
    }

  private:
    Model* model;
    bool isAnimating;
    uint16_t currentAnimationIndex;
    std::chrono::duration<float> currentAnimationDuration;
    std::queue<keyduration> animQueue;
    std::chrono::time_point<std::chrono::high_resolution_clock> animationStartTime;

    
    //move the vertices according toward current animation keyframe using speed as a factor
    void _updateAnimation() {
      
      auto now = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> elapsedTime = now - animationStartTime;
      
      float t = elapsedTime / currentAnimationDuration;

      if (t >= 1.0f) {
        t = 1.0f;
        isAnimating = false;
      }
      for (size_t i = 0; i < verts.size(); ++i) {
        vec3d* v = verts[i];
        const vec3d& start = *v;
        const vec3d& target = kf_points[currentAnimationIndex][i];
        v->x = start.x + (target.x - start.x) * t;
        v->y = start.y + (target.y - start.y) * t;
        v->z = start.z + (target.z - start.z) * t;
      }
       if (!isAnimating) {
        std::cout << "Animation " << currentAnimationIndex << " finished" << std::endl;

        // Ensure final position is exactly the target position
        for (size_t i = 0; i < verts.size(); ++i) {
          vec3d* v = verts[i];
          const vec3d& target = kf_points[currentAnimationIndex][i];
          *v = target;
        }
      }
    }
};

//Mouth part animations
class MouthPart : public AnimatedObject {
  public:

    //Animation keys
    enum KEY_FRAME {
      OPEN,
      CLOSE
    };

    MouthPart(Model* model) : AnimatedObject(model) { 
      //associate the vertices of the mouth with the corresponding triangles + point indexes
      // clang-format off
      tps = {
        {7,1},  //=v[17]
        {9,1},  //=v[16]
        {9,2},  //=v[15]
        {12,0}, //=v[18]
        {12,2}, //=v[56]
        {35,0}, //=v[46] 
        {36,0}, //=v[45]
        {33,1}, //=v[44]
        {47,1}, //=v[43]
      };
      //keyframe => vector<points>
      kf_points = {
        //TODO: real points for each frame
        //KEY_OPEN
        {
          {-0.547272, -0.677873, 0.391896},  //=v[17]
          {-0.310748, -0.546415, 0.579291},  //=v[16]
          {-0.082378, -0.539592, 0.691661},  //=v[15]
          {-0.20804, -0.78771, 0.587208},    //=v[18]
          {0.004478, -0.790754, 0.641731},   //=v[56]
          {0.216484, -0.78771, 0.587208},    //=v[46]
          {0.555716, -0.677873, 0.391896},   //=v[45]
          {0.319192, -0.546415, 0.579291},   //=v[44]
          {0.090822, -0.539592, 0.691661},   //=v[43]
        },
        //KEY_CLOSE
        {
          {-0.547272,-0.677873,0.391896},   //=v[17]
          {-0.310748,-0.6632,0.579291},     //=v[16]
          {-0.082378,-0.6447,0.691661},     //=v[15]
          {-0.20804,-0.72056,0.587208},     //=v[18]
          {0.004478,-0.71485,0.641731},     //=v[56]
          {0.216484,-0.72056,0.587208},     //=v[46]
          {0.555716,-0.677873,0.391896},    //=v[45]
          {0.319192,-0.66319,0.579291},     //=v[44]
          {0.090822,-0.6476,0.691661},      //=v[43]
        },
      }; 
      // clang-format on
      //loads tps into refs to the actual vertices
      load(); 
    }
};
 
//3d face model including animated subparts
class FaceModel : public Model {
  public:
    // clang-format off 
    //Hardcoded face model vertices
    std::vector<vec3d> verts = {
      {-0.123703, 0.001472, 0.952351},
      {-0.12059, -0.184893, 0.858286},
      {-0.096589, 0.851554, 0.378855},
      {-0.308867, 0.098008, 0.460455},
      {-0.313299, 1.02628, 0.411342},
      {-0.226048, 0.6784, 0.218026},
      {-1.03587, 0.990863, 0.116014},
      {-1.11066, 0.105018, 0.048242},
      {-0.080185, -0.31292, 0.566291},
      {-0.378635, -0.048315, 0.421444},
      {-0.304947, -0.225835, 0.473946},
      {-1.16352, 0.643507, -0.243753},
      {-1.08676, 1.641, -0.353627},
      {-0.490549, 1.88597, 0.038541},
      {-0.903636, -0.562408, -0.039296},

      //kp: 9-2. mouth [2]
      {-0.082378, -0.539592, 0.691661},

      //kp: 9-1. mouth [1]
      {-0.310748, -0.546415, 0.579291},
      
      //kp: 7-1. mouth [0]
      {-0.547272, -0.677873, 0.391896},

      //kp: 12-0 mouth [3]
      {-0.20804, -0.78771, 0.587208},

      {-0.272745, -0.967215, 0.46947},
      {-0.294976, -1.24988, 0.572406},
      {-0.530438, 0.564415, 0.182007},
      {-0.514957, 0.818628, 0.145485},
      {-0.355678, 0.667006, 0.118644},
      {-0.806416, 0.803185, 0.085337},
      {-0.793271, 0.56203, 0.112307},
      {-0.893837, 0.679656, 0.023895},
      {-0.563347, -1.1456, 0.273962},
      {0.132147, 0.001472, 0.952351},
      {0.129034, -0.184893, 0.858286},
      {0.105033, 0.851554, 0.378855},
      {0.317311, 0.098008, 0.460455},
      {0.321743, 1.02628, 0.411342},
      {0.234492, 0.6784, 0.218026},
      {1.04431, 0.990863, 0.116014},
      {1.11911, 0.105018, 0.048242},
      {0.088628, -0.31292, 0.566291},
      {0.387079, -0.048315, 0.421444},
      {0.313391, -0.225835, 0.473946},
      {1.17196, 0.643507, -0.243753},
      {1.09161, 1.63217, -0.347511},
      {0.498993, 1.88597, 0.038541},
      {0.91208, -0.562408, -0.039296},

      //kp: 47-1 mouth[8]
      {0.090822, -0.539592, 0.691661},

      //kp:33-1 mouth [7]
      {0.319192, -0.546415, 0.579291},

      //kp:36-0 mouth [6]
      {0.555716, -0.677873, 0.391896},

      //kp:35-0 mouth [5]
      {0.216484, -0.78771, 0.587208},

      {0.281189, -0.967215, 0.46947},
      {0.30342, -1.24988, 0.572406},
      {0.538881, 0.564415, 0.182007},
      {0.523401, 0.818628, 0.145485},
      {0.364122, 0.667006, 0.118644},
      {0.81486, 0.803185, 0.085337},
      {0.801715, 0.56203, 0.112307},
      {0.903642, 0.679152, 0.021932},
      {0.571791, -1.1456, 0.273962},

      //kp: 12-2 mouth [4]
      {0.004478, -0.790754, 0.641731},
    };
    //Hardcoded face model faces
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
      {3, 33, 5},
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
      {3, 31, 33},
      {33, 42, 14},
      {20, 21, 49},
    };
    // clang-format on
    MouthPart* mouth;

    FaceModel() {
      for (auto& f : faces)
      {
        triangleref t;
        t.p[0] = &verts[f.f[0] - 1];
        t.p[1] = &verts[f.f[1] - 1];
        t.p[2] = &verts[f.f[2] - 1];
        tris.push_back(t);
      } 
      mouth = new MouthPart(this);
    }

    void Update() {
      mouth->Update();
    }
};

class TextAnimator {
  public:
  TextAnimator(olc::PixelGameEngine* pge, float typeSpeed, float pauseTime, float cursorBlinkRate)
    : pge(pge), typeSpeed(typeSpeed), pauseTime(pauseTime), cursorBlinkRate(cursorBlinkRate),
      currentIndex(0), isTyping(false), cursorVisible(true), firstMessage(true), displayText(true) {
    font = std::make_unique<olc::Font>( "./sprites/test3d/font_monocode_14.png");
    lastUpdate = std::chrono::high_resolution_clock::now();
    lastCursorBlink = std::chrono::high_resolution_clock::now();
    lastPauseStart = std::chrono::high_resolution_clock::now();
  }

  void QueueText(const std::string& text) {
    textQueue.push(text);
  }

  void Update() {
    auto now = std::chrono::high_resolution_clock::now();

    if (isTyping) {
      UpdateTyping(now);
    } else {
      HandlePause(now);
    }

    UpdateCursorBlink(now);
  }

  void DrawText(int x, int y, olc::Pixel color = olc::WHITE) {
    if (displayText) {
      std::string toDraw = currentText.substr(0, currentIndex);
      font->DrawStringPropDecal( {(float)x,(float)y}, toDraw, olc::WHITE, {1.0f, 1.0f} );
      //pge->DrawString(x, y, toDraw, color);
      DrawCursor(x, y, toDraw, color);
    }
  }

  private:
  olc::PixelGameEngine* pge;
  std::unique_ptr<olc::Font> font;
  std::queue<std::string> textQueue;
  std::string currentText;
  size_t currentIndex;
  float typeSpeed;
  float pauseTime;
  float cursorBlinkRate;
  std::chrono::high_resolution_clock::time_point lastUpdate;
  std::chrono::high_resolution_clock::time_point lastCursorBlink;
  std::chrono::high_resolution_clock::time_point lastPauseStart;
  bool isTyping;
  bool cursorVisible;
  bool firstMessage;
  bool displayText;

  void StartTyping() {
    currentText = textQueue.front();
    textQueue.pop();
    currentIndex = 0;
    isTyping = true;
    lastUpdate = std::chrono::high_resolution_clock::now();
  }

  void UpdateTyping(const std::chrono::high_resolution_clock::time_point& now) {
    if (std::chrono::duration<float>(now - lastUpdate).count() >= typeSpeed) {
      currentIndex++;
      lastUpdate = now;
      if (currentIndex > currentText.size()) {
        isTyping = false;
        lastPauseStart = now;
        if (textQueue.empty()) {
          displayText = false;
        }
      }
    }
  }

  void HandlePause(const std::chrono::high_resolution_clock::time_point& now) {
    if (firstMessage || std::chrono::duration<float>(now - lastUpdate).count() >= pauseTime) {
      if (!textQueue.empty()) {
        StartTyping();
        firstMessage = false;
      }
    }

    if (textQueue.empty() && std::chrono::duration<float>(now - lastPauseStart).count() >= pauseTime) {
      displayText = false;
    }
  }

  void UpdateCursorBlink(const std::chrono::high_resolution_clock::time_point& now) {
    if (std::chrono::duration<float>(now - lastCursorBlink).count() >= cursorBlinkRate) {
      cursorVisible = !cursorVisible;
      lastCursorBlink = now;
    }
  }

  void DrawCursor(int x, int y, const std::string& toDraw, olc::Pixel color) {
    if (cursorVisible && (isTyping || (!isTyping && !textQueue.empty()))) {
      int cursorX = x + toDraw.length() * 8; // Assuming each character is 8 pixels wide
      pge->FillRect(cursorX, y, 8, 14, color); // Draw the cursor as an 8x8 rectangle
    }
  }
};

class ThreeDimensionRenderer : public olc::PixelGameEngine {
  public:
    ThreeDimensionRenderer() {
      sAppName = "3dTest";
    }

  private:

    bool bShowFps = true;
    bool bShowDebug = false;

    const float fAspectRatio = static_cast<float>(SCREEN_W) / static_cast<float>(SCREEN_H);
    const float fFov = 90.0f; // Field of view in degrees
    const float fFovRad = 1.0f / tan(fFov * 0.5f / 180.0f * 3.14159f);

    float fAccumulatedTime = 0.0f;
    float fTargetFrameTime = 100/1000.0f;
    float fTheta = 2* 3.14159f; 

    int iDbgTriIndex=0;
  
    mat4x4 matProj;

    //Fonts
    std::unique_ptr<olc::Font> pixelFont48;
    std::unique_ptr<olc::Font> computerFont20;
    std::unique_ptr<olc::Font> computerFont28;
    std::unique_ptr<olc::Font> monoFont28;

    FaceModel* faceModel;
    //FaceMesh meshFace;
    StarField starField;
    Grid grid;

    TextAnimator* animator;

    //to load the model's obj, and generate the tri vector initalization from console output
    #ifdef ENABLE_MESH_LOADER
    MeshLoader meshLoader;
    #endif
  
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
      float screenX = (x / (z * fAspectRatio)) * fFovRad * SCREEN_W + SCREEN_W / 2.0f;
      float screenY = (y / z) * fFovRad * SCREEN_H + SCREEN_H / 2.0f;
      return {screenX, screenY};
    }

    //for esp:
    void DrawText(olc::Font* font, const std::string& text, const olc::vf2d& pos, const olc::Pixel& col, const olc::vf2d& scale = {1.0f, 1.0f}) {
      font->DrawStringPropDecal(pos, text, col, scale);
    }

    void DrawGrid() {
      // Draw the one-axis perspective grid (Z-axis lines)
      for (int i = -GRID_SIZE; i <= GRID_SIZE; ++i) {
        for (int j = 1; j < grid.gridDepth; j += GRID_SIZE) {
          // Lines parallel to the X-axis
          vec2d startX = Project(-GRID_SIZE * GRID_SIZE, grid.gridHeight, j);
          vec2d endX = Project(GRID_SIZE * GRID_SIZE, grid.gridHeight, j);
          DrawLine(startX.x, startX.y, endX.x, endX.y, olc::WHITE);

          // Lines parallel to the Z-axis
          vec2d startZ = Project(i * GRID_SIZE, grid.gridHeight, j);
          vec2d endZ = Project(i * GRID_SIZE, grid.gridHeight, j + GRID_SIZE);
          DrawLine(startZ.x, startZ.y, endZ.x, endZ.y, olc::WHITE);
        }
      }
    }

    void DrawStarfield() {
      int pixelX;
      int pixelY;
      int pixelRadius;
        for(auto &star : starField.stars){
        star.z-= 0.1f;
        if(star.z <= 0){
          star.z = SCREEN_W;
        }
        pixelX = (star.x - starField.centerX) * (starField.focalLength / star.z);
        pixelX += starField.centerX;
        pixelY = (star.y - starField.centerY) * (starField.focalLength / star.z);
        pixelY += starField.centerY;
        pixelRadius = 1 * (starField.focalLength / star.z);
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
      computerFont28->DrawStringPropDecal( {static_cast<float>(SCREEN_W-text_size.x) /2 ,2.0f}, text, olc::WHITE, {fScale, fScale} );
      //monoFont28->DrawStringPropDecal( {static_cast<float>(SCREEN_W-text_size.x) /2 ,5.0f}, text, olc::WHITE, {fScale, fScale} );
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
        auto text = "i: " + std::to_string(iDbgTriIndex);
        auto text_size   = pixelFont48->GetTextSizeProp( text );
        //auto text_centre      = text_size / 2.0f;
        auto fScale                 = 1.0f;
        pixelFont48->DrawStringPropDecal( {0,(float)SCREEN_H-text_size.y}, text, olc::MAGENTA, {fScale, fScale} );
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
      for (triangleref tri : faceModel->tris) {
  
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

        //check if projected tri contains one or more left eye points
        //for(auto bidule: meshFace.leftEye.tps) {
        //  for(int i=0; i<3; i++) {
        //    vec3d trianglePoint = *tri.p[i];
        //    triangleref leftEyeTriangle = meshFace.tris[bidule.triIndex];
        //    vec3d leftEyeTrianglePoint =*leftEyeTriangle.p[bidule.pointIndex];
        //    if(trianglePoint.x == leftEyeTrianglePoint.x && trianglePoint.y == leftEyeTrianglePoint.y && trianglePoint.z == leftEyeTrianglePoint.z) {
        //      FillCircle(triProjected.p[i].x, triProjected.p[i].y, 2, olc::CYAN);
        //    }
        //  }
        // }

        //check if projected tri contains one or more right eye points
        //for(auto bidule: meshFace.rightEye.tps) {
        //  for(int i=0; i<3; i++) {
        //    vec3d trianglePoint = *tri.p[i];
        //    triangleref rightEyeTriangle = meshFace.tris[bidule.triIndex];
        //    vec3d rightEyeTrianglePoint =*rightEyeTriangle.p[bidule.pointIndex];
        //    if(trianglePoint.x == rightEyeTrianglePoint.x && trianglePoint.y == rightEyeTrianglePoint.y && trianglePoint.z == rightEyeTrianglePoint.z) {
        //      FillCircle(triProjected.p[i].x, triProjected.p[i].y, 2, olc::CYAN);
        //    }
        //  }
        //}

        //check if projected tri contains one or more mouth points
        for(auto tp: faceModel->mouth->tps) {
          for(int i=0; i<3; i++) {
            vec3d trianglePoint = *tri.p[i];
            triangleref mouthTriangle = faceModel->tris[tp.triIndex];
            vec3d mouthTrianglePoint =*mouthTriangle.p[tp.pointIndex];
            if(trianglePoint.x == mouthTrianglePoint.x && trianglePoint.y == mouthTrianglePoint.y && trianglePoint.z == mouthTrianglePoint.z) {
            FillCircle(triProjected.p[i].x, triProjected.p[i].y, 2, olc::CYAN);
            }
          }
        }

        // Rasterize triangle
        if(index == iDbgTriIndex && bShowDebug) {
          FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
            triProjected.p[1].x, triProjected.p[1].y,
            triProjected.p[2].x, triProjected.p[2].y, olc::GREY);
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

      //Load model's triangles from the file
      #ifdef ENABLE_MESH_LOADER
        meshLoader.LoadFromObjectFile("./models/3dface.obj");
      #endif
     
      faceModel = new FaceModel();
 
      starField.Init(); 

      // Load Fonts
      pixelFont48 = std::make_unique<olc::Font>( "./sprites/test3d/font_48.png");
      computerFont20 = std::make_unique<olc::Font>( "./sprites/test3d/font_computer_20.png");
      computerFont28 = std::make_unique<olc::Font>( "./sprites/test3d/font_computer_28.png");
      monoFont28 = std::make_unique<olc::Font>( "./sprites/test3d/font_monocode_28.png");

      animator = new TextAnimator(this, 0.15f, 3.0f, 0.2f);    
      animator->QueueText("Hello, world!");
      animator->QueueText("This is a test.");
      animator->QueueText("Hello, world!");
      animator->QueueText("This is a test.");
      animator->QueueText("Hello, world!");
      animator->QueueText("This is a test.");

      //queue close mouth animation
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 1.0f);
      faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
 
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

      // Update animator
      animator->Update();

      // Update face model animations
      faceModel->Update();

      Clear(olc::BLACK);

      // Draw text
      animator->DrawText(20,SCREEN_H-20);

      //input to toggle debug text
      if (GetKey(olc::Key::B).bPressed) {
        bShowDebug = !bShowDebug;
      }

      //input to toggle fps
      if (GetKey(olc::Key::F).bPressed) {
        bShowFps = !bShowFps;
      }
      
      //select next triangle
      if (GetKey(olc::Key::UP).bPressed) {
        iDbgTriIndex++;
        if(iDbgTriIndex > faceModel->tris.size()-1) 
          iDbgTriIndex = faceModel->tris.size()-1;
      }

      //select previous triangle
      if (GetKey(olc::Key::DOWN).bPressed) {
        iDbgTriIndex--;
        if(iDbgTriIndex < 0) 
          iDbgTriIndex = 0;
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
}
