//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _LAZERS_H_
#define _LAZERS_H_


#define SCALE (3)

#include <SDL/SDL.h>

#ifdef SCALE
#include <SDL_image/SDL_image.h>
#else
#include <SDL_image.h>
#endif

typedef SDL_Color color;

#include <list>

#include "pieces.h"




class Tile
{
  private:
    Piece *piece_;
  
  public:  
    u8 colors[8];
    

    u8 x, y;
    
    Tile() : piece_{nullptr}, colors{COLOR_NONE}, x{0}, y{0} { }
    
    void resetLasers()
    {
      for (int i = 0; i < 8; ++i)
        colors[i] = COLOR_NONE;
    }
    
    void place(Piece* piece) { this->piece_ = piece; }
    Piece *piece() { return piece_; }
};
  

  
  
  
class Game;
  
class Gfx
{
  private:
    static SDL_PixelFormat *format;
    static const u16 WIDTH = 320;
    static const u16 HEIGHT = 240;
      
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
  private:
    Tile *tiles;
    Tile *inventory;
    std::list<Laser> lasers;
  
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
      
      
      for (int i = 0; i < 7; ++i)
      {
        tileAt(i+3,12)->place(new LaserSource(NORTH, static_cast<LaserColor>(i+1), this));
        
        if (i < 6)
          tileAt(i+3,13)->place(new Filter(static_cast<LaserColor>(i+1), this));

      }
      
      tileAt(2,14)->place(new Polarizer(NORTH, COLOR_MAGENTA, this));
      tileAt(1,14)->place(new Polarizer(NORTH, COLOR_WHITE, this));
      tileAt(3,14)->place(new Tunnel(NORTH, this));

      tileAt(3,3)->place(new Mirror(NORTH, this));
      tileAt(4,3)->place(new DoubleMirror(NORTH, this));


      tileAt(2,2)->place(new Splitter(NORTH, this));
      tileAt(3,2)->place(new DSplitter(NORTH, this));
      tileAt(4,2)->place(new Wall(this));
      tileAt(5,2)->place(new Wall(this));
      tileAt(6,2)->place(new Prism(NORTH, this));
      tileAt(7,2)->place(new Bender(this));

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
  
    void generateBeam(Position position, Direction direction, LaserColor color);
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
