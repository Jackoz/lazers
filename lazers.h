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
  
    Piece *piece() { return piece_; }
    void place(Piece* piece) { this->piece_ = piece; if (piece) piece->place(this); }

};
  

  
  
  

  
  
class Field
{  
  private:
    Tile *tiles;
    Tile *inventory;
    std::list<Laser> lasers;
    std::list<Goal*> goals;
  
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
        place(i+3,12, new LaserSource(NORTH, static_cast<LaserColor>(i+1), this));
        
        if (i < 6)
          place(i+3,13, new Filter(static_cast<LaserColor>(i+1), this));
      }
      
      place(2,14, new Polarizer(NORTH, COLOR_MAGENTA, this));
      place(1, 14, new Polarizer(NORTH, COLOR_WHITE, this));
      place(3, 14, new Tunnel(NORTH, this));
      place(4, 14, new ColorShifter(NORTH, this));
      place(5, 14, new ColorInverter(NORTH, this));
      place(6, 14, new StrictGoal(COLOR_NONE, this));
      place(7, 14, new StrictGoal(COLOR_YELLOW, this));
      place(8, 14, new Teleporter(this));
      place(9, 14, new Teleporter(this));
      //tileAt(8,14)->place(new Gate(COLOR_NONE, this));
      //tileAt(9,14)->place(new PrimaryGate(COLOR_YELLOW, this));


      place(3, 3, new Mirror(NORTH, this));
      place(4, 3, new DoubleMirror(NORTH, this));


      place(2, 2, new Splitter(NORTH, this));
      place(3, 2, new DSplitter(NORTH, this));
      place(4, 2, new Wall(this));
      place(5, 2, new Glass(this));
      place(6, 2, new Prism(NORTH, this));
      place(7, 2, new Bender(this));
      place(8, 2, new Twister(this));
      place(9, 2, new RoundFilter(this));
      place(10, 2, new CrossColorInverter(NORTH, this));
      place(11, 2, new SkewMirror(NORTH, this));
      place(12, 2, new DoubleSkewMirror(NORTH, this));
      place(13, 2, new DoubleSplitterMirror(NORTH, this));
      place(14, 2, new StarSplitter(this));

      

      updateLasers();
    }
  
    void place(u8 x, u8 y, Piece *piece)
    {
      Tile *tile = tileAt(x,y);
      tile->place(piece);
    }
  
    Tile* tileAt(int x, int y)
    {
      if (x >= FIELD_WIDTH)
        return &inventory[y*INVENTORY_WIDTH + (x-FIELD_WIDTH)];
      else
        return &tiles[y*FIELD_WIDTH + x];
    }
  
    Tile* tileAt(Position p) { return tileAt(p.x, p.y); }
  
    void addGoal(Goal *goal) { goals.push_back(goal); }
  
    void generateBeam(Position position, Direction direction, LaserColor color);
    void updateLasers();
    bool checkForWin();
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
