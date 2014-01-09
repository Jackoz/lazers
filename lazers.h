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


typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;
typedef SDL_Color color;


enum LaserColor : u8
{
  COLOR_RED = 0x01,
  COLOR_GREEN = 0x02,
  COLOR_BLUE = 0x04,
  
  COLOR_YELLOW = 0x03,
  COLOR_MAGENTA = 0x05,
  COLOR_CYAN = 0x05,
  
  COLOR_WHITE = 0x07
};


enum PieceType : u8
{
  PIECE_WALL,
  PIECE_SOURCE,
  PIECE_TARGET,
  PIECE_MIRROR
};
  
enum Rotation : u8
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
  u8 x, y;
};

struct Piece
{
  PieceType type;
  Rotation rotation;
};
  
struct Tile
{
  LaserColor colors[8];
  Piece *piece;
};
  

  
  
  
class Game;
  
class Gfx
{
  private:
    static SDL_PixelFormat *format;
    static const u16 WIDTH = 320;
    static const u16 HEIGHT = 200;
  
    static const u16 TILE_SIZE = 12;
  
  public:
    static void line(u16 x1, u16 y1, u16 x2, u16 y2, u32 color);
    static void rect(u16 x1, u16 y1, u16 x2, u16 y2, u32 color);
    static void clear(u32 color);
    static inline void lock() { SDL_LockSurface(screen); }
    static inline void unlock() { SDL_UnlockSurface(screen); }

  
    static void setFormat(SDL_PixelFormat *format) { Gfx::format = format; }
    static void init();
    static void render(Game *game);
  
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
  static SDL_Surface *screen;
  
};
  
class Game
{
  private:
    bool running = true;

  public:
    void init();
    void loop();
    void handleEvents();
  
  
  static const u16 FIELD_WIDTH = 15;
  static const u16 FIELD_HEIGHT = 15;
  
  Position position = {0,0};
};

#endif
