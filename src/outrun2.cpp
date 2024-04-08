
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
  
  float fPlayerProgression = 0.0f; // progression du joueur en nombre de segments (ex: 3.5 = il est au milieu du 3è segment)
  float fPlayerSpeed = 1.0f;
  double fAccumulatedTime = 0.0;

  Outrun()
  {
    initTrack();
  }

  void initTrack() {

    //create 4 flat straight sections
    for (int nSectionIndex = 0; nSectionIndex < 4; nSectionIndex++) {
      Section s;
      s.fCurvatureX = (nSectionIndex % 2) * 2 - 1.0;
      s.fCurvatureY = 0.0f;
      s.fType = 0;
      for (int j = 0; j < 6; j++) {
        Segment seg;
        seg.nIndex = nSectionIndex* 6 +j; 
        
        //todo: set height based on section type
        //seg.nHeight = 
        
        //add start flag ti 4th segment of first section
        if(nSectionIndex==0 && j==4) {
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

  void generateNextSection() {
    // TODO a faire correctement
    Section s;
    s.fCurvatureX = -sections[0].fCurvatureX;
    s.fCurvatureY = 0.0f;
    s.fType = 0;
    for (int j = 0; j < 6; j++) {
      Segment seg;
      seg.nIndex = 999; //sectionIndex* 6 +j; :shrug: 
      
      //todo: set height based on section type
      //seg.nHeight = 
      
      //add start flag ti 4th segment of first section
      //if(sectionIndex==0 && j==4) {
        //seg.sprite = 
        //TODO
      //}
      s.segments.push_back(seg);
    }
    sections.push_back(s);
  }

  void update(float fElapsedTime) {
    //Accumulate time
    fAccumulatedTime += fElapsedTime;

    //Move player forward
    fPlayerProgression += fElapsedTime * fPlayerSpeed;

    //Remove sections that are behind the player
    //TODO: remove only sections with no more visible sprites on screen
    //sprites are taller than the lines of the segment that are rendered
    if(sections.size() > 0 && fPlayerProgression > sections[0].segments.size()) {
      fPlayerProgression -= sections[0].segments.size();
      sections.erase(sections.begin());
    }

    //Generate new section if needed
    if(sections.size() < 5)
      generateNextSection();
  }

  bool bStarted = false;

private:

};

class Pseudo3dRenderer : public olc::PixelGameEngine
{
public:
  Pseudo3dRenderer(string theme="default")
  {
    sAppName = "Outrun3dView";
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
  

  void DrawRoad() {
    if(game.sections.size() == 0)
      return;
    //TODO
  }

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

  virtual bool OnUserUpdate(float fElapsedTime)
  {  
    // Handle Input
    if (GetKey(olc::Key::D).bPressed)
      bShowDebug = !bShowDebug;
    if(GetKey(olc::Key::M).bPressed)
      bDrawMask = !bDrawMask;
    
    game.update(fElapsedTime);
    Clear(olc::VERY_DARK_YELLOW);
    DrawRoad();
    if(bShowDebug)
      DrawDebug(); // Draw debug info
    if(bDrawMask)
      DrawMask();   // Draw circular mask
    return true;
  }
};

class TopViewRenderer : public olc::PixelGameEngine
{
public:
  TopViewRenderer()
  {
    sAppName = "OutrunTopView";
  }

private:  

  Outrun game;
  
  bool bShowDebug = false;
  bool bDrawMask = true;

  float fSectionLength = 8.0f;
  float fRoadWidth = 64.0f;
  float fCurvatureXcoeff = M_PI * 0.125f;

protected:
  /*
    rendu temporaire vu de dessus, sans perspective
  */
  void Draw2dRoad() {
    if(game.sections.size() == 0)
      return;

    float x = ScreenWidth() / 2.0f;
    float y = ScreenHeight();
    float angle = - M_PI * 0.5f;

    // calcule les transformations liées a la position du joueur sur la première section
    for(int i = 0 ; i < game.fPlayerProgression + 1 ; i++) {
      float fClampedPosition = clamp(game.fPlayerProgression - i, 0.0f, 1.0f);
      float dx = cos(angle);
      float dy = sin(angle);
      angle -= game.sections[0].fCurvatureX * fCurvatureXcoeff / game.sections[0].segments.size() * fClampedPosition;
      x -= dx * fSectionLength * fClampedPosition;
      y -= dy * fSectionLength * fClampedPosition;
    }

    // print les sections
    for(auto section : game.sections) {
      for(auto segment : section.segments) {
        float dx = cos(angle);
        float dy = sin(angle);

        float last_left_x = x + dy * fRoadWidth * 0.5;
        float last_right_x = x - dy * fRoadWidth * 0.5;
        float last_left_y = y - dx * fRoadWidth * 0.5;
        float last_right_y = y + dx * fRoadWidth * 0.5;

        angle += section.fCurvatureX * fCurvatureXcoeff / section.segments.size();
        dx = cos(angle);
        dy = sin(angle);

        x += dx * fSectionLength;
        y += dy * fSectionLength;

        float left_x = x + dy * fRoadWidth * 0.5;
        float right_x = x - dy * fRoadWidth * 0.5;
        float left_y = y - dx * fRoadWidth * 0.5;
        float right_y = y + dx * fRoadWidth * 0.5;

        DrawLine(left_x, left_y, right_x, right_y);
        DrawLine(last_left_x, last_left_y, left_x, left_y);
        DrawLine(last_right_x, last_right_y, right_x, right_y);
      }
    }
  }

  virtual bool OnUserCreate()
  { 
    //init logic
    game = Outrun();
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {  
    // Handle Input
    if (GetKey(olc::Key::D).bPressed)
      bShowDebug = !bShowDebug;
    if(GetKey(olc::Key::M).bPressed)
      bDrawMask = !bDrawMask;
    
    game.update(fElapsedTime);
    Clear(olc::BLACK);
    Draw2dRoad();
    return true;
  }
};

int main()
{
  auto theme="default";
  //Pseudo3dRenderer renderer3d = Pseudo3dRenderer(theme);
  TopViewRenderer renderer2d = TopViewRenderer(); 
  
  //if (renderer3d.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
  //  renderer3d.Start();
  if (renderer2d.Construct(SCREEN_W , SCREEN_H, 1, 1))
    renderer2d.Start();
  return 0;
}
