
#include <chrono>
#include <cstdint>
#include <memory>

#define OLC_PGE_APPLICATION

#include <string>
using namespace std;

#include "../lib/olcPixelGameEngine.h" 

#define DB_PERLIN_IMPL
#include "../lib/dbPerlin.hpp"


#define SCREEN_W 240
#define SCREEN_H 240
#define SCREEN_PIXELSIZE 2
#define TILE_WIDTH 8
#define TILE_HEIGHT 8

enum Tiles { 
  TILE_NOTHING = 0,
  TILE_GRASS = 1,
  TILE_SAND = 2,
  TILE_HIGHGRASS = 3,
  TILE_WHEAT = 4,
  TILE_BUSH = 5,
  TILE_TREE = 6,
  TILE_ROCK = 7
};

class TileMap {
  public:
    static const uint8_t tilesX = 30;
    static const uint8_t tilesY = 30;

    //map data
    uint8_t map[tilesX * tilesY];

    TileMap() {
      for (auto y = 0; y < tilesY; y++) {
        for (auto x = 0; x < tilesX; x++) {
          map[y * tilesY + x] = TILE_NOTHING;
        }
      }
    }

    void randomize(float fElapsedTime) {
      for (auto y = 0u; y < tilesY; ++y) {
        for (auto x = 0u; x < tilesX; ++x) {
          auto const noise = (
              db::perlin((float)x / 64.0f, (float)y / 64.0f, .25f*fElapsedTime) * 1.0 +
              db::perlin((float)x / 32.0f, (float)y / 32.0f, .75f*fElapsedTime) * 0.5
          ) / 1.5;

          auto const brightness = (uint8_t)((noise * 0.5 + 0.5) * 255.0);

          uint8_t tile = 0;
          if(brightness<100) {
            tile = TILE_ROCK;
          } else if(brightness<120) {
            tile = TILE_TREE;
          } else if(brightness<140) {
            tile = TILE_BUSH;
          } else if(brightness<160) {
            tile = TILE_WHEAT;
          } else if(brightness<180) {
            tile = TILE_HIGHGRASS;
          } else if(brightness<200) {
            tile = TILE_SAND;
          } else {
            tile = TILE_GRASS;
          }
          map[y * tilesY + x] = tile;
        }
      }
    }

    uint8_t getTileXY(int x, int y) {
      return map[y * tilesX + x];
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
  bool bDrawMask = false;
  TileMap* tileMap;

  std::unique_ptr<olc::Sprite> sprTileSheet;
  std::unique_ptr<olc::Sprite> sprTileMap;

  olc::vi2d tileSize = {TILE_WIDTH, TILE_HEIGHT};
  olc::vf2d playerPos = {0.0f, 0.0f};

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
            olc::Pixel p = sprTileSheet->GetPixel((tile-1)*tileSize.x+tx, ty);
            sprTileMap->SetPixel(tileCoords.x+tx, tileCoords.y+ty, p);
          }
        }
      }
    }
  }

  virtual bool OnUserCreate()
  { 
    // Initialize the tile map
    tileMap = new TileMap();
    sprTileMap = std::make_unique<olc::Sprite>(tileMap->tilesX * TILE_WIDTH, tileMap->tilesY * TILE_HEIGHT);
    //tileMap->randomize(0);
    // Load the sprites
    sprTileSheet = std::make_unique<olc::Sprite>("./sprites/testtiles/tileset.png");
    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime)
  {  
    fTotalTime+=fElapsedTime;
    // Handle Input
    if (GetKey(olc::Key::D).bPressed)
      bShowDebug = !bShowDebug;
    if(GetKey(olc::Key::M).bPressed)
      bDrawMask = !bDrawMask;
    
    Clear(olc::BLACK);
    tileMap->randomize(fTotalTime);
    RenderTileMap();
    DrawSprite({0,0}, sprTileMap.get(), 1);
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
