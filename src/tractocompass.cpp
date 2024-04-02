
#define OLC_PGE_APPLICATION

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define SCREEN_W 240
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 1

#include <string>

class TestRenderer : public olc::PixelGameEngine
{
public:
  TestRenderer()
  {
    sAppName = "TractorTest";
  }

private:  

  int sprTractorFrameIndex=16;

  std::unique_ptr<olc::Font> pixelFont48;
  std::unique_ptr<olc::Sprite> sprTractor;
  olc::vi2d sprTractorPos = {ScreenWidth()/2 - 78,ScreenHeight()/2 -56}; 
  olc::vi2d sprTractorSize = {78,78}; 

  float fAccumulatedTime = 0.0f;
  float fTargetFrameTime = 100/1000.0f;

  bool bDrawMask = true;
  int nMaskRadius = SCREEN_W / 2;

  float fWorldX = 800.0f;
	float fWorldY = 800.0f;
	float fWorldA = 0.1f;
	float fNear = 0.005f;
	float fFar = 0.03f;
	float fFoVHalf = 3.14159f / 4.0f;

	std::unique_ptr<olc::Sprite> sprGround;
	std::unique_ptr<olc::Sprite> sprSky;
  std::unique_ptr<olc::Sprite> sprCompass;

  olc::vi2d sprCompassSize = {360,20};

	int nMapSize = 1600;

  
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

