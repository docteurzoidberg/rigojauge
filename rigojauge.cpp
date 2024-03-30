#define OLC_PGE_APPLICATION

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

Last Updated: 10/07/2017
*/

#include <iostream>
#include <string>
using namespace std;

#include "olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "olcPGEX_Font.h"


class OneLoneCoder_FormulaOLC : public olc::PixelGameEngine
{
public:
  OneLoneCoder_FormulaOLC()
  {
    sAppName = "Rigojauge2000";
  }

private:

  float fDistance = 0.0f;			// Distance car has travelled around track
  float fCurvature = 0.0f;		// Current track curvature, lerped between track sections
  float fTrackCurvature = 0.0f;	// Accumulation of track curvature
  float fTrackDistance = 0.0f;	// Total distance of track

  float fCarPos = 0.0f;			// Current car position
  float fPlayerCurvature = 0.0f;			// Accumulation of player curvature
  float fSpeed = 1.0f;			// Current player speed

  vector<pair<float, float>> vecTrack; // Track sections, sharpness of bend, length of section

  list<float> listLapTimes;		// List of previous lap times
  float fCurrentLapTime;			// Current lap time
   

  std::unique_ptr<olc::Font> pixelFont;

  std::unique_ptr<olc::Sprite> sprTile;
  olc::vi2d sprTileSize = olc::vi2d( 40, 29 );


protected:

  // Called by olcConsoleGameEngine
  virtual bool OnUserCreate()
  { 

    // Load the sprite
	  sprTile = std::make_unique<olc::Sprite>("./gfx/car_color.png");
    pixelFont = std::make_unique<olc::Font>( "./gfx/Pixels.png" );

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

    // Calculate total track distance, so we can set lap times
    for (auto t : vecTrack)
      fTrackDistance += t.second;

    listLapTimes = { 0,0,0,0,0 };
    fCurrentLapTime = 0.0f;

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

    // Lap Timing and counting
    fCurrentLapTime += fElapsedTime;
    if (fDistance >= fTrackDistance)
    {
      fDistance -= fTrackDistance;
      listLapTimes.push_front(fCurrentLapTime);
      listLapTimes.pop_back();
      fCurrentLapTime = 0.0f;
    }
    
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

    // Draw Sky - light blue and dark blue
    for (int y = 0; y < ScreenHeight() / 2; y++) {
      for (int x = 0; x < ScreenWidth(); x++) {
        Draw(x, y, y< ScreenHeight() / 4 ? olc::BLUE: olc::DARK_BLUE);
      }
    }
      
    // Draw Scenery - our hills are a rectified sine wave, where the phase is adjusted by the
    // accumulated track curvature
    for (int x = 0; x < ScreenWidth(); x++)
    {
      int nHillHeight = (int)(fabs(sinf(x * 0.01f + fTrackCurvature) * 16.0f));
      for (int y = (ScreenHeight() / 2) - nHillHeight; y < ScreenHeight() / 2; y++) {
        Draw(x, y, olc::YELLOW);
      }
    }


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
        olc::Pixel nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + fDistance * 0.1f) > 0.0f ? olc::GREEN : olc::DARK_GREEN;
        olc::Pixel nClipColour = sinf(80.0f *  powf(1.0f - fPerspective, 2) + fDistance) > 0.0f ? olc::RED : olc::WHITE;
        
        // Start finish straight changes the road colour to inform the player lap is reset
        olc::Pixel nRoadColour = (nTrackSection-1) == 0 ? olc::WHITE : olc::GREY;

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

    // Draw a car that represents what the player is doing
  
    //estimate direction based on current curvature
    
    nCarDirection = 0;
    if (fCurvature < -0.1f) nCarDirection = -1;
    if (fCurvature > +0.1f) nCarDirection = 1;
    
    olc::vi2d vSpritePos = olc::vi2d(0,0);
    switch (nCarDirection)
    {
      //left
      case -1:
        vSpritePos = olc::vi2d(7,0);
      break;
      //front
      case 0:
        vSpritePos = olc::vi2d(4,0);
      break;
      //right
      case 1:
        vSpritePos = olc::vi2d(1,0);
      break;
    }

    //Draw the car sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    DrawPartialSprite(olc::vi2d(nCarPos, 240 - 58 - 8), sprTile.get(), vSpritePos * sprTileSize, sprTileSize, 2);
    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
    


    // Draw Stats
   
    auto text = "fCurvature: " + to_string(fCurvature);

    auto text_size   = pixelFont->GetTextSizeProp( text );
    
    // Compute the centre points so we can rotate about them
    auto text_centre      = text_size / 2.0f;
    auto fScale                 = 1.0f;
    
    pixelFont->DrawRotatedStringPropDecal( {0,static_cast<float>(text_centre.y)}, text, 0, text_centre, olc::MAGENTA, {fScale, fScale} );
        
    //DrawString(0, 0, L"Distance: " + to_wstring(fDistance));
    //DrawString(0, 1, L"Target Curvature: " + to_wstring(fCurvature));
    //DrawString(0, 2, L"Player Curvature: " + to_wstring(fPlayerCurvature));
    //DrawString(0, 3, L"Player Speed    : " + to_wstring(fSpeed));
    //DrawString(0, 4, L"Track Curvature : " + to_wstring(fTrackCurvature));

    auto disp_time = [](float t) // Little lambda to turn floating point seconds into minutes:seconds:millis string
    {
      int nMinutes = t / 60.0f;
      int nSeconds = t - (nMinutes * 60.0f);
      int nMilliSeconds = (t - (float)nSeconds) * 1000.0f;
      return to_wstring(nMinutes) + L"." + to_wstring(nSeconds) + L":" + to_wstring(nMilliSeconds);
    };

    // Display current laptime
    //DrawString(10, 8, disp_time(fCurrentLapTime));

    // Display last 5 lap times
    //int j = 10;
    //for (auto l : listLapTimes)
    //{
    //	DrawString(10, j, disp_time(l));
    //	j++;
    //}

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
                Draw(x, y, olc::Pixel(0, 0, 0));
            }
        }
    }
    return true;
  }
};

int main()
{
  OneLoneCoder_FormulaOLC game;
  if (game.Construct(240, 240, 1, 1))
    game.Start();

  return 0;
}
