#include <memory>
#define OLC_PGE_APPLICATION

//DrZoid: ported from lonecoder's to olcpixelgameengine and customized a lot

/*
OneLoneCoder.com - Code-It-Yourself! Racing Game at the command prompt (quick and simple c++)
"Let's go, go, go!!!" - @Javidx9

License
~~~~~~~
Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details. 
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9

GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be 
educational, and perhaps to the oddly minded - a little bit of fun. 
Please hack this, change it and use it in any way you see fit. You acknowledge 
that I am not responsible for anything bad that happens as a result of 
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!


Background
~~~~~~~~~~
I'm a sim-racer when I'm not coding. Racing games are far more sophisticated than
they used to be. Frankly, retro racing games are a bit naff. But when done in the
command prompt they have a new level of craziness.

Controls
~~~~~~~~
Left Arrow/Right Arrow Steer, Up Arrow accelerates. There are no brakes!
Set the fastest lap times you can!

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/KkMZI5Jbf18
*/


#include <string>
using namespace std;

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 240
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

class RigoJauge
{
public:
  RigoJauge()
  {
    // Define track
    vecTrack.push_back(make_pair(0.0f, 10.0f));		// Short section for start/finish line
    vecTrack.push_back(make_pair(0.0f, 200.0f));
    vecTrack.push_back(make_pair(1.0f, 200.0f));
    vecTrack.push_back(make_pair(0.0f, 400.0f));
    vecTrack.push_back(make_pair(-1.0f, 100.0f));
    vecTrack.push_back(make_pair(0.0f, 200.0f));
    vecTrack.push_back(make_pair(-1.0f, 200.0f));
    vecTrack.push_back(make_pair(1.0f, 200.0f));
    vecTrack.push_back(make_pair(0.0f, 200.0f));
    vecTrack.push_back(make_pair(0.2f, 500.0f));
    vecTrack.push_back(make_pair(0.0f, 200.0f));
  }

  void start() {
    bStarted = true;
  }

  void update(float fElapsedTime) {

    if(bStarted) {
      if(fSpeed<1.0f)
        fSpeed += 0.1f * fElapsedTime;
    }

    // Clamp Speed
    if (fSpeed < 0.0f)	fSpeed = 0.0f;
    if (fSpeed > 1.0f)	fSpeed = 1.0f;

    // Move car along track according to car speed
    fDistance += (70.0f * fSpeed) * fElapsedTime;	   
    
    // Get Point on track
    float fOffset = 0;
    nTrackSection = 0;

    // Find position on track (could optimise)
    while (nTrackSection < vecTrack.size() && fOffset <= fDistance)
    {			
      fOffset += vecTrack[nTrackSection].second;
      nTrackSection++;
    }
    
    // Interpolate towards target track curvature
    float fTargetCurvature = vecTrack[nTrackSection - 1].first;
    float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;

    // Accumulate player curvature
    fCurvature += fTrackCurveDiff;

    // Clamp the player curvature to ensure it stays within bounds
    if (fPlayerCurvature < -1.0f) fPlayerCurvature = -1.0f;
    if (fPlayerCurvature > 1.0f) fPlayerCurvature = 1.0f;

    fTrackCurvature += (fCurvature) * fElapsedTime * fSpeed;

    // Calculate the difference between track curvature and car position
    float fCurvatureDiff = fTrackCurvature - fPlayerCurvature;
    
   // Adjust the player curvature based on the difference
    fPlayerCurvature += fCurvatureDiff * fElapsedTime * fSpeed;  

    nCarDirection = 0;
    if (fCurvature < -0.1f) nCarDirection = -1;
    if (fCurvature > +0.1f) nCarDirection = 1;

    //DRZOID: forced for now
    fCarPos = 0.0f;
    fPlayerCurvature = fTrackCurvature;
  }

  float fCarPos = 0.0f;			      // Current car position
  float fSpeed = 0.0f;		        // Current speed
  float fDistance= 0.0f;          // Distance car has travelled around track
  float fCurvature = 0.0f;		    // Current track curvature, lerped between track sections
  float fTrackCurvature = 0.0f;	  // Accumulation of track curvature
  float fPlayerCurvature = 0.0f;	// Accumulation of player curvature 
  int nTrackSection = 0;          // Current track section
  int nCarDirection = 0;          // Current car direction

  bool bStarted = false;

private:

 vector<pair<float, float>> vecTrack; // Track sections, sharpness of bend, length of section
  
};

