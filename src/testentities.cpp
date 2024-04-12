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

class PNJ {
  private:
    olc::vi2d spriteSize = {0, 0};
    unique_ptr<olc::Sprite> spriteSheet;

    //handle animation looping. current frame index, start frame, end frame
    uint8_t curAnimationFrame = 0;
    uint8_t startAnimationFrame = 0;
    uint8_t endAnimationFrame = 0;
    bool animationLoop = false;
    float fTotalTime = 0.0f;

    float targetX=0.0f;
    float targetY=0.0f; 
  
  public:
    string name="Unamed"; 
    
    float x = 0.0f;
    float y = 0.0f;
    float speed = 1.0f; //tile per second?
    olc::Pixel color = olc::WHITE;

    PNJ(float x, float y, string name) {
      this->x = x;
      this->y = y;
      this->name = name;
      cout << "Animal constructor" << endl;
    }
    ~PNJ() {
      cout << "Animal destructor" << endl;
    }

    void setAnimation(uint8_t startIndex, uint8_t endIndex) {
      curAnimationFrame = 0;
      startAnimationFrame = startIndex;  
      endAnimationFrame = endIndex;
    }

    void loadSpriteSheet(string path, uint16_t w, uint16_t h) {
      spriteSheet = std::make_unique<olc::Sprite>(path);
      spriteSize = {w, h};
    }

    olc::vi2d getSpriteSize() {
      return spriteSize;
    }

    void setSpriteSize(uint16_t w, uint16_t h) {
      spriteSize = {w, h};
    }

    //auto getSpriteSheet() {
      //get sprites
    //  return spriteSheet;
    //}

    void update(float fElapsedTime) {
      
      //update animation
      if (curAnimationFrame < endAnimationFrame) {
        curAnimationFrame++;
      } else {
        if (animationLoop) {
          curAnimationFrame = startAnimationFrame;
        }
      }

      //move randomly for 0 to 3 tile
      float moveX = (rand() % 3) - 1;
      float moveY = (rand() % 3) - 1;
      setTarget(x+ moveX, y+moveY);
      move(fElapsedTime*0.1f);
    }

    void setTarget(float dx, float dy) {
      targetX = dx;
      targetY = dy;
    }

    void move(float elapsedTime) {
      //move torward target based on elapsed time and speed
      if(x>targetX)
        x -= targetX * speed * elapsedTime;
      else
        x += targetX * speed * elapsedTime;

      if(y>targetY)
        y -= targetY * speed * elapsedTime;
      else
        y += targetY * speed * elapsedTime;
    }
};

class Duck : public PNJ {
  public:

    enum DuckType {
      MALE = 0,
      FEMALE = 1,
      DUCKLING = 2,
    };

    enum DuckState {
      STATE_IDLE = 0,
      STATE_WALKING = 1,
      //STATE_RUNNING = 2,
      //STATE_FLYING = 3,
      //STATE_SWIMMING = 4,
    };

    uint8_t state = STATE_IDLE;
    uint8_t type = MALE;

    Duck(float x, float y, string name, olc::Pixel color) : PNJ(x,y,name){
      cout << "Duck constructor" << endl; 
      this->speed = 1.0f; //tile per second?
      this->color = color;
      this->state = STATE_IDLE;
      loadSpriteSheet("./sprites/duck.png", 16, 16);
    }
    ~Duck() {
      cout << "Duck destructor" << endl;
    }
};

class Player {
  public:
    float x = 0.0f;
    float y = 0.0f;
    float speed = 1.0f;
};

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
      //init empty map
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

TileMap tileMap;
Player player;

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
  bool bUseDebugSprites = true;

  std::unique_ptr<olc::Sprite> sprTileSheet;
  //std::unique_ptr<olc::Sprite> sprTileMap;

  olc::vi2d tileSize = {TILE_WIDTH, TILE_HEIGHT};

  //render 5 tiles each direction around player pos
  const uint8_t renderRadius = 5u; //(SCREEN_W / 2 ) / TILE_WIDTH ; 

  vector<PNJ*> pnjs;

