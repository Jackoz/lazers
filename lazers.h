//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _LAZERS_H_
#define _LAZERS_H_

#include <SDL/SDL.h>
#include <SDL_image/SDL_image.h>

#include <list>

#define SCALE (3)



#define PIXEL(s, w, x, y) s[(y)*(w) + x]

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

typedef signed char s8;
typedef signed short s16;

typedef SDL_Color color;



static const u16 FIELD_WIDTH = 15;
static const u16 FIELD_HEIGHT = 15;

static const u16 INVENTORY_WIDTH = 6;
static const u16 INVENTORY_HEIGHT = 4;


enum LaserColor : u8
{
  COLOR_NONE = 0x00,
  COLOR_RED = 0x01,
  COLOR_GREEN = 0x02,
  COLOR_BLUE = 0x04,
  
  COLOR_YELLOW = 0x03,
  COLOR_MAGENTA = 0x05,
  COLOR_CYAN = 0x06,
  
  COLOR_WHITE = 0x07
};


enum PieceType : u8
{
  PIECE_AIR,
  PIECE_WALL,
  PIECE_SOURCE,
  PIECE_TARGET,
  PIECE_MIRROR
};
  
enum Direction : u8
{
  NORTH = 0,
  NORTH_EAST,
  EAST,
  SOUTH_EAST,
  SOUTH,
  SOUTH_WEST,
  WEST,
  NORTH_WEST
};
  
struct Position
{
  s8 x, y;
  
  Position(u8 x, u8 y) : x(x), y(y) { }
  
};
  
struct Laser
{
  Position position;
  Direction direction;
  LaserColor color;
  
  Laser(Position position, Direction direction, LaserColor color) : position(position), direction(direction), color(color) { }
  
  bool isValid() { return position.x >= 0 && position.y >= 0 && position.x < FIELD_WIDTH && position.y < FIELD_HEIGHT; }
  void invalidate() { position.x = -1; position.y = -1; }
  
  void rotateLeft(u8 steps)
  {
    u8 delta = direction - steps;
    if (delta < 0) delta += 8;
    direction = static_cast<Direction>(delta);
  }
  
  void rotateRight(u8 steps)
  {
    u8 delta = direction + steps;
    delta %= 8;
    direction = static_cast<Direction>(delta);
  }
  
  void flip() { rotateLeft(4); }
};


class Piece
{
  protected:
    PieceType type_;
    Direction rotation_;
    LaserColor color_;
  
  public:
    Piece(PieceType type, Direction rotation, LaserColor color) : type_(type), rotation_(rotation), color_(color) { }
  
    Direction rotation() { return rotation_; }
    PieceType type() { return type_; }
    LaserColor color() { return color_; }
  
    void rotateLeft() { rotation_ = rotation_ == NORTH ? NORTH_WEST : static_cast<Direction>(rotation_-1); }
    void rotateRight() { rotation_ = rotation_ == NORTH_WEST ? NORTH : static_cast<Direction>(rotation_+1); }
  
    virtual bool produceLaser() = 0;
    virtual bool blocksLaser(Laser &laser) = 0;
    virtual void receiveLaser(Laser &laser, std::list<Laser> &lasers) = 0;
  
    virtual u8 gfxRow() = 0;
  
    s8 deltaDirection(Laser& laser)
    {
      s8 delta = laser.direction - rotation_ + 4;
      
      if (delta > 4) delta -= 8;
      else if (delta < -4) delta += 8;
      else if (delta == -4) delta = 4;
      
      return delta;
    }

};

class Wall : public Piece
{
  public:
    Wall(Direction rotation, LaserColor color) : Piece(PIECE_WALL, rotation, color) { }

    bool produceLaser() override { return false; }
    bool blocksLaser(Laser &laser) override { return true; }
    void receiveLaser(Laser &laser, std::list<Laser> &lasers) override { }
  
    u8 gfxRow() override { return 1; }
};

class LaserSource : public Piece
{
  public:
    LaserSource(Direction rotation, LaserColor color) : Piece(PIECE_SOURCE, rotation, color) { }
    
    bool produceLaser() override { return true; }
    bool blocksLaser(Laser &laser) override { return true; }
    void receiveLaser(Laser &laser, std::list<Laser> &lasers) override { /*laser.invalidate();*/ }
  
    u8 gfxRow() override { return 1; }
};

class Mirror : public Piece
{
  public:
    Mirror(Direction rotation, LaserColor color) : Piece(PIECE_MIRROR, rotation, color) { }
    
    bool produceLaser() override { return false; }
    bool blocksLaser(Laser &laser) override { return false; }
    void receiveLaser(Laser &laser, std::list<Laser> &lasers) override
    {
      s8 delta = deltaDirection(laser);
      
      // opposite
      if (delta == 0)
        laser.flip();
      else if (delta == -1)
        laser.rotateLeft(2);
      else if (delta == 1)
        laser.rotateRight(2);
      else
        laser.invalidate();
    }
  