class RigoJaugeRenderer : public olc::PixelGameEngine
{
public:
  RigoJaugeRenderer(string theme="default")
  {
    sAppName = "Rigojauge2000";
    sTheme = theme;
  }

private:  

  RigoJauge game;
  string sTheme;
  
  bool bShowDebug = false;
  bool bDrawMask = true;  
  bool bShowStartMessage = true;
  
  int nMaskRadius = std::min(SCREEN_W, SCREEN_H) / 2;
  
  //landscape phase & position
  int phase = 0;
  int landscapeX = 0;
  int landscapeY = 0;    
  
  float fTargetFrameTime = 1.0f / 60.0f; // Virtual FPS of 100fps
  float fAccumulatedTime = 0.0f;
  float fNextToggleStarMessage = 0.5f;

  //colors
  olc::Pixel COLOR_GRASS_DARK = olc::Pixel(15,143,137);  //dark green
  olc::Pixel COLOR_GRASS_LIGHT = olc::Pixel(51,181,136); //lighter green
  olc::Pixel COLOR_RUMBLE = olc::RED;
  olc::Pixel COLOR_ROAD = olc::Pixel(30,30,30); 
  olc::Pixel COLOR_SKY = olc::Pixel(20,87,141); //

  //sprites
  std::unique_ptr<olc::Font> pixelFont24;
  std::unique_ptr<olc::Font> pixelFont48;

  std::unique_ptr<olc::Sprite> sprCar;
  std::unique_ptr<olc::Sprite> sprPalmAnimation;
  std::unique_ptr<olc::Sprite> sprPalmStatic;
  std::unique_ptr<olc::Sprite> sprLandscapeRepeat;
  std::unique_ptr<olc::Sprite> sprLapBanner;

  //sprite sizes
  olc::vi2d sprCarSize = olc::vi2d( 40, 29 );
  olc::vi2d sprLandscapeSize = olc::vi2d( 585, 86 );
  olc::vi2d sprLapBannerSize = olc::vi2d( 180, 139 );
  olc::vi2d sprPalmStaticSize = olc::vi2d( 52, 37 );


protected:
  
  void DrawDebug() {
    if(!bShowDebug) return;
    // Draw Stats
    auto text = "fAccumulatedTime: " + to_string(fAccumulatedTime);
    auto text_size   = pixelFont24->GetTextSizeProp( text );
    
    // Compute the centre points so we can rotate about them
    auto text_centre      = text_size / 2.0f;
    auto fScale                 = 1.0f;
    
    pixelFont24->DrawStringPropDecal( {0,static_cast<float>(text_centre.y)}, text, olc::MAGENTA, {fScale, fScale} );
  }

  void DrawSky() {
    for (int y = 0; y < ScreenHeight() / 2; y++) {
      for (int x = 0; x < ScreenWidth(); x++) {
        Draw(x, y, COLOR_SKY);
      }
    }
  }

  void DrawLandscape() {
    // Calculate the phase based on track curvature and x position
    phase = (int) ((ScreenWidth()-sprLandscapeSize.x)/2) * game.fCurvature;
    landscapeX = (ScreenWidth()-sprLandscapeSize.x)/2 + phase;
    landscapeY = ScreenHeight()/2 - sprLandscapeSize.y ;
  
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    DrawPartialSprite(olc::vi2d(landscapeX , landscapeY), sprLandscapeRepeat.get(), olc::vi2d(0,0), sprLandscapeSize, 1);  
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
  }

