
#define OLC_PGE_APPLICATION

#include "../lib/olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "../lib/olcPGEX_Font.h"

#define DB_PERLIN_IMPL
#include "../lib/dbPerlin.hpp"

#define SCREEN_W 240
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 2

#include <string>


#define TILE_WIDTH 16
#define TILE_HEIGHT 16

enum Tiles { 
  TILE_NOTHING = 0, 
  TILE_DIRT = 1,
  TILE_GRASS = 2,
  TILE_SAND = 3,
  TILE_HIGHGRASS = 4,
  TILE_WHEAT = 5,
  TILE_BUSH = 6,
  TILE_TREE = 7,
  TILE_ROCK = 8
};

class TileMap {
  public:
    static const uint8_t tilesX = 30;
    static const uint8_t tilesY = 30;

    //map data
    uint8_t map[tilesX][tilesY];

    TileMap() {
      for (auto y = 0; y < tilesY; y++) {
        for (auto x = 0; x < tilesX; x++) {
          map[x][y] = TILE_NOTHING;
        }
      }
    }

    uint8_t getNoise(int x, int y, double seed) {
      auto const noise = (
          db::perlin((double)x / 64.0f, (double)y / 64.0f, .25f*seed) * 1.0 +
          db::perlin((double)x / 32.0f, (double)y / 32.0f, .75f*seed) * 0.5
      ) / 1.5;
      auto const scaled = (uint8_t)((noise * 0.5 + 0.5) * 255.0);
      return scaled;
    }

    void generate(int worldx, int worldy, double seed) {


      //word pos to tile coordinates:

      //world 0,0 is center screen so we must generate offset by TILE_WIDTH/2, TILE_HEIGHT/2
      auto startX = worldx - tilesX/2;
      auto startY = worldy - tilesY/2;

      for (auto y = 0u; y < tilesY; ++y) {
        for (auto x = 0u; x < tilesX; ++x) {
          //if(map[x][y] == TILE_NOTHING)
          map[x][y] = toTile( getNoise(startX+x, startY+y, seed));
        }
      }
    }

    void randomize(float fElapsedTime) {
      for (auto y = 0u; y < tilesY; ++y) {
        for (auto x = 0u; x < tilesX; ++x) {
          map[x][y] = toTile( getNoise(x, y, fElapsedTime));
        }
      }
    }

    uint8_t getTileXY(int x, int y) {
      return map[x][y];
    }

    private:
      uint8_t toTile(uint8_t noise) {
        uint8_t tile = 0;
        if (noise > 200) {
          tile = TILE_NOTHING;
        } else if (noise > 160) {
          tile = TILE_SAND;
        } else if (noise > 140) {
          tile = TILE_DIRT;
        } else if (noise > 80) {
          tile = TILE_GRASS;
        } else if (noise > 60) {
          tile = TILE_WHEAT;
        }
        return tile;
      }
};

class TestRenderer : public olc::PixelGameEngine
{
public:
  TestRenderer()
  {
    sAppName = "TractorTest";
  }

private:  

  int sprTractorFrameIndex=16;

  int turnDirection = 0;  //-1 = left, 1 = right, 0 = none

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

  std::unique_ptr<olc::Font> pixelFont48;
  
  std::unique_ptr<olc::Sprite> sprTractor;
  std::unique_ptr<olc::Sprite> sprCompass;
	//std::unique_ptr<olc::Sprite> sprGround;
  std::unique_ptr<olc::Sprite> sprSky;
  std::unique_ptr<olc::Sprite> sprSign;
  std::unique_ptr<olc::Sprite> sprSignDirections;

  olc::vi2d sprTractorPos = {(SCREEN_W/2) - 78,(SCREEN_H/2) -56}; 
  olc::vi2d sprTractorSize = {78,78}; 

  olc::vi2d sprCompassSize = {360,20};

  olc::vi2d sprSignSize = {130,72};
  olc::vi2d sprSignPos = olc::vi2d({(SCREEN_W/2) - sprSignSize.x/2,0});

  olc::vi2d signMiniTractorPos = sprSignPos + olc::vi2d({56,0});
  olc::vi2d signBackgroundPos = sprSignPos + olc::vi2d({2,24});  
  olc::vi2d signBackgroundSize = {126,47};
  olc::vi2d signDirectionSpriteSize = {47,17};
  olc::vi2d signDirectionTextPos = sprSignPos + olc::vi2d({11,30});
  olc::vi2d signDirectionAnglePos = sprSignPos + olc::vi2d({11,50});

	int nMapSize = 480;

  TileMap* tileMap;
  
  std::unique_ptr<olc::Sprite> sprTileSheet;
  std::unique_ptr<olc::Sprite> sprTileMap;

  bool bShowDebugTileSprite = false;

