//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _LAZERS_H_
#define _LAZERS_H_

#include <cstdlib>

#include <list>
#include <unordered_set>

#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include <array>

#include <cassert>

#include "pieces.h"
#include "files/files.h"

class Game;

class Tile
{
private:
  std::unique_ptr<Piece> _piece;

public:
  std::array<LaserColor, 8> colors;
  
  u8 x, y;
  u8 variant;
  
  Tile() : _piece{nullptr}, colors{LaserColor::NONE}, x{0}, y{0}, variant{static_cast<u8>(rand()%3)} { }
  
  void resetLasers() { std::fill(colors.begin(), colors.end(), LaserColor::NONE); }
  void clear() { _piece.reset(); }

  bool empty() const { return _piece == nullptr; }
  
  const std::unique_ptr<Piece>& piece() const { return _piece; }

  void place(Piece* piece)
  {
    if (piece)
      assert(!_piece);

    _piece.reset(piece);
  }

  void swap(std::unique_ptr<Piece>& other) { std::swap(_piece, other); }
  void swap(Tile* other) { std::swap(_piece, other->_piece); }
};

class Field
{  
private:
  u32 _width, _height, _invWidth, _invHeight;
  
  LevelSpec* _level;
  
  Tile *tiles;
  Tile *inventory;
  std::list<Laser> lasers;
  std::list<Goal*> goals;
  std::unordered_set<Laser, Laser::hash> beams;
  
  bool failed;

  void resetLasers();

public:
  Field(u32 width, u32 height, u32 invWidth, u32 invHeight) :
  _width(width), _height(height),
  _invWidth(invWidth), _invHeight(invHeight),
  _level(nullptr),
  tiles(new Tile[width*height]), inventory(new Tile[invWidth*invHeight]),
  failed(false)
  {
    for (u32 i = 0; i < _width; ++i)
      for (u32 j = 0; j < _height; ++j)
      {
        Tile& tile = tiles[j*_width + i];
        tile.x = i;
        tile.y = j;
      }
    
    for (u32 i = 0; i < _invWidth; ++i)
      for (u32 j = 0; j < _invHeight; ++j)
      {
        Tile& tile = inventory[j*_invWidth + i];
        tile.x = i;
        tile.y = j;
      }
    
    
    /*for (int i = 0; i < 7; ++i)
    {
      place({(s8)(i+3),8}, new LaserSource(NORTH, static_cast<LaserColor>(i+1)));
      
      if (i < 6)
        place({(s8)(i+3),9}, new Filter(static_cast<LaserColor>(i+1)));
    }
    
    place({2, 10}, new Polarizer(NORTH, LaserColor::MAGENTA));
    place({1, 10}, new Polarizer(NORTH, LaserColor::WHITE));
    place({3, 10}, new Tunnel(NORTH));
    place({4, 10}, new ColorShifter(NORTH));
    place({5, 10}, new ColorInverter(NORTH));
    place({6, 10}, new StrictGoal(LaserColor::NONE));
    place({7, 10}, new StrictGoal(LaserColor::YELLOW));
    place({8, 10}, new Teleporter());
    place({9, 10}, new Teleporter());
    place({10, 10}, new Refractor(NORTH));
    
    place({3, 3}, new Mirror(NORTH));
    place({4, 3}, new DoubleMirror(NORTH));
    
    place({2, 2}, new Splitter(NORTH));
    place({3, 2}, new DSplitter(NORTH));
    place({4, 2}, new Wall());
    place({5, 2}, new Glass());
    place({6, 2}, new Prism(NORTH));
    place({7, 2}, new Bender());
    place({8, 2}, new Twister());
    place({9, 2}, new RoundFilter());
    place({10, 2}, new CrossColorInverter(NORTH));
    place({11, 2}, new SkewMirror(NORTH));
    place({12, 2}, new DoubleSkewMirror(NORTH));
    place({13, 2}, new DoubleSplitterMirror(NORTH));
    place({14, 2}, new StarSplitter(this));
    */
    
    /*std::stringstream ss;
    for (int i = 0; i < width(); ++i)
      for (int j = 0; j < height(); ++j)
        if (tileAt(i, j)->piece())
        {
          PieceSaveInfo info = Files::savePiece(tileAt(i, j)->piece());
          ss << info.data;
          
        }
    
    std::string res = ss.str();
    char *output;
    size_t outLen;
    Files::encode(reinterpret_cast<const u8*>(res.c_str()), res.length(), &output, &outLen);
    printf("%*s\n",static_cast<int>(outLen),output);*/
    
    
    updateLasers();
  }
  
  LevelSpec* const level() { return _level; }
  u32 width() const { return _width; }
  u32 height() const { return _height; }
  u32 invWidth() const { return _invWidth; }
  u32 invHeight() const { return _invHeight; }
  
  bool isInside(const Pos& p) const { return p.x >= 0 && p.x < _width && p.y >= 0 && p.y < _height; }

  void reset()
  {
    goals.clear();
    beams.clear();
    failed = false;
    
    std::for_each(tiles, tiles + _width*_height, [] (Tile& tile) { tile.resetLasers(); tile.clear(); });
    std::for_each(inventory, inventory + _invWidth*_invHeight, [] (Tile& tile) { tile.clear(); });    
  }

  Piece* generatePiece(const PieceInfo *info);
  void load(LevelSpec* level);

  void fail() { failed = true; }
  bool isFailed() { return failed; }
  
  void place(Position p, Piece* piece)
  {
    Tile* tile = tileAt(p);
    tile->place(piece);
  }
  
  inline const Tile* tileAt(Position p) const {
    return p.type == Position::Type::INVENTORY ?
      &inventory[p.y * _invWidth + p.x] :
      &tiles[p.y * _width + p.x];
  }

  inline Tile* tileAt(Position p) {
    return p.type == Position::Type::INVENTORY ?
    &inventory[p.y * _invWidth + p.x] :
    &tiles[p.y * _width + p.x];
  }

  void addGoal(Goal* goal) { goals.push_back(goal); }

  void generateBeam(Position position, Direction direction, LaserColor color);
  void updateLasers();
  bool checkForWin();
};
  


#endif