  virtual bool OnUserCreate()
  {
    sprGround = std::make_unique<olc::Sprite>("./sprites/map1.png");
    sprCompass = std::make_unique<olc::Sprite>("./sprites/testcompass1.png");
    sprTractor = std::make_unique<olc::Sprite>("./sprites/tractor32.png");
    pixelFont48 = std::make_unique<olc::Font>( "./sprites/default/font_48.png");
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {
    Clear(olc::YELLOW);

    //input to toggle mask
    if (GetKey(olc::Key::M).bPressed)
    {
      bDrawMask = !bDrawMask;
    }
    	// Control rendering parameters dynamically
		if (GetKey(olc::Key::Q).bHeld) fNear += 0.1f * fElapsedTime;
		if (GetKey(olc::Key::A).bHeld) fNear -= 0.1f * fElapsedTime;

		if (GetKey(olc::Key::S).bHeld) fFar -= 0.1f * fElapsedTime;

		if (GetKey(olc::Key::Z).bHeld) fFoVHalf += 0.1f * fElapsedTime;
		if (GetKey(olc::Key::X).bHeld) fFoVHalf -= 0.1f * fElapsedTime;

    fAccumulatedTime += fElapsedTime;

    // Create Frustum corner points
		float fFarX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fFar;
		float fFarY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fFar;

		float fNearX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fNear;
		float fNearY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fNear;

		float fFarX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fFar;
		float fFarY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fFar;

		float fNearX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fNear;
		float fNearY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fNear;

		// Starting with furthest away line and work towards the camera point
		for (int y = 0; y < ScreenHeight() / 2; y++)
		{
			// Take a sample point for depth linearly related to rows down screen
			float fSampleDepth = (float)y / ((float)ScreenHeight() / 2.0f);		

			// Use sample point in non-linear (1/x) way to enable perspective
			// and grab start and end points for lines across the screen
			float fStartX = (fFarX1 - fNearX1) / (fSampleDepth) + fNearX1;
			float fStartY = (fFarY1 - fNearY1) / (fSampleDepth) + fNearY1;
			float fEndX = (fFarX2 - fNearX2) / (fSampleDepth) + fNearX2;
			float fEndY = (fFarY2 - fNearY2) / (fSampleDepth) + fNearY2;

			// Linearly interpolate lines across the screen
			for (int x = 0; x < ScreenWidth(); x++)
			{
				float fSampleWidth = (float)x / (float)ScreenWidth();
				float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
				float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

				// Wrap sample coordinates to give "infinite" periodicity on maps
				fSampleX = fmod(fSampleX, 1.0f);
				fSampleY = fmod(fSampleY, 1.0f);

				// Sample symbol and colour from map sprite, and draw the
				// pixel to the screen
				auto pixel = sprGround->GetPixel(fSampleX*nMapSize, fSampleY*nMapSize);
				
				Draw(x, y + (ScreenHeight() / 2), pixel);		

				// Sample symbol and colour from sky sprite, we can use same
				// coordinates, but we need to draw the "inverted" y-location
				//sym = sprSky->SampleGlyph(fSampleX, fSampleY);
				//col = sprSky->SampleColour(fSampleX, fSampleY);
				//Draw(x, (ScreenHeight() / 2) - y, sym, col);
        // Handle user navigation with arrow keys
		
			}
		}

    fWorldA -= 1.0f * fElapsedTime;

    if (GetKey(olc::Key::LEFT).bHeld)
			fWorldA -= 1.0f * fElapsedTime;

		if (GetKey(olc::Key::RIGHT).bHeld)
			fWorldA += 1.0f * fElapsedTime;

		if (GetKey(olc::Key::UP).bHeld)
		{
			fWorldX += cosf(fWorldA) * 0.2f * fElapsedTime;
			fWorldY += sinf(fWorldA) * 0.2f * fElapsedTime;
		}

		if (GetKey(olc::Key::DOWN).bHeld)
		{
			fWorldX -= cosf(fWorldA) * 0.2f * fElapsedTime;
			fWorldY -= sinf(fWorldA) * 0.2f * fElapsedTime;
		}

    // Draw tractor sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    
    DrawPartialSprite(sprTractorPos,  sprTractor.get(), olc::vi2d(16,0)*sprTractorSize, sprTractorSize,2);
    DrawPartialSprite({sprTractorPos.x, 0},  sprTractor.get(), olc::vi2d(sprTractorFrameIndex,0)*sprTractorSize, sprTractorSize,.5);

    //DrawPartialSprite(sprTractorPos,  sprTractor.get(), olc::vi2d(sprTractorFrameIndex,0)*sprTractorSize, sprTractorSize,2);


    //map frame index (0 to 31) to a 360 compass direction
    auto fAngle = (180 + (static_cast<float>(sprTractorFrameIndex) / 32.0f)*360.0f) ;
    if(fAngle>360) fAngle-=360;


    // Draw compass sprite
    //compass sprite is 360 pixel wide. we need to offset it so the correct direction is shown in the middle of the screen.
    //north is at 0, so for a 0 angle sprite should be at halh screen width.
    
    //calculate offset:
    int nCompassOffset =(((static_cast<float>(sprTractorFrameIndex) / 32.0f)*360.0f)/360.0f)*sprCompassSize.x + ScreenWidth()/2 - sprCompassSize.x/2;

    //repeat left if sprite offset > 0
    if(nCompassOffset>0){
      //calculate new offset
      int nLeftCompassOffset = nCompassOffset-sprCompassSize.x;
      DrawSprite( {nLeftCompassOffset,ScreenHeight() - sprCompassSize.y-20},  sprCompass.get());
    }

    //draw compass sprite
    DrawSprite( {nCompassOffset,ScreenHeight() - sprCompassSize.y-20},  sprCompass.get());

    //repeat right if sprite offset + sprite width < screen width
    if(nCompassOffset+sprCompassSize.x<ScreenWidth()){
      //calculate new offset
      int nRightCompassOffset = nCompassOffset+sprCompassSize.x;
      DrawSprite( {nRightCompassOffset,ScreenHeight() - sprCompassSize.y-20},  sprCompass.get());
    }

    auto sAngleText = std::to_string( (int)fAngle);
  
    auto sAngleTextSize   = pixelFont48->GetTextSizeProp( sAngleText );
    
    // Compute the centre points so we can rotate about them
    auto text_centre      = sAngleTextSize / 2.0f;
    auto fScale                 = 1.0f;
    
    //pixelFont24->DrawStringPropDecal( {0,static_cast<float>(text_centre.y)}, text, olc::MAGENTA, {fScale, fScale} ); 

    //draw centered horizontaly
    pixelFont48->DrawStringPropDecal( {(float)(ScreenWidth()/2 - sAngleTextSize.x/2) +20,static_cast<float>(32)}, sAngleText, olc::BLACK, {fScale, fScale} );

    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

    if(bDrawMask){
      DrawMask();
    }

    // Update sprite index every 60ms
    //increment sprite index but loop when exceed 32
    if(fAccumulatedTime>fTargetFrameTime){
      fAccumulatedTime=0.0f;
      sprTractorFrameIndex++;
      if(sprTractorFrameIndex>31) sprTractorFrameIndex=0;
    }
    return true;
  }
};

int main() {
    TestRenderer renderer;
    if (renderer.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
      renderer.Start();
    return 0;
}