  olc::vi2d tileSize = {TILE_WIDTH, TILE_HEIGHT};

  void DrawSky() {

  }

  void RenderTileMap() {
    //render tile map
    for (int y = 0; y < tileMap->tilesY; y++) {
      for (int x = 0; x < tileMap->tilesX; x++) {
        int tile = tileMap->getTileXY(x, y);
        if(tile == TILE_NOTHING) continue;
        olc::vi2d tileCoords = olc::vi2d(x, y)*tileSize;
        
        //this draw directly to screen*
        //DrawPartialSprite(tileCoords, sprTileSheet.get(), olc::vi2d(tile, 0)*tileSize, tileSize); 
        
        //this draw tile's pixels to destination sprite
        for (int ty = 0; ty < tileSize.y; ty++) {
          for (int tx = 0; tx < tileSize.x; tx++) {
            olc::Pixel p = sprTileSheet->GetPixel((tile-1)*tileSize.x+tx, ty + (bShowDebugTileSprite?0:tileSize.y));
            sprTileMap->SetPixel(tileCoords.x+tx, tileCoords.y+ty, p);
          }
        }
      }
    }
  }

  void DrawRoad() {
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
				//auto pixel = sprGround->GetPixel(fSampleX*nMapSize, fSampleY*nMapSize);
				auto pixel = sprTileMap->GetPixel(fSampleX*nMapSize, fSampleY*nMapSize);
				
				Draw(x, y + (ScreenHeight() / 2), pixel);		

				// Sample symbol and colour from sky sprite, we can use same
				// coordinates, but we need to draw the "inverted" y-location
				//sym = sprSky->SampleGlyph(fSampleX, fSampleY);
				//col = sprSky->SampleColour(fSampleX, fSampleY);
				//Draw(x, (ScreenHeight() / 2) - y, sym, col);
        // Handle user navigation with arrow keys
		
			}
		}

  }

  void DrawSign(float fAngle) {
    
    olc::vi2d signDirectionSpriteIndex = {0,0};

    //choose direction spritesheet index based on angle
    if(fAngle<22.5){
      //sAngleText = "N"; 
      signDirectionSpriteIndex = {0,0};
    }else if(fAngle<67.5){
      //sAngleText = "NE";
      signDirectionSpriteIndex = {1,0};
    }else if(fAngle<112.5){
      //sAngleText = "E";
      signDirectionSpriteIndex = {2,0};
    }else if(fAngle<157.5){
      //sAngleText = "SE";
      signDirectionSpriteIndex = {3,0};
    }else if(fAngle<202.5){
      //sAngleText = "S";
      signDirectionSpriteIndex = {4,0};
    }else if(fAngle<247.5){
      //sAngleText = "SW";
      signDirectionSpriteIndex = {5,0};
    }else if(fAngle<292.5){
      //sAngleText = "W";
      signDirectionSpriteIndex = {6,0};
    }else if(fAngle<337.5){
      //sAngleText = "NW";
      signDirectionSpriteIndex = {7,0};
    }

    //fill sign background with yellow
    FillRect(signBackgroundPos, signBackgroundSize, olc::YELLOW);

    //draw sign sprite
    DrawSprite(sprSignPos,  sprSign.get(),1);

    //Tracteur qui tourne
    DrawPartialSprite(signMiniTractorPos,  sprTractor.get(), olc::vi2d(sprTractorFrameIndex,0)*sprTractorSize, sprTractorSize, 1);

    //draw direction text
    DrawPartialSprite(signDirectionTextPos,  sprSignDirections.get(), signDirectionSpriteIndex * signDirectionSpriteSize,signDirectionSpriteSize);

    //sAngleText = std::to_string( (int)fAngle) + sAngleText;
    auto sAngleText = std::to_string( (int)fAngle);
    auto sAngleTextSize   = pixelFont48->GetTextSizeProp( sAngleText );
    
    //draw angle text
    pixelFont48->DrawStringPropDecal( signDirectionAnglePos - olc::vi2d(0, sAngleTextSize.y/2), sAngleText, olc::BLACK);
  }

  void DrawCompass() {
    //calculate offset:
    int nCompassOffset =(((static_cast<float>(sprTractorFrameIndex) / 32.0f)*360.0f)/360.0f)*sprCompassSize.x*2 + ScreenWidth()/2 - sprCompassSize.x;

    //repeat left if sprite offset > 0
    if(nCompassOffset>0){
      //calculate new offset
      int nLeftCompassOffset = nCompassOffset-sprCompassSize.x*2;
      DrawSprite( {nLeftCompassOffset,ScreenHeight() - sprCompassSize.y*2 + 5},  sprCompass.get(),2);
    }

    //draw compass sprite
    DrawSprite( {nCompassOffset,ScreenHeight() - sprCompassSize.y*2+ 5},  sprCompass.get(),2);

    //repeat right if sprite offset + sprite width < screen width
    if(nCompassOffset+sprCompassSize.x*2<ScreenWidth()){
      //calculate new offset
      int nRightCompassOffset = nCompassOffset+sprCompassSize.x*2+ 5;
      DrawSprite( {nRightCompassOffset,ScreenHeight() - sprCompassSize.y},  sprCompass.get(),2);
    }

    //draw line over compass bar
    //DrawLine(olc::vi2d(0,ScreenHeight() - sprCompassSize.y*2),olc::vi2d(ScreenWidth(),ScreenHeight() - sprCompassSize.y*2), olc::BLACK);
    FillRect(olc::vi2d(0,ScreenHeight() - sprCompassSize.y*2 + 4), olc::vi2d(ScreenWidth(),2), olc::RED);
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

  virtual bool OnUserCreate()
  {
    tileMap = new TileMap();
    tileMap->randomize(0.0f);

    //sprGround = std::make_unique<olc::Sprite>("./sprites/tractocompass/map.png");
    sprTileSheet = std::make_unique<olc::Sprite>("./sprites/tractocompass/tileset16px.png");
    sprTileMap = std::make_unique<olc::Sprite>(tileMap->tilesX * TILE_WIDTH, tileMap->tilesY * TILE_HEIGHT);
    sprCompass = std::make_unique<olc::Sprite>("./sprites/tractocompass/compassbar.png");
    sprSign = std::make_unique<olc::Sprite>("./sprites/tractocompass/sign.png");
    sprSignDirections = std::make_unique<olc::Sprite>("./sprites/tractocompass/sign-directions.png");
    sprTractor = std::make_unique<olc::Sprite>("./sprites/tractocompass/tractor32.png");
    pixelFont48 = std::make_unique<olc::Font>( "./sprites/tractocompass/font_48.png");
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {
    fAccumulatedTime += fElapsedTime;

    Clear(olc::CYAN);

    //input to toggle mask
    if (GetKey(olc::Key::M).bPressed)
    {
      bDrawMask = !bDrawMask;
    }

    //input to toggle debug tile sprite
    if (GetKey(olc::Key::D).bPressed)
    {
      bShowDebugTileSprite = !bShowDebugTileSprite;
    }

    	// Control rendering parameters dynamically
		if (GetKey(olc::Key::Q).bHeld) fNear += 0.1f * fElapsedTime;
		if (GetKey(olc::Key::A).bHeld) fNear -= 0.1f * fElapsedTime;
		if (GetKey(olc::Key::S).bHeld) fFar -= 0.1f * fElapsedTime;
		if (GetKey(olc::Key::Z).bHeld) fFoVHalf += 0.1f * fElapsedTime;
		if (GetKey(olc::Key::X).bHeld) fFoVHalf -= 0.1f * fElapsedTime;

    if (GetKey(olc::Key::LEFT).bHeld) {
      fWorldA -= 1.0f * fElapsedTime;
      turnDirection = -1;
    } else if (GetKey(olc::Key::RIGHT).bHeld) {
      fWorldA += 1.0f * fElapsedTime;
      turnDirection = 1;
    } else {
      turnDirection = 0;
    }

    //for now keep moving forward
		//if (GetKey(olc::Key::UP).bHeld)
		//{
			fWorldX += cosf(fWorldA) * 0.2f * fElapsedTime;
			fWorldY += sinf(fWorldA) * 0.2f * fElapsedTime;
		//}
    
    //slow down
		if (GetKey(olc::Key::DOWN).bHeld)
		{
			fWorldX -= cosf(fWorldA) * 0.1f * fElapsedTime;
			fWorldY -= sinf(fWorldA) * 0.1f * fElapsedTime;
		}

    // Render the tile map
    //tileMap->randomize(fAccumulatedTime);
    tileMap->generate(fWorldX, fWorldY, 1234.56f);
    RenderTileMap();

    //Draw background map
    DrawRoad();
    
    // Draw tractor sprite
    SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
    
    //Tracteur "fixe"
    DrawPartialSprite(sprTractorPos,  sprTractor.get(), olc::vi2d(16 + turnDirection,0)*sprTractorSize, sprTractorSize,2);

    //map frame index (0 to 31) to a 360 compass direction
    auto fAngle = (180 + (static_cast<float>(sprTractorFrameIndex) / 32.0f)*360.0f) ;
    if(fAngle>360) fAngle-=360;

    //draw sign elements
    DrawSign(fAngle);

    //draw compass bar
    DrawCompass();

    SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

    //draw circle mask
    if(bDrawMask){
      DrawMask();
    }

    //make world rotate
    //fWorldA -= 0.5f * fElapsedTime;

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

