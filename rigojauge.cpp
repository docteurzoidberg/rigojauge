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

#include "olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "olcPGEX_Font.h"

class RigoJauge : public olc::PixelGameEngine
{
public:
  RigoJauge()
  {
    sAppName = "Rigojauge2000";
  }

private:

  float fDistance = 0.0f;			    // Distance car has travelled around track
  float fCurvature = 0.0f;		    // Current track curvature, lerped between track sections
  float fTrackCurvature = 0.0f;	  // Accumulation of track curvature
  float fTrackDistance = 0.0f;	  // Total distance of track

  float fCarPos = 0.0f;			      // Current car position
  float fPlayerCurvature = 0.0f;	// Accumulation of player curvature
  float fSpeed = 1.0f;			      // Current player speed

  olc::Pixel COLOR_GRASS_DARK = olc::Pixel(15,143,137);  //dark green
  olc::Pixel COLOR_GRASS_LIGHT = olc::Pixel(51,181,136); //lighter green
  olc::Pixel COLOR_RUMBLE = olc::RED;
  olc::Pixel COLOR_ROAD = olc::Pixel(125,124,125);  //gray
  olc::Pixel COLOR_SKY = olc::Pixel(20,87,141); //

  vector<pair<float, float>> vecTrack; // Track sections, sharpness of bend, length of section

  std::unique_ptr<olc::Font> pixelFont;
  std::unique_ptr<olc::Sprite> sprCar;
  std::unique_ptr<olc::Sprite> sprPalmAnimation;
  std::unique_ptr<olc::Sprite> sprLandscapeRepeat;
  olc::vi2d sprCarSize = olc::vi2d( 40, 29 );
  olc::vi2d sprLandscapeSize = olc::vi2d( 585, 86 );

protected:

  // Called by olcConsoleGameEngine
  virtual bool OnUserCreate()
  { 
    // Load the sprites
	  sprCar = std::make_unique<olc::Sprite>("./sprites/car_color.png");
    sprPalmAnimation = std::make_unique<olc::Sprite>("./sprites/palm_animation1.png");
    sprLandscapeRepeat = std::make_unique<olc::Sprite>("./sprites/landscape_repeat2.png");
    pixelFont = std::make_unique<olc::Font>( "./sprites/pixels_font.png" );

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
    return true;
  }

  // Called by olcConsoleGameEngine
  virtual bool OnUserUpdate(float fElapsedTime)
  {
    // Handle control input
    int nCarDirection = 0;
    int nScreenWidth = ScreenWidth();
    int nScreenHeight = ScreenHeight();
    int nRadius = std::min(nScreenWidth, nScreenHeight) / 2;

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
      

    // Clamp Speed
    if (fSpeed < 0.0f)	fSpeed = 0.0f;
    if (fSpeed > 1.0f)	fSpeed = 1.0f;
    
    // Move car along track according to car speed
    fDistance += (70.0f * fSpeed) * fElapsedTime;
    
    // Get Point on track
    float fOffset = 0;
    int nTrackSection = 0;

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

    //Draw Sky 
    for (int y = 0; y < ScreenHeight() / 2; y++) {
      for (int x = 0; x < ScreenWidth(); x++) {
        Draw(x, y, COLOR_SKY);
      }
    }

    // Define the width of the sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency


    // Draw Scenery - using a repeating sprite for hills
 
    // Calculate the phase based on track curvature and x position
    int phase = (int) ((nScreenWidth-sprLandscapeSize.x)/2) * fCurvature;
 
    int spriteX = (nScreenWidth-sprLandscapeSize.x)/2 + phase;
    int spriteY = nScreenHeight/2 - sprLandscapeSize.y ;
    
    DrawPartialSprite(olc::vi2d(spriteX , spriteY), sprLandscapeRepeat.get(), olc::vi2d(0,0), sprLandscapeSize, 1);
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
    
    // Draw Track - Each row is split into grass, clip-board and track
    for (int y = 0; y < ScreenHeight() / 2; y++)
      for (int x = 0; x < ScreenWidth(); x++)
      {
        // Perspective is used to modify the width of the track row segments
        float fPerspective = (float)y / (ScreenHeight()/2.0f);
        float fRoadWidth = 0.1f + fPerspective * 0.8f; // Min 10% Max 90%
        float fClipWidth = fRoadWidth * 0.15f;
        fRoadWidth *= 0.5f;	// Halve it as track is symmetrical around center of track, but offset...

        // ...depending on where the middle point is, which is defined by the current
        // track curvature.
        float fMiddlePoint = 0.5f + fCurvature * powf((1.0f - fPerspective), 3);

        // Work out segment boundaries
        int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
        int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
        int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
        int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();
        
        int nRow = ScreenHeight() / 2 + y;

        // Using periodic oscillatory functions to give lines, where the phase is controlled
        // by the distance around the track. These take some fine tuning to give the right "feel"
        olc::Pixel nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + fDistance * 0.1f) > 0.0f ? COLOR_GRASS_LIGHT : COLOR_GRASS_DARK;
        olc::Pixel nClipColour = sinf(80.0f *  powf(1.0f - fPerspective, 2) + fDistance) > 0.0f ? olc::RED : olc::WHITE;
        
        // Start finish straight changes the road colour to inform the player lap is reset
        olc::Pixel nRoadColour = (nTrackSection-1) == 0 ? olc::WHITE : COLOR_ROAD;

        // Draw the row segments
        if (x >= 0 && x < nLeftGrass)
          Draw(x, nRow, nGrassColour);
        if (x >= nLeftGrass && x < nLeftClip)
          Draw(x, nRow, nClipColour);
        if (x >= nLeftClip && x < nRightClip)
          Draw(x, nRow, nRoadColour);
        if (x >= nRightClip && x < nRightGrass)
          Draw(x, nRow, nClipColour);
        if (x >= nRightGrass && x < ScreenWidth()) 
          Draw(x, nRow, nGrassColour);
      }

