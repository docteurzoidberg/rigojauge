
#include <cstdint>
#include <memory>
#define OLC_PGE_APPLICATION

#include <string>
using namespace std;

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 240
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

struct Segment {
  uint32_t nIndex=0; 
  uint32_t nHeight=0; //vertical size of a segment
};

struct Section {

  //Section x curvature
  float fCurvatureX=0.0f; 
  //Section y curvature
  float fCurvatureY=0.0f;

  uint8_t fType = 0;

  //Straight, no slope = 6 segments;
  //curved. uphill = 7 segments;
  //curved. no slope = 5 segments;
  //straight, downhill = 6 segments;

  std::vector<Segment> segments;
};

class Outrun
{
public:

  std::vector<Section> sections;
  Outrun()
  {
    initTrack();
  }

  void initTrack() {

    //create 4 flat straight sections
    for (int sectionIndex = 0; sectionIndex < 4; sectionIndex++) {
      Section s;
      s.fCurvatureX = 0.0f;
      s.fCurvatureY = 0.0f;
      s.fType = 0;
      for (int j = 0; j < 6; j++) {
        Segment seg;
        seg.nIndex = sectionIndex* 6 +j; 
        
        //todo: set height based on section type
        //seg.nHeight = 
        
        //add start flag ti 4th segment of first section
        if(sectionIndex==0 && j==4) {
          //seg.sprite = 
          //TODO
        }
        s.segments.push_back(seg);
      }
      sections.push_back(s);
    }
  }

  void start() {
    bStarted = true;
  }

  void update(float fElapsedTime) {

  }

  bool bStarted = false;

private:

};

class OutrunRenderer : public olc::PixelGameEngine
{
public:
  OutrunRenderer(string theme="default")
  {
    sAppName = "Outrun";
    sTheme = theme;
  }

private:  

  Outrun game;
  string sTheme;
  
  bool bShowDebug = false;
  bool bDrawMask = true;
 
  //colors
  olc::Pixel COLOR_GRASS_DARK = olc::Pixel(15,143,137);  //dark green
  olc::Pixel COLOR_GRASS_LIGHT = olc::Pixel(51,181,136); //lighter green
  olc::Pixel COLOR_RUMBLE = olc::RED;
  olc::Pixel COLOR_ROAD = olc::Pixel(30,30,30); 
  olc::Pixel COLOR_MIDLANE = olc::WHITE;
  olc::Pixel COLOR_SKY = olc::Pixel(20,87,141); //

  //sprites
  std::unique_ptr<olc::Font> pixelFont24;
  std::unique_ptr<olc::Font> pixelFont48;

  std::unique_ptr<olc::Sprite> sprCar;          //car sprite sheet

  std::unique_ptr<olc::Sprite> sprStartFlag;    //start/finish flag

  std::unique_ptr<olc::Sprite> sprTree1;        //roadside tree model #1
  std::unique_ptr<olc::Sprite> sprTree2;        //roadside tree model #2
  std::unique_ptr<olc::Sprite> sprTree3;        //roadside tree model #3

  std::unique_ptr<olc::Sprite> sprLeftTurn;     //left turn roadside sign
  std::unique_ptr<olc::Sprite> sprRightTurn;    //right turn roadside sign

  //TODO: add more roadside element (buildings, etc...)

protected:
  
  void DrawMask() {
    for (int y = 0; y < SCREEN_H; y++) {
      for (int x = 0; x < SCREEN_W; x++) {
        if (sqrt((x - SCREEN_W / 2) * (x - SCREEN_W / 2) + (y - SCREEN_H / 2) * (y - SCREEN_H / 2)) > SCREEN_W / 2) {
          Draw(x, y, olc::BLACK);
        }
      }
    }
  }
  
  void DrawDebug() {
    // Draw Stats
    //auto text = "fAccumulatedTime: " + to_string(fAccumulatedTime);
    //auto text_size   = pixelFont24->GetTextSizeProp( text );
    // Compute the centre points so we can rotate about them
    //auto text_centre      = text_size / 2.0f;
    //auto fScale                 = 1.0f;
    //pixelFont24->DrawStringPropDecal( {0,static_cast<float>(text_centre.y)}, text, olc::MAGENTA, {fScale, fScale} );
  }

  // Called by olcConsoleGameEngine
  virtual bool OnUserCreate()
  { 
    //init logic
    game = Outrun();

    // Load the sprites
	  pixelFont24 = std::make_unique<olc::Font>( "./sprites/outrun/" + sTheme + "/font_24.png" );
    pixelFont48 = std::make_unique<olc::Font>( "./sprites/outrun/" + sTheme + "/font_48.png" );

    //TODO: generate 4 layer background
    //TODO: load car spritesheet
    //TODO: load roadside objects. start flag. etc...

    return true;
  }

  // Called by olcConsoleGameEngine
  virtual bool OnUserUpdate(float fElapsedTime)
  {  
    // Handle Input
    if (GetKey(olc::Key::D).bPressed)
      bShowDebug = !bShowDebug;
    
    if(GetKey(olc::Key::M).bPressed)
      bDrawMask = !bDrawMask;
    
    if(bShowDebug)
      DrawDebug(); // Draw debug info
    if(bDrawMask)
      DrawMask();   // Draw circular mask
    return true;
  }
};

int main()
{
  auto theme="default";
  OutrunRenderer renderer(theme);
  if (renderer.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
    renderer.Start();
  return 0;
}
