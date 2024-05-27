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

//#define TILES_1PX 
//#define TILES_2PX
//#define TILES_4PX
#define TILES_8PX
//#define TILES_16PX

#ifdef TILES_1PX
  #define TILE_WIDTH 1
  #define TILE_HEIGHT 1
  #define TILE_SET "./sprites/testtiles/tileset1px.png"
#endif
#ifdef TILES_2PX
  #define TILE_WIDTH 2
  #define TILE_HEIGHT 2
  #define TILE_SET "./sprites/testtiles/tileset2px.png"
#endif
#ifdef TILES_4PX
  #define TILE_WIDTH 2
  #define TILE_HEIGHT 2
  #define TILE_SET "./sprites/testtiles/tileset4px.png"
#endif
#ifdef TILES_8PX
  #define TILE_WIDTH 8
  #define TILE_HEIGHT 8
  #define TILE_SET "./sprites/testtiles/tileset8px.png"
#endif
#ifdef TILES_16PX
  #define TILE_WIDTH 16
  #define TILE_HEIGHT 16
  #define TILE_SET "./sprites/testtiles/tileset16px.png"
#endif

#define TILES_X SCREEN_W/TILE_WIDTH
#define TILES_Y SCREEN_H/TILE_HEIGHT
#define TILEMAP_WIDTH (TILES_X * TILE_WIDTH)
#define TILEMAP_HEIGHT (TILES_Y * TILE_HEIGHT)

enum Tiles {
  TILE_NOTHING = 0, 
  TILE_WATER= 1,
  TILE_DIRT = 2,
  TILE_GRASS = 3,
  TILE_WHEAT = 4,
};

class TileMap {
  public:

    //map data
    uint8_t map[TILES_X][TILES_Y];
 
    TileMap() {
      for (auto y = 0; y < TILES_Y; y++) {
        for (auto x = 0; x < TILES_X; x++) {
          map[x][y] = TILE_WATER;
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
        
        //Default to nothing but whouldn't be seen if every value is mapped
        uint8_t tile = TILE_NOTHING;

        //Tiles rules:
        //Water can be only in grass
        //Wheat can be only in grass
        //Dirt can be only in grass

        //Distribution of tiles:
        //Water: 5%
        //Wheat: 20%
        //Dirt: 10%
        //Grass: 65%         

        if(noise < 65) {
          tile = TILE_WATER;
        } else if(noise < 67) {
          tile = TILE_DIRT;
        } else if(noise < 192) {
          tile = TILE_GRASS;
        } else {
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
  bool bShowDebug = true;
  bool bShowPerlin = false;
  bool bDrawMask = false;
  bool bUseDebugSprites = true;

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
    sprTileMap = std::make_unique<olc::Sprite>(TILEMAP_WIDTH, TILEMAP_HEIGHT);

    // Load the sprites
    sprTileSheet = std::make_unique<olc::Sprite>(TILE_SET);
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
