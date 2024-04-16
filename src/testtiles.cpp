#include <cstdint>
#include <memory>
#include <sys/types.h>

#define OLC_PGE_APPLICATION

#include <string>
using namespace std;

#include "../lib/olcPixelGameEngine.h" 

#define DB_PERLIN_IMPL
#include "../lib/dbPerlin.hpp"


#define SCREEN_W 480
#define SCREEN_H 480
#define SCREEN_PIXELSIZE 1
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define TILES_X 30
#define TILES_Y 30
#define TILEMAP_WIDTH (TILES_X * TILE_WIDTH)
#define TILEMAP_HEIGHT (TILES_Y * TILE_HEIGHT)

enum Tiles { 
  TILE_NOTHING = 0, 

  TILE_DIRT = 1,
  TILE_EARTH = 2,

  TILE_GRASS1 = 3,
  TILE_GRASS2 = 4,
  TILE_GRASS3 = 5,
  TILE_GRASS4 = 6,

  TILE_WHEAT = 7,
  TILE_ROCK = 8,
};

class TileMap {
  public:

    //map data
    uint8_t map[TILES_X][TILES_Y];
 
    TileMap() {
      for (auto y = 0; y < TILES_Y; y++) {
        for (auto x = 0; x < TILES_X; x++) {
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

      //world 0,0 is center screen so we must generate offset by TILE_WIDTH/2, TILE_HEIGHT/2
      auto startX = worldx - TILES_X/2;
      auto startY = worldy - TILES_Y/2;

      for (auto y = 0u; y < TILES_Y; ++y) {
        for (auto x = 0u; x < TILES_X; ++x) {
          map[x][y] =getNoise(startX+x, startY+y, seed);
        }
      }
    }

    void randomize(float fElapsedTime) {
      for (auto y = 0u; y < TILES_Y; ++y) {
        for (auto x = 0u; x < TILES_X; ++x) {
          map[x][y] = getNoise(x, y, fElapsedTime);
        }
      }
    }

    uint8_t getTileXY(int x, int y) {
      return toTile(map[x][y]);
    }
    
    uint8_t getNoiseXY(int x, int y) {
      return map[x][y];
    }

    private:
      uint8_t toTile(uint8_t noise) {
        uint8_t tile = 0;
        if (noise > 200) {
          tile = TILE_NOTHING;
        } else if (noise > 160) {
          tile = TILE_EARTH;
        } else if (noise > 140) {
          tile = TILE_DIRT;
        } else if (noise > 80) {
          tile = TILE_GRASS1;
        }  
        else if (noise > 78) {
          tile = TILE_GRASS2;
        }
        else if (noise > 76) {
          tile = TILE_GRASS3;
        }
        else if (noise > 60) {
          tile = TILE_WHEAT;
        }
        return tile;
      }
};

class TileMapRenderer : public olc::PixelGameEngine
{
public:
  TileMapRenderer()
  {
    sAppName = "Tiles";
  }

private:
  float fTotalTime = 0.0f;
  bool bShowDebug = false;
  bool bShowPerlin = false;
  bool bDrawMask = false;
  bool bUseDebugSprites = false;

  TileMap* tileMap;
  std::unique_ptr<olc::Sprite> sprTileSheet;
  std::unique_ptr<olc::Sprite> sprTileMap;

  olc::vi2d tileSize = {TILE_WIDTH, TILE_HEIGHT};

  olc::vf2d playerPos = {0.0f, 0.0f};

  //render 5 tiles each direction around player pos
  const uint8_t renderRadius = 5u; //(SCREEN_W / 2 ) / TILE_WIDTH ;

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

  void RenderTileMap() {
    //render tile map
    for (int y = 0; y < TILES_Y; y++) {
      for (int x = 0; x < TILES_X; x++) {

        auto tileNoise = tileMap->getNoiseXY(x, y);
        int tile = tileMap->getTileXY(x, y);

        if(!bShowPerlin && tile == TILE_NOTHING) 
          continue;

        olc::vi2d tileCoords = olc::vi2d(x, y)*tileSize;
        
        //this draw directly to screen*
        //DrawPartialSprite(tileCoords, sprTileSheet.get(), olc::vi2d(tile, 0)*tileSize, tileSize); 

        for (int ty = 0; ty < tileSize.y; ty++) {
          for (int tx = 0; tx < tileSize.x; tx++) {
            if(bShowPerlin)
              sprTileMap->SetPixel(x*tileSize.x+tx, y*tileSize.y+ty, olc::Pixel(tileNoise, tileNoise, tileNoise));
            else {
              olc::Pixel p = sprTileSheet->GetPixel((tile-1)*tileSize.x+tx, ty + (bUseDebugSprites?0:tileSize.y)); 
              sprTileMap->SetPixel(x*tileSize.x+tx, y*tileSize.y+ty, p);
            }
          }
        }  
      }
    }
  }

  virtual bool OnUserCreate()
  { 
    // Initialize the tile map
    tileMap = new TileMap();
    sprTileMap = std::make_unique<olc::Sprite>(TILES_X * TILE_WIDTH, TILES_Y * TILE_HEIGHT);
    tileMap->randomize(1230);
    // Load the sprites
    sprTileSheet = std::make_unique<olc::Sprite>("./sprites/testtiles/tileset16px.png");
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {  
    fTotalTime+=fElapsedTime;
    // Handle Input
    if (GetKey(olc::Key::D).bPressed){
      bShowDebug = !bShowDebug;
      bUseDebugSprites = !bUseDebugSprites;
    }
    if(GetKey(olc::Key::M).bPressed)
      bDrawMask = !bDrawMask;
    
    if(GetKey(olc::Key::P).bPressed)
      bShowPerlin = !bShowPerlin;

    if(GetKey(olc::Key::UP).bHeld) {
      playerPos.y -= 15.0f * fElapsedTime;  
    }
    if(GetKey(olc::Key::DOWN).bHeld) {
      playerPos.y += 15.0f * fElapsedTime;  
    }
    if(GetKey(olc::Key::RIGHT).bHeld) {
      playerPos.x += 15.0f * fElapsedTime;  
    }
    if(GetKey(olc::Key::LEFT).bHeld) {
      playerPos.x -= 15.0f * fElapsedTime;  
    }

    
    Clear(olc::BLACK);
    //tileMap->randomize(fTotalTime);
    tileMap->generate(playerPos.x, playerPos.y, 0);
    RenderTileMap();

  // Draw the tile map centered on the screen

    DrawSprite({(SCREEN_W/2) - (TILEMAP_WIDTH/2),(SCREEN_H/2) - (TILEMAP_HEIGHT/2)}, sprTileMap.get(), 1);
    if(bDrawMask) {
      DrawMask();
    }
    return true;
  }
};

int main()
{
  TileMapRenderer renderer2d = TileMapRenderer();
  if (renderer2d.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
    renderer2d.Start();
  return 0;
}