  void DrawRoad() {
    
    // Draw Track - Each row is split into grass, clip-board and track
    for (int y = 0; y < ScreenHeight() / 2; y++) 
    {
      for (int x = 0; x < ScreenWidth(); x++)
      {
        // Perspective is used to modify the width of the track row segments
        float fPerspective = (float)y / (ScreenHeight()/2.0f);
        float fRoadWidth = 0.1f + fPerspective * 0.8f; // Min 10% Max 90%
        float fLaneWidth = fRoadWidth * 0.5f;
        float fClipWidth = fRoadWidth * 0.15f;
        fRoadWidth *= 0.5f;	// Halve it as track is symmetrical around center of track, but offset...

        // ...depending on where the middle point is, which is defined by the current
        // track curvature.
        float fMiddlePoint = 0.5f + game.fCurvature * powf((1.0f - fPerspective), 3);

        // Work out segment boundaries
        int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
        int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
        int nMidlaneLeft = (fMiddlePoint - fRoadWidth * .05) * ScreenWidth();
        int nMidlaneRight = (fMiddlePoint + fRoadWidth * .05) * ScreenWidth();
        int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
        int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();
        
        int nRow = ScreenHeight() / 2 + y;

        // Using periodic oscillatory functions to give lines, where the phase is controlled
        // by the distance around the track. These take some fine tuning to give the right "feel"
        olc::Pixel nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + game.fDistance * 0.1f) > 0.0f ? COLOR_GRASS_LIGHT : COLOR_GRASS_DARK;
        olc::Pixel nClipColour = sinf(80.0f *  powf(1.0f - fPerspective, 2) + game.fDistance) > 0.0f ? COLOR_RUMBLE : olc::WHITE;
        olc::Pixel nLaneColour = sinf(60.0f *  powf(1.0f - fPerspective, 2) + game.fDistance) > 0.0f ? COLOR_ROAD : olc::WHITE;
        
        // Start finish straight changes the road colour to inform the player lap is reset
        //olc::Pixel nRoadColour = (game.nTrackSection-1) == 0 ? olc::WHITE : COLOR_ROAD;
        olc::Pixel nRoadColour = COLOR_ROAD; // en dur

        if (x >= 0 && x < nLeftGrass)
          Draw(x, nRow, nGrassColour);
        if (x >= nLeftGrass && x < nLeftClip)
          Draw(x, nRow, nClipColour);
        if(x>=nLeftClip && x < nRightClip)
          Draw(x, nRow, nRoadColour);
        if (x >= nMidlaneLeft && x < nMidlaneRight) // changement ici
          Draw(x, nRow, nLaneColour); // changement ici
        if (x >= nRightClip && x < nRightGrass)
          Draw(x, nRow, nClipColour);
        if (x >= nRightGrass && x < ScreenWidth()) 
          Draw(x, nRow, nGrassColour);
      }
      
      //Draw the tree sprite (en dur partout)
      int posX = 0;
      int posY = y;
      SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
      DrawSprite({posX, posY}, sprPalmStatic.get(),1);
      SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
    }
  }

  void DrawCar() {
    
    // Draw Car
    int carSpriteX = ScreenWidth() / 2 + ((int)(ScreenWidth() * game.fCarPos) / 2.0) - 40;
    int carSpriteY = ScreenHeight() - (sprCarSize.y*2) - 8;
 

    //TODO: uphill/downhill
    bool bUpHill = false;
    bool bDownHill = false;
    
    olc::vi2d vSpritePos = olc::vi2d(0,0);
    switch (game.nCarDirection)
    {
      //left
      case -1:
        if(bDownHill) vSpritePos = olc::vi2d(8,0);
        else if(bUpHill) vSpritePos = olc::vi2d(6,0);
        else vSpritePos = olc::vi2d(7,0);
      break;
      //front
      case 0:
        if(bDownHill) vSpritePos = olc::vi2d(5,0);
        else if(bUpHill) vSpritePos = olc::vi2d(3,0);
        vSpritePos = olc::vi2d(4,0);
      break;
      //right
      case 1:
        if(bDownHill) vSpritePos = olc::vi2d(2,0);
        else if(bUpHill) vSpritePos = olc::vi2d(0,0);
        vSpritePos = olc::vi2d(1,0);
      break;
    }

    //Draw the car choosed sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    DrawPartialSprite(olc::vi2d(carSpriteX, carSpriteY), sprCar.get(), vSpritePos * sprCarSize, sprCarSize, 2);
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
  }

  void DrawStartMessage() {
   
    // Draw Start Message
    auto text = "PRESS START";
    auto text_size   = pixelFont48->GetTextSizeProp( text );
    
    // Compute the centre points so we can rotate about them
    auto text_centre      = text_size / 2.0f;
    auto fScale                 = 1.0f;
    
    //blink text every 3 seconds
    if(fAccumulatedTime > fNextToggleStarMessage) {
      if(bShowStartMessage) {
        fNextToggleStarMessage = fAccumulatedTime + 0.5;
        bShowStartMessage = false;
      } else {
        fNextToggleStarMessage = fAccumulatedTime + 0.5f;
        bShowStartMessage = true;
      }
    }

    if(bShowStartMessage)
      pixelFont48->DrawRotatedStringPropDecal( {static_cast<float>(ScreenWidth()/2),static_cast<float>(ScreenHeight()/2 -10)}, text, 0, text_centre, olc::WHITE, {fScale, fScale} );
    
    //pixelFont48->DrawRotatedStringPropDecal( {static_cast<float>(ScreenWidth()/2),static_cast<float>(ScreenHeight()/2)}, text, 0, text_centre, olc::MAGENTA, {fScale, fScale} );
  }

  void DrawLapBanner() {
    // Draw Lap Banner sprite
    olc::vi2d vSpritePos = olc::vi2d((ScreenWidth()-sprLapBannerSize.x)/2,ScreenHeight()-sprLapBannerSize.y-54);
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    DrawSprite(vSpritePos, sprLapBanner.get());
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
  }

  void DrawMask() {
    for (int y = 0; y < ScreenHeight(); y++)
    {
        for (int x = 0; x < ScreenWidth(); x++)
        {
            int nX = x - ScreenWidth() / 2;
            int nY = y - ScreenHeight() / 2;
            if (nX * nX + nY * nY > nMaskRadius * nMaskRadius)
            {
                // Draw black outside the circular screen
                Draw(x, y, olc::BLACK);
            }
        }
    }
  }

  // Called by olcConsoleGameEngine
  virtual bool OnUserCreate()
  { 
    //init logic
    game = RigoJauge();

    // Load the sprites
	  sprCar = std::make_unique<olc::Sprite>("./sprites/" + sTheme + "/car.png");
    sprPalmAnimation = std::make_unique<olc::Sprite>("./sprites/" + sTheme + "/palm_animation.png");
    sprPalmStatic = std::make_unique<olc::Sprite>("./sprites/" + sTheme + "/palm_static1.png");
    sprLandscapeRepeat = std::make_unique<olc::Sprite>("./sprites/" + sTheme + "/landscape.png");
    sprLapBanner = std::make_unique<olc::Sprite>("./sprites/" + sTheme + "/startfinish.png");
    pixelFont24 = std::make_unique<olc::Font>( "./sprites/" + sTheme + "/font_24.png" );
    pixelFont48 = std::make_unique<olc::Font>( "./sprites/" + sTheme + "/font_48.png" );

    //overwrite sprite sizes and colors
    if (sTheme=="1bit") {
      COLOR_GRASS_DARK = olc::Pixel(15,143,137);  //dark green
      COLOR_GRASS_LIGHT = olc::Pixel(51,181,136); //lighter green
      COLOR_RUMBLE = olc::RED;
      COLOR_ROAD = olc::Pixel(30,30,30);
      COLOR_SKY = olc::Pixel(20,87,141);
      sprCarSize = olc::vi2d( 40, 29 );
      sprLandscapeSize = olc::vi2d( 585, 86 );
      sprLapBannerSize = olc::vi2d( 180, 139 );
    }
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
    
    if (GetKey(olc::Key::SPACE).bPressed && !game.bStarted)
     game.start();

    // Accumulate elapsed time
    fAccumulatedTime += fElapsedTime;

    //  if (m_keys[VK_UP].bHeld)
    //		fSpeed += 2.0f * fElapsedTime;
    //	else
    //		fSpeed -= 1.0f * fElapsedTime;

    // Car Curvature is accumulated left/right input, but inversely proportional to speed
    // i.e. it is harder to turn at high speed
    //	if (m_keys[VK_LEFT].bHeld)
    //	{
    //		fPlayerCurvature -= 0.7f * fElapsedTime * (1.0f - fSpeed / 2.0f);
    //		nCarDirection = -1;
    //	}

    //if (m_keys[VK_RIGHT].bHeld)
    //{
    //	fPlayerCurvature += 0.7f * fElapsedTime * (1.0f - fSpeed / 2.0f);
    //	nCarDirection = +1;
    //}

    // If car curvature is too different to track curvature, slow down
    // as car has gone off track
    //if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.8f) {
      //fSpeed -= 5.0f * fElapsedTime; 
    //}

    // Update Game Logic
    game.update(fElapsedTime);
    DrawSky();
    DrawLandscape();
    DrawRoad();
    DrawCar();
    if(!game.bStarted) {
      DrawStartMessage();
      DrawLapBanner();
      //TODO: press start message
      //TODO: lap start flag sprite
      //TODO: side animations
    }
    DrawDebug();
    if(bDrawMask)
      DrawMask();   // Draw circular mask
    return true;
  }
};

int main()
{
  auto theme="default";
  RigoJaugeRenderer renderer(theme);
  if (renderer.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
    renderer.Start();
  return 0;
}