    // Draw Car - car position on road is proportional to difference between
    // current accumulated track curvature, and current accumulated player curvature
    // i.e. if they are similar, the car will be in the middle of the track
    //fCarPos = fPlayerCurvature - fTrackCurvature; 
    
    //DrZoid
    //TODO: augmenter et decrementer  le playercurvature dans les virage pour que la voiture bouge de gauche a droite
    //pour l'instant forcé a 0 -> reste centree et sur la courbure de la route
    fCarPos = 0.0f;
    fPlayerCurvature = fTrackCurvature;

    int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0) - 40; // Offset for sprite

 
    //Draw a car that represents what the player is doing

    //DrZoid: direction = current curvature
    
    nCarDirection = 0;
    if (fCurvature < -0.1f) nCarDirection = -1;
    if (fCurvature > +0.1f) nCarDirection = 1;

    //TODO: uphill/downhill
    bool bUpHill = false;
    bool bDownHill = false;
    
    olc::vi2d vSpritePos = olc::vi2d(0,0);
    switch (nCarDirection)
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

    //Draw the car sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    DrawPartialSprite(olc::vi2d(nCarPos, nScreenHeight - (sprCarSize.y*2) - 8), sprCar.get(), vSpritePos * sprCarSize, sprCarSize, 2);
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
    
    // Draw Stats
    auto text = "phase: " + to_string(phase);
    auto text_size   = pixelFont->GetTextSizeProp( text );
    
    // Compute the centre points so we can rotate about them
    auto text_centre      = text_size / 2.0f;
    auto fScale                 = 1.0f;
    
    //pixelFont->DrawRotatedStringPropDecal( {0,static_cast<float>(text_centre.y)}, text, 0, text_centre, olc::MAGENTA, {fScale, fScale} );
        
    // DrZoid: c'est pas moi c'est GPT
    // Draw circular mask
    for (int y = 0; y < nScreenHeight; y++)
    {
        for (int x = 0; x < nScreenWidth; x++)
        {
            int nX = x - nScreenWidth / 2;
            int nY = y - nScreenHeight / 2;
            if (nX * nX + nY * nY > nRadius * nRadius)
            {
                // Draw black outside the circular screen
                Draw(x, y, olc::BLACK);
            }
        }
    }
    return true;
  }
};

int main()
{
  RigoJauge game;
  if (game.Construct(240, 240, 1, 1))
    game.Start();

  return 0;
}
