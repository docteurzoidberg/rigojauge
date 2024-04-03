
#define OLC_PGE_APPLICATION

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 240
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

#include <string>

class XLevelRenderer : public olc::PixelGameEngine
{
public:
  XLevelRenderer()
  {
    sAppName = "XLevel";
  }

private:
  float fAccumulatedTime = 0.0f;
  float fTargetFrameTime = 100/1000.0f;

  bool bDrawMask = true;
  int nMaskRadius = SCREEN_W / 2;

  std::unique_ptr<olc::Font> pixelFont48;
  std::unique_ptr<olc::Sprite> sprTractor;
  
  olc::vi2d sprTractorPos = {(SCREEN_W/2) - 78,(SCREEN_H/2) -56}; 
  olc::vi2d sprTractorSize = {78,78}; 

  void DrawMask() {
    for (int y = 0; y < SCREEN_H; y++) {
      for (int x = 0; x < SCREEN_W; x++) {
        if (sqrt((x - SCREEN_W / 2) * (x - SCREEN_W / 2) + (y - SCREEN_H / 2) * (y - SCREEN_H / 2)) > nMaskRadius) {
          Draw(x, y, olc::BLACK);
        }
      }
    }
  }
  virtual bool OnUserCreate()
  {
    sprTractor = std::make_unique<olc::Sprite>("./sprites/xlevel/delorean/plane.png");
    pixelFont48 = std::make_unique<olc::Font>( "./sprites/xlevel/delorean/font_48.png");
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {
    fAccumulatedTime += fElapsedTime;

    Clear(olc::BLACK);

    //input to toggle mask
    if (GetKey(olc::Key::M).bPressed)
    {
      bDrawMask = !bDrawMask;
    }
  
    // Draw tractor sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    DrawPartialSprite(sprTractorPos,  sprTractor.get(), olc::vi2d(16,0)*sprTractorSize, sprTractorSize,2);
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

    //draw circle mask
    if(bDrawMask){
      DrawMask();
    }
    if(fAccumulatedTime>fTargetFrameTime){
      fAccumulatedTime=0.0f;
    }
    return true;
  }
};

int main() {
    XLevelRenderer renderer;
    if (renderer.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
      renderer.Start();
    return 0;
}