protected:

  void RenderPNJs(float fElapsedTime) {
    for(auto pnj : pnjs) {


      //if pnj is a duck
      if(Duck* duck = static_cast<Duck*>(pnj)) {
        //duck->update(fElapsedTime);
      }

      //tile offset relative to player (screen center)
      auto tileOffsetX = (pnj->x - player.x) * TILE_WIDTH;
      auto tileOffsetY = (pnj->y - player.y) * TILE_HEIGHT;

      //tile position on screen is tileOffset - screen center to get at 0
      auto pnjScreenX = (SCREEN_W / 2) + tileOffsetX;
      auto pnjScreenY = (SCREEN_H / 2) + tileOffsetY;
      
      //fill color
      FillRect(pnjScreenX, pnjScreenY, TILE_WIDTH, TILE_HEIGHT, pnj->color);

      //draw red box around pnj
      DrawRect(pnjScreenX, pnjScreenY, TILE_WIDTH, TILE_HEIGHT, olc::RED);

      //TODO: draw pnj sprite
      //TODO: update pnj logic@
      pnj->update(fElapsedTime);
    }
  }

  void RenderTileMap() {
    //render tile map
    for (int y = 0; y < TILES_Y; y++) {
      for (int x = 0; x < TILES_X; x++) {

        auto tileNoise = tileMap.getNoiseXY(x, y);
        auto tile = tileMap.toTile(tileNoise);

        if(!bShowPerlin && tile == TILE_NOTHING) 
          continue;

        //tile position on screen is tileOffset - screen center to get at 0
        auto tileX = x * TILE_WIDTH;
        auto tileY = y * TILE_HEIGHT;
        auto tileCoords = olc::vi2d(tileX, tileY);
        
        //draw directly to screen*
        if(bShowPerlin)
          FillRect(tileCoords, tileSize, olc::Pixel(tileNoise, tileNoise, tileNoise));
        else
          DrawPartialSprite(tileCoords, sprTileSheet.get(), olc::vi2d(tile, (bUseDebugSprites?0:1))*tileSize, tileSize); 
        
        //Draw to a buffer sprite instead of screen
        //for (int ty = 0; ty < tileSize.y; ty++) {
          //for (int tx = 0; tx < tileSize.x; tx++) {
            //if(bShowPerlin)
              //sprTileMap->SetPixel(x*tileSize.x+tx, y*tileSize.y+ty, olc::Pixel(tileNoise, tileNoise, tileNoise));
            //else {
              //olc::Pixel p = sprTileSheet->GetPixel((tile-1)*tileSize.x+tx, ty + (bUseDebugSprites?0:tileSize.y)); 
              //sprTileMap->SetPixel(x*tileSize.x+tx, y*tileSize.y+ty, p);
            //}
          //}
        //}

      }
    }
  }

  virtual bool OnUserCreate() { 
    //sprTileMap = std::make_unique<olc::Sprite>(TILES_X * TILE_WIDTH, TILES_Y * TILE_HEIGHT);
    //tileMap.randomize(1230);
    // Load the sprites
    sprTileSheet = std::make_unique<olc::Sprite>("./sprites/testtiles/tileset16px.png");

    //create some PNJs
    pnjs.push_back(new Duck(0.0f,0.0f,"Albert", olc::BLUE));
    pnjs.push_back(new Duck(5.0f,5.0f,"Bernard", olc::GREEN));
    pnjs.push_back(new Duck(12.0f,12.0f,"Charles", olc::YELLOW));
    pnjs.push_back(new Duck(-15.0f,-15.0f,"Denis", olc::RED));

    return true;
  }

  virtual bool OnUserUpdate(float fElapsedTime) {  
    fTotalTime+=fElapsedTime;
    // Handle Input
    if (GetKey(olc::Key::D).bPressed){
      bShowDebug = !bShowDebug;
      bUseDebugSprites = !bUseDebugSprites;
    }

    if(GetKey(olc::Key::P).bPressed)
      bShowPerlin = !bShowPerlin;

    if(GetKey(olc::Key::UP).bHeld) {
      player.y -= 15.0f * fElapsedTime;  
      cout << "Player pos: " << player.x << ", " << player.y << endl;
    }
    if(GetKey(olc::Key::DOWN).bHeld) {
      player.y += 15.0f * fElapsedTime;  
      cout << "Player pos: " << player.x << ", " << player.y << endl;
    }
    if(GetKey(olc::Key::RIGHT).bHeld) {
      player.x += 15.0f * fElapsedTime;  
      cout << "Player pos: " << player.x << ", " << player.y << endl;
    }
    if(GetKey(olc::Key::LEFT).bHeld) {
      player.x -= 15.0f * fElapsedTime;  
      cout << "Player pos: " << player.x << ", " << player.y << endl;
    }

    
    Clear(olc::BLACK);
    //tileMap->randomize(fTotalTime);
    tileMap.generate(player.x, player.y, 0);

    RenderTileMap();

    //Draw the tile map centered on the screen
    //DrawSprite({(SCREEN_W/2) - (TILEMAP_WIDTH/2),(SCREEN_H/2) - (TILEMAP_HEIGHT/2)}, sprTileMap.get(), 1);
    
    RenderPNJs(fElapsedTime);
    return true;
  }
};

int main()
{
  player = Player();
  tileMap = TileMap();
  TileMapRenderer renderer2d = TileMapRenderer();
  if (renderer2d.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
    renderer2d.Start();
  return 0;
}
