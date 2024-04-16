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

#define SEED 1962784967

class PNJ {
  private:
    olc::vi2d spriteSize = {0, 0};
    unique_ptr<olc::Sprite> spriteSheet;

    //handle animation looping. current frame index, start frame, end frame
    uint8_t curAnimationFrame = 0;
    uint8_t startAnimationFrame = 0;
    uint8_t endAnimationFrame = 0;
    uint8_t animationFps = 1;
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

    void setAnimation(uint8_t startIndex, uint8_t endIndex, float fps) {
      curAnimationFrame = startIndex;
      startAnimationFrame = startIndex;  
      endAnimationFrame = endIndex;
      animationFps = fps;
    }

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
      //float moveX = (rand() % 3) - 1;
      //float moveY = (rand() % 3) - 1;
      //setTarget(x+ moveX, y+moveY);
      //move(fElapsedTime*0.1f);
    }

    void setTarget(float dx, float dy) {
      targetX = dx;
      targetY = dy;
    }

    void move(float elapsedTime) {
      //TODO: debug/test
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
      STATE_WANDERING= 1,
      STATE_FOLLOWING = 2,
      STATE_FLEEING = 3,
      //STATE_RUNNING = 2,
      //STATE_FLYING = 3,
      //STATE_SWIMMING = 4,
    };

    enum DuckAnimations {
      ANIM_IDLE = 0,
      ANIM_WALK = 1,
      ANIM_RUN = 2,
      ANIM_FLY = 3,
      ANIM_SWIM = 4,
    };
   
    bool isLeader = false;
    bool isRunning = false; //to calc speed

    uint8_t state = STATE_IDLE;
    uint8_t type = MALE;

    Duck(float x, float y, string name, olc::Pixel color, bool leader=false) : PNJ(x,y,name){
      cout << "Duck constructor" << endl; 
      this->speed = 1.0f; //tile per second?
      this->color = color;
      this->state = STATE_IDLE;
      loadSpriteSheet("./sprites/duck.png", 16, 16);
    }
    ~Duck() {
      cout << "Duck destructor" << endl;
    }

    void setAnimation(uint8_t anim) {

      uint8_t startIndex = 0;
      uint8_t endIndex = 0;
      uint8_t fps = 1;

      switch(anim) {
        case ANIM_IDLE:
          startIndex = 0;
          endIndex = 3;
          fps=1;
          break;
        case ANIM_WALK:
          startIndex = 4;
          endIndex = 7;
          break;
        case ANIM_RUN:
          startIndex = 8;
          endIndex = 11;
          fps = 3;
          break;
        case ANIM_FLY:
          startIndex = 12;
          endIndex = 15;
          break;
        case ANIM_SWIM:
          startIndex = 16;
          endIndex = 19;
          break;
      }
      PNJ::setAnimation(startIndex, endIndex, fps);
    }

    void startIdling() {
      state = STATE_IDLE;
      setAnimation(ANIM_IDLE);
    }

    void startWandering() {
      state = STATE_WANDERING;
      setAnimation(ANIM_WALK);
    }

    void startFollowing(Duck * duckToFollow) {
      state = STATE_FOLLOWING;
      setAnimation(ANIM_WALK);
    }

    void startFleeing(olc::vf2d awayFrom) {
      state = STATE_FLEEING;
      //todo: calc flee vector
      setAnimation(ANIM_FLY);
    }

    //given other pnjs, player pos and tilemap 2d array, update duck logic
    void DuckLogic(Game* game) {

      //game contains receives player pos, tilemap content, and other pnjs
      
      //uint8_t map[TILES_X][TILES_Y];
      //Player player = Player();
      //vector<PNJ*> pnjs;

      //Duck's logic:
      //if player is near, run away
      //if player is far, walk around
      //if player is very far, idle
      //if water is near and no player around, swim

      //TODO: calc player distance
      //auto playerDist = sqrt((player->x - x)*(player->x - x) + (player->y - y)*(player->y - y));

      //TODO: calculate flee vector
      //TODO: calculate wander vector (random walk)
      //TODO: calculate nearest water tile
    }

};

class Player {
  public:
    float x = 0.0f;
    float y = 0.0f;
    float speed = 1.0f;
    bool hasMoved = false;
    olc::vi2d getPosI() {
      return {(int)x, (int)y};
    }
    olc::vf2d getPosF() {
      return {x, y};
    } 
    void move(float dx, float dy) {
      x += dx;
      y += dy;
      hasMoved = true;
    }
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