    u8 gfxRow() override { return 0; }
};


struct Tile
{
  u8 colors[8];
  Piece *piece;

  u8 x, y;
  
  Tile() : colors{COLOR_NONE}, piece{nullptr}, x{0}, y{0} { }
  
  void resetLasers()
  {
    for (int i = 0; i < 8; ++i)
      colors[i] = COLOR_NONE;
  }

};
  

  
  
  
class Game;
  
class Gfx
{
  private:
    static SDL_PixelFormat *format;
    static const u16 WIDTH = 320;
    static const u16 HEIGHT = 240;
  
    static const u16 TILE_SIZE = 14;
    static const u16 PIECE_SIZE = 12;
  
    static const s16 GFX_PIECE_ROWS[];
    static const s16 GFX_LASER_ROWS[];
    static const u16 GFX_FIELD_POS_X, GFX_FIELD_POS_Y, GFX_INVENTORY_POS_X;
  
    static u16 coordX(u16 x, bool isInventory) {
      if (!isInventory || x < FIELD_WIDTH)
        return TILE_SIZE*x + GFX_FIELD_POS_X;
      else
        return TILE_SIZE*(x-FIELD_WIDTH) + GFX_INVENTORY_POS_X;
    }
    static u16 coordY(u16 y) { return TILE_SIZE*y + GFX_FIELD_POS_Y; }
  
  public:
    static void line(u16 x1, u16 y1, u16 x2, u16 y2, u32 color);
    static void rect(u16 x1, u16 y1, u16 w, u16 h, u32 color);
    static void rectFill(s16 x1, s16 y1, u16 x2, u16 y2, u32 color);
    static void clear(u32 color);
    static inline void lock() { SDL_LockSurface(screen); }
    static inline void unlock() { SDL_UnlockSurface(screen); }
    static void blit(SDL_Surface *src, SDL_Surface *dst, u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h, u16 f);

  
    static void setFormat(SDL_PixelFormat *format) { Gfx::format = format; }
    static void init();
    static void render(Game *game);
  
    static void drawField(Game *game);
  
    //static inline color makeColor(u8 r, u8 g, u8 b) { return (color){r,g,b,0}; }
  
    static inline u32 ccc(u8 r, u8 g, u8 b)
    {
      return SDL_MapRGB(format, r, g, b);
    }
  
    static inline u32 makeColor(color color)
    {
      return SDL_MapRGB(format, color.r, color.g, color.b);
      //return (r << format->Rshift) | (g << format->Gshift) | (b << format->Bshift);
    }
  
  static SDL_Surface *tiles;
  
  #ifdef SCALE
    static SDL_Surface *realScreen;
  #endif
  static SDL_Surface *screen;
  
};
  
  
class Field
{
  static const s8 directions[8][2];
    
  private:
    Tile *tiles;
    Tile *inventory;
  
  public:
    Field() : tiles(new Tile[FIELD_WIDTH*FIELD_HEIGHT]), inventory(new Tile[INVENTORY_WIDTH*INVENTORY_HEIGHT])
    {
      for (int i = 0; i < FIELD_WIDTH; ++i)
        for (int j = 0; j < FIELD_HEIGHT; ++j)
        {
          Tile *tile = tileAt(i,j);
          tile->x = i;
          tile->y = j;
        }
      
      for (int i = 0; i < INVENTORY_WIDTH; ++i)
        for (int j = 0; j < INVENTORY_HEIGHT; ++j)
        {
          Tile *tile = tileAt(FIELD_WIDTH+i,j);
          tile->x = FIELD_WIDTH+i;
          tile->y = j;
        }
      
      tileAt(3,3)->piece = new Mirror(NORTH, COLOR_NONE);
      tileAt(5,5)->piece = new LaserSource(NORTH, COLOR_RED);
      tileAt(7,7)->piece = new LaserSource(NORTH, COLOR_GREEN);
      updateLasers();
    }
  
    Tile* tileAt(int x, int y)
    {
      if (x >= FIELD_WIDTH)
        return &inventory[y*INVENTORY_WIDTH + (x-FIELD_WIDTH)];
      else
        return &tiles[y*FIELD_WIDTH + x];
    }
  
    Tile* tileAt(Position p) { return tileAt(p.x, p.y); }
  
    void updateLasers();
};
  
class Game
{
  private:
    bool running;
    Field field_;
    Tile *selectedTile_;

  public:
    Game() : running(true), selectedTile_(nullptr), fposition(Position(0,0)), iposition(Position(FIELD_WIDTH,0)), position(&fposition) { }
    void init();
    void loop();
    void handleEvents();
  
    Field *field() { return &field_; }
    Tile *selectedTile() { return selectedTile_; }
  
  	Position fposition, iposition;
  	Position *position;
};

#endif
