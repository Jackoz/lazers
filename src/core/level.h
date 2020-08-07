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
  u8 x, y;

public:
  std::array<LaserColor, 8> colors;
  
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
  
  const LevelSpec* _level;
  
  std::vector<Tile> tiles;
  std::vector<Tile> inventory;
  std::list<Laser> lasers;
  std::list<std::unique_ptr<Goal>> goals;
  std::unordered_set<Laser, Laser::hash> beams;
  
  bool won;
  bool failed;

public:
  Field(u32 width, u32 height, u32 invWidth, u32 invHeight) :
  _width(width), _height(height),
  _invWidth(invWidth), _invHeight(invHeight),
  _level(nullptr),
  failed(false), won(false)
  {
    tiles.resize(width*height);
    inventory.resize(invWidth*invHeight);
    
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
  
  const LevelSpec* const level() { return _level; }
  u32 width() const { return _width; }
  u32 height() const { return _height; }
  u32 invWidth() const { return _invWidth; }
  u32 invHeight() const { return _invHeight; }
  
  bool isInside(const Pos& p) const { return p.x >= 0 && p.x < _width && p.y >= 0 && p.y < _height; }
  bool isInsideInventory(const Pos& p) const { return p.x >= 0 && p.x < _invWidth && p.y >= 0 && p.y < _invHeight; }

  void reset()
  {
    goals.clear();
    beams.clear();
    failed = false;
    
    std::for_each(tiles.begin(), tiles.end(), [] (Tile& tile) { tile.resetLasers(); tile.clear(); });
    std::for_each(inventory.begin(), inventory.end(), [] (Tile& tile) { tile.clear(); });    
  }

  Piece* generatePiece(const PieceInfo& info);
  void load(const LevelSpec* level);

  void fail() { failed = true; }
  bool isFailed() const { return failed; }
  bool isWon() const { return won; }
  
  void place(Position p, Piece* piece)
  {
    Tile* tile = tileAt(p);
    tile->place(piece);
  }
  
  inline const Tile* tileAt(Position p) const {
    if (p.type == Position::Type::INVENTORY && isInsideInventory(p))
      return &inventory[p.y * _invWidth + p.x];
    else if (isInside(p))
      return &tiles[p.y * _width + p.x];
    else return nullptr;
  }

  inline Tile* tileAt(Position p) {
    if (p.type == Position::Type::INVENTORY && isInsideInventory(p))
      return &inventory[p.y * _invWidth + p.x];
    else if (isInside(p))
      return &tiles[p.y * _width + p.x];
    else return nullptr;
  }

  void generateBeam(Position position, Direction direction, LaserColor color);
  void updateLasers();

  void checkForWin();

  void generateDummy();
};
  


#endif