    uint8_t calcNoise(int x, int y, double seed) {
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
          map[x][y] =calcNoise(startX+x, startY+y, seed);
        }
      }
    }

    void randomize(float fElapsedTime) {
      for (auto y = 0u; y < TILES_Y; ++y) {
        for (auto x = 0u; x < TILES_X; ++x) {
          map[x][y] = calcNoise(x, y, fElapsedTime);
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
      } else if (noise > 78) {
        tile = TILE_GRASS2;
      } else if (noise > 76) {
        tile = TILE_GRASS3;
      } else if (noise > 60) {
        tile = TILE_WHEAT;
      }
      return tile;
    }
};

class Game {
  public:
    TileMap tileMap;
    Player player;
    vector<PNJ*> pnjs;

    Game() {
      player = Player();
      tileMap = TileMap();

       //create some PNJs
      addPNJ(new Duck(0.0f,0.0f,"Albert", olc::BLUE, true));
      addPNJ(new Duck(5.0f,5.0f,"Bernard", olc::GREEN));
      addPNJ(new Duck(12.0f,12.0f,"Charles", olc::YELLOW));
      addPNJ(new Duck(-15.0f,-15.0f,"Denis", olc::RED));
    }

    void addPNJ(PNJ* pnj) {
      pnjs.push_back(pnj);
    }

    void movePlayer(float dx, float dy) {
      player.move(dx, dy);
    }

    //world pos is the integer value of the player pos
    olc::vi2d getWorldPos() {
      return player.getPosI();
    } 

    olc::vf2d getPlayerPos() {
      return player.getPosF();
    }

    void generateTileMap() {
      tileMap.generate((int)player.x, (int)player.y, SEED);
    }

    void update(float fElapsedTime) {

      //update player
      //update pnjs
      //update tilemap

      //reset player moved flag !
      player.hasMoved = false;
    }
};

class GameRenderer : public olc::PixelGameEngine {
  public:
    GameRenderer() {
      sAppName = "Tiles";
    }

  private:
    Game game;

    float fTotalTime = 0.0f;
    bool bShowDebug = true;
    bool bShowPerlin = false;
    bool bUseDebugSprites = true;

    std::unique_ptr<olc::Sprite> sprTileSheet;

    olc::vi2d lastWorldPos = {-1, -1};
    olc::vi2d tileSize = {TILE_WIDTH, TILE_HEIGHT};

  protected:

    void RenderPNJs(float fElapsedTime) {
      for(auto pnj : game.pnjs) {


        //if pnj is a duck
        if(Duck* duck = static_cast<Duck*>(pnj)) {
          //duck->update(fElapsedTime);
        }

        //tile offset relative to player (screen center)
        auto tileOffsetX = (pnj->x - game.player.x) * TILE_WIDTH;
        auto tileOffsetY = (pnj->y - game.player.y) * TILE_HEIGHT;

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

          auto tileNoise = game.tileMap.getNoiseXY(x, y);
          auto tile = game.tileMap.toTile(tileNoise);

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
      
      //init game
      game = Game();

      // Load the sprites
      sprTileSheet = std::make_unique<olc::Sprite>("./sprites/testtiles/tileset16px.png");

      //Set random seed
      srand(SEED);

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

      if(GetKey(olc::Key::UP).bHeld) 
        game.movePlayer(0,-15.0f * fElapsedTime);  
      
      if(GetKey(olc::Key::DOWN).bHeld) 
        game.movePlayer(0, 15.0f * fElapsedTime);  

      if(GetKey(olc::Key::RIGHT).bHeld) 
        game.movePlayer(15.0f * fElapsedTime, 0);  
      
      if(GetKey(olc::Key::LEFT).bHeld) 
        game.movePlayer(-15.0f * fElapsedTime, 0); 

      //debug show player pos if player as moved
      if(game.player.hasMoved) {
        cout << "Player pos: " << game.player.x << ", " << game.player.y << endl;
      }
      
      //get world pos
      auto newWorldPos = game.getWorldPos();

      //if world pos  changed, update tile map
      if(lastWorldPos != newWorldPos) {
        sAppName = to_string(newWorldPos.x) + ", " + to_string(newWorldPos.y);
        cout <<  "New world pos: " << newWorldPos.x << ", " << newWorldPos.y <<  ", regenerating tile map" << endl;
        game.generateTileMap();
        lastWorldPos = newWorldPos;
      }
      
      //no need to clear screen if we draw all tiles ! (insert smart gif here)
      RenderTileMap();

      //render and update PNJs
      RenderPNJs(fElapsedTime);
      
      //update game logic
      game.update(fElapsedTime);
      
      return true;
    }
};

int main() {
  GameRenderer renderer2d = GameRenderer();
  if (renderer2d.Construct(SCREEN_W , SCREEN_H, SCREEN_PIXELSIZE, SCREEN_PIXELSIZE))
    renderer2d.Start();
  else {
    cout << "Failed to construct renderer" << endl;
    return 1;
  }
  return 0;
}
