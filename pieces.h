//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _PIECES_H_
#define _PIECES_H_

#include "common.h"

#include <SDL/SDL.h>
#include <list>

#define UNUSED(x) (void)(x)
  
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
    s8 delta = direction - steps;
    if (delta < 0) delta += 8;
    direction = static_cast<Direction>(delta);
  }
  
  void rotateRight(u8 steps)
  {
    s8 delta = direction + steps;
    delta %= 8;
    direction = static_cast<Direction>(delta);
  }
  
  void flip() { rotateLeft(4); }
  
  Direction rotatedDirection(u8 steps)
  {
    s8 delta = direction + steps;
    if (delta < 0) delta += 8;
    else if (delta >= 8) delta %= 8;
    return static_cast<Direction>(delta);
  }
};

class Field;

class Piece
{
  protected:
    PieceType type_;
    Direction rotation_;
    LaserColor color_;
    bool movable, roteable;
  
    Field *field;
  
    virtual Position gfxTile() = 0;
  
  public:
    Piece(PieceType type, Direction rotation, LaserColor color, Field *field) : type_(type), rotation_(rotation), color_(color), movable(true), roteable(true), field(field) { }
  
    Direction rotation() { return rotation_; }
    PieceType type() { return type_; }
    LaserColor color() { return color_; }
  
    void rotateLeft() { rotation_ = rotation_ == NORTH ? NORTH_WEST : static_cast<Direction>(rotation_-1); }
    void rotateRight() { rotation_ = rotation_ == NORTH_WEST ? NORTH : static_cast<Direction>(rotation_+1); }
  
    virtual bool produceLaser() = 0;
    virtual bool blocksLaser(Laser &laser) = 0;
    virtual void receiveLaser(Laser &laser) = 0;
  
    virtual bool canBeMoved() = 0;
    virtual bool canBeRotated() = 0;
  
    SDL_Rect gfxRect()
    {
      Position tile = gfxTile();
      return {static_cast<s16>(tile.x*PIECE_SIZE), static_cast<s16>(tile.y*PIECE_SIZE), PIECE_SIZE, PIECE_SIZE};
    }
  
    s8 deltaDirection(Laser& laser)
    {
      s8 delta = laser.direction - rotation_;
      
      if (delta > 4) delta -= 8;
      else if (delta < -4) delta += 8;
      else if (delta == -4) delta = 4;
      
      return delta;
    }
};

class Wall : public Piece
{
  public:
    Wall(Field *field) : Piece(PIECE_WALL, NORTH, COLOR_NONE, field) { }

    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return true; }
    void receiveLaser(Laser &laser) { UNUSED(laser); }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return false; }
  
    Position gfxTile() { return Position(13,7); }
};

class LaserSource : public Piece
{
  public:
    LaserSource(Direction rotation, LaserColor color, Field *field) : Piece(PIECE_SOURCE, rotation, color, field) { }
    
    bool produceLaser() { return true; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return true; }
    void receiveLaser(Laser &laser) { UNUSED(laser); /*laser.invalidate();*/ }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{rotation_, 1}; }
};

class Mirror : public Piece
{
  public:
    Mirror(Direction rotation, Field *field) : Piece(PIECE_MIRROR, rotation, COLOR_NONE, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
    void receiveLaser(Laser &laser)
    {
      s8 delta = deltaDirection(laser);
      
      if (delta == 0)
        laser.flip();
      else if (delta == -1)
        laser.rotateLeft(2);
      else if (delta == 1)
        laser.rotateRight(2);
      else
        laser.invalidate();
    }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{rotation_, 0}; }
};

class DoubleMirror : public Piece
{
  public:
    DoubleMirror(Direction rotation, Field *field) : Piece(PIECE_DOUBLE_MIRROR, rotation, COLOR_NONE, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
    void receiveLaser(Laser &laser)
    {
      s8 delta = deltaDirection(laser);
      
      if (delta == 0 || delta == 4)
        laser.flip();
      else if (delta == -1 || delta == 3)
        laser.rotateLeft(2);
      else if (delta == 1 || delta == -3)
        laser.rotateRight(2);
      else
        laser.invalidate();
    }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{static_cast<s8>(rotation_%4 + 4), 5}; }
};

class Splitter : public Piece
{
  public:
    Splitter(Direction rotation, Field *field) : Piece(PIECE_SPLITTER, rotation, COLOR_NONE, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
    void receiveLaser(Laser &laser);
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{rotation_, 2}; }
};

class DSplitter : public Piece
{
  public:
    DSplitter(Direction rotation, Field *field) : Piece(PIECE_DSPLITTER, rotation, COLOR_NONE, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
    void receiveLaser(Laser &laser);
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{rotation_, 3}; }
};

class Prism : public Piece
{
public:
  Prism(Direction rotation, Field *field) : Piece(PIECE_PRISM, rotation, COLOR_NONE, field) { }
  
  bool produceLaser() { return false; }
  bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
  void receiveLaser(Laser &laser);
  
  bool canBeMoved() { return movable; }
  bool canBeRotated() { return roteable; }
  
  Position gfxTile() { return Position{rotation_, 4}; }
};

class Bender : public Piece
{
public:
  Bender(Field *field) : Piece(PIECE_PRISM, NORTH, COLOR_NONE, field) { }
  
  bool produceLaser() { return false; }
  bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
  void receiveLaser(Laser &laser)
  {
    laser.rotateRight(1);
  }
  
  bool canBeMoved() { return movable; }
  bool canBeRotated() { return false; }
  
  Position gfxTile() { return Position{14, 7}; }
};

class Filter : public Piece
{
  public:
    Filter(LaserColor color, Field *field) : Piece(PIECE_FILTER, NORTH, color, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { return (color_ & laser.color) == 0; }
    void receiveLaser(Laser &laser)
    {
      laser.color = static_cast<LaserColor>(laser.color & color_);
    }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return false; }
  
    Position gfxTile() { return Position{color_+8, 8}; }
};

class Polarizer : public Piece
{
  public:
    Polarizer(Direction rotation, LaserColor color, Field *field) : Piece(PIECE_POLARIZER, rotation, color, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { return deltaDirection(laser)%4 != 0 || (color_ & laser.color) == 0; }
    void receiveLaser(Laser &laser)
    {
      laser.color = static_cast<LaserColor>(laser.color & color_);
    }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{static_cast<s8>(color_+8), static_cast<s8>(9 + rotation_%4)}; }
};

class Tunnel : public Piece
{
  public:
    Tunnel(Direction rotation, Field *field) : Piece(PIECE_TUNNEL, rotation, COLOR_NONE, field) { }
    
    bool produceLaser() { return false; }
    bool blocksLaser(Laser &laser) { UNUSED(laser); return false; }
    void receiveLaser(Laser &laser)
    {
      if (deltaDirection(laser) != 0)
        laser.invalidate();
    }
  
    bool canBeMoved() { return movable; }
    bool canBeRotated() { return roteable; }
  
    Position gfxTile() { return Position{rotation_, 6}; }
};


#endif
