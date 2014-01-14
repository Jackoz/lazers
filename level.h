//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _LAZERS_H_
#define _LAZERS_H_





#include <list>

#include <sstream>
#include <string>
#include "base64.h"

#include "pieces.h"
#include "files.h"


class Game;




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

      
      std::stringstream ss;
      for (int i = 0; i < FIELD_WIDTH; ++i)
        for (int j = 0; j < FIELD_HEIGHT; ++j)
          if (tileAt(i, j)->piece())
          {
            PieceSaveInfo info = Files::savePiece(tileAt(i, j)->piece());
            ss << info.data;
            
          }
      
      std::string res = ss.str();
      std::string encoded = base64_encode(reinterpret_cast<unsigned const char*>(res.c_str()), res.length());
      printf("%s\n",encoded.c_str());
      
      
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
  


#endif
