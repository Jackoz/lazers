//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _PIECES_H_
#define _PIECES_H_

#include "common/common.h"

#include <list>
#include <functional>

#define UNUSED(x) (void)(x)

struct Laser
{
  Position position;
  Direction direction;
  LaserColor color;
  
  Laser(Position position, Direction direction, LaserColor color) : position(position), direction(direction), color(color) { }
  
  void invalidate() { position.x = -1; position.y = -1; }
  
  void advance() { position += direction; }
  
  void rotateLeft(u8 steps)
  {
    int delta = direction - steps;
    if (delta < 0) delta += 8;
    direction = static_cast<Direction>(delta);
  }
  
  void rotateRight(u8 steps)
  {
    int delta = direction + steps;
    delta %= 8;
    direction = static_cast<Direction>(delta);
  }
  
  void flip() { rotateLeft(4); }
  
  Direction rotatedDirection(u8 steps)
  {
    int delta = direction + steps;
    if (delta < 0) delta += 8;
    else if (delta >= 8) delta %= 8;
    return static_cast<Direction>(delta);
  }
  
  bool operator==(const Laser &o) const { return (position.x == o.position.x && position.y == o.position.y && color == o.color && direction == o.direction); }
  
  struct hash
  {
  public:
    size_t operator()(const Laser& k) const
    {
      return (k.position.x << 24) | (k.position.y << 16) | ((size_t)k.color << 8) | k.direction;
    }
  };
};

class PieceMechanics
{
private:
  bool _canBeColored, _canBeRotated, _doesBlockLaser;
  std::function<void(Field*, Laser&)> onLaserReceive;
  std::function<Laser(void)> onLaserGeneration;

public:
  PieceMechanics(bool canBeColored, bool canBeRotated, bool doesBlockLaser, decltype(onLaserReceive) onLaserReceive, decltype(onLaserGeneration) onLaserGeneration) :
    _canBeColored(canBeColored), _canBeRotated(canBeRotated), _doesBlockLaser(doesBlockLaser), onLaserReceive(onLaserReceive), onLaserGeneration(onLaserGeneration) { }

  inline bool canBeColored() const { return _canBeColored; }
  inline bool canBeRotated() const { return _canBeRotated; }
  inline bool doesBlockLaser() const { return _doesBlockLaser; }

  inline void onLaserReceive(Field* field, Laser& laser) const { onLaserReceive(field, laser); }
  inline Laser onLaserGeneration() { return onLaserGeneration(); }

  static decltype(onLaserGeneration) emptyGenerator() { return []() { return Laser(Pos(-1, -1), Dir::NORTH, LaserColor::NONE); }; }
};

class Field;
class Tile;

class Piece
{
protected:
  PieceType type_;
  Direction rotation_;
  LaserColor color_;
  bool movable, roteable, colorable, infinite;
  
public:
  Piece(PieceType type, Direction rotation, LaserColor color) : type_(type), rotation_(rotation), color_(color), movable(true), roteable(true), infinite(false), colorable(false) { }
  virtual ~Piece() { }
  
  Direction rotation() const { return rotation_; }
  PieceType type() const { return type_; }
  LaserColor color() const { return color_; }
  
  void rotateLeft() { rotation_ = rotation_ == NORTH ? NORTH_WEST : static_cast<Direction>(rotation_-1); }
  void rotateRight() { rotation_ = rotation_ == NORTH_WEST ? NORTH : static_cast<Direction>(rotation_+1); }
  void setColor(LaserColor color) { color_ = color;  }
  
  virtual Laser produceLaser() const { return Laser(Position(-1, -1), Direction::NORTH, LaserColor::NONE); }
  virtual bool blocksLaser(Laser &laser) { return false; }
  virtual void receiveLaser(Field* field, Laser &laser) { };
  
  void setCanBeMoved(bool value) { movable = value; };
  void setCanBeRotated(bool value) { roteable = value; }
  void setInfinite(bool value) { infinite = value; }
  void setCanBeColored(bool value) { colorable = value;  }
  
  virtual bool canBeMoved() const { return movable; }
  virtual bool canBeRotated() const { return roteable; }
  bool canBeColored() const { return colorable; }
  bool isInfinite() const { return infinite; }
  
  int deltaDirection(Laser& laser)
  {
    int delta = laser.direction - rotation_;
    
    if (delta > 4) delta -= 8;
    else if (delta < -4) delta += 8;
    else if (delta == -4) delta = 4;
    
    return delta;
  }
};

class Wall : public Piece
{
public:
  Wall() : Piece(PIECE_WALL, NORTH, LaserColor::NONE) { }
  
  bool blocksLaser(Laser &laser) override { UNUSED(laser); return true; }
  bool canBeRotated() const override { return false; }
};

class Glass : public Piece
{
public:
  Glass() : Piece(PIECE_GLASS, NORTH, LaserColor::NONE) { }
  bool canBeRotated() const override { return false; }
};

class LaserSource : public Piece
{
public:
  LaserSource(Direction rotation, LaserColor color) : Piece(PIECE_SOURCE, rotation, color) { }

  Laser produceLaser() const override { return Laser(Position(0,0), rotation_, color_); }
  bool blocksLaser(Laser &laser) override { UNUSED(laser); return true; }
};

class Mirror : public Piece
{
public:
  Mirror(Direction rotation) : Piece(PIECE_MIRROR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta == 0)
      laser.flip();
    else if (delta == -1)
      laser.rotateLeft(2);
    else if (delta == 1)
      laser.rotateRight(2);
    else
      laser.invalidate();
  }
};

class SkewMirror : public Piece
{
public:
  SkewMirror(Direction rotation) : Piece(PIECE_SKEW_MIRROR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta == 0)
      laser.rotateRight(3);
    else if (delta == -1)
      laser.rotateLeft(3);
    else if (delta == -2)
      laser.rotateLeft(1);
    else if (delta == 1)
      laser.rotateRight(1);
    else
      laser.invalidate();
  }
};

class DoubleMirror : public Piece
{
public:
  DoubleMirror(Direction rotation) : Piece(PIECE_DOUBLE_MIRROR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta == 0 || delta == 4)
      laser.flip();
    else if (delta == -1 || delta == 3)
      laser.rotateLeft(2);
    else if (delta == 1 || delta == -3)
      laser.rotateRight(2);
    else
      laser.invalidate();
  }
};

class DoubleSplitterMirror : public Piece
{
public:
  DoubleSplitterMirror(Direction rotation) : Piece(PIECE_DOUBLE_SPLITTER_MIRROR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
};

class DoubleSkewMirror : public Piece
{
public:
  DoubleSkewMirror(Direction rotation) : Piece(PIECE_DOUBLE_SKEW_MIRROR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser)%4;
    if (delta < 0) delta += 4;
    
    if (delta == 0)
      laser.rotateRight(3);
    else if (delta == 3)
      laser.rotateLeft(3);
    else if (delta == 2)
      laser.rotateLeft(1);
    else if (delta == 1)
      laser.rotateRight(1);
    else
      laser.invalidate();
  }
  
};

class DoublePassMirror : public Piece
{
public:
  DoublePassMirror(Direction rotation) : Piece(PIECE_DOUBLE_PASS_MIRROR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta == 0 || delta == 4)
      laser.flip();
    else if (delta == -1 || delta == 3)
      laser.rotateLeft(2);
    else if (delta == 1 || delta == -3)
      laser.rotateRight(2);
  }
};

class Refractor : public Piece
{
public:
  Refractor(Direction rotation) : Piece(PIECE_REFRACTOR, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser)%4;
    if (delta < 0) delta += 4;
    
    if (delta == 0)
      laser.rotateRight(1);
    else if (delta == 1)
      laser.rotateLeft(1);
    else
      laser.invalidate();
  }
};

class Splitter : public Piece
{
public:
  Splitter(Direction rotation) : Piece(PIECE_SPLITTER, rotation, LaserColor::NONE) { }
  void receiveLaser(Field* field, Laser &laser) override;
};

class ThreeWaySplitter : public Piece
{
public:
  ThreeWaySplitter(Direction rotation) : Piece(PIECE_THREE_WAY_SPLITTER, rotation, LaserColor::NONE) { }
  void receiveLaser(Field* field, Laser &laser) override;
  
};

class StarSplitter : public Piece
{
public:
  StarSplitter(Field *field) : Piece(PIECE_STAR_SPLITTER, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
  
  bool canBeRotated() const override { return false; }
};

class DSplitter : public Piece
{
public:
  DSplitter(Direction rotation) : Piece(PIECE_ANGLED_SPLITTER, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
};

class Prism : public Piece
{
public:
  Prism(Direction rotation) : Piece(PIECE_PRISM, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
};

class FlippedPrism : public Piece
{
public:
  FlippedPrism(Direction rotation) : Piece(PIECE_FLIPPED_PRISM, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
};

class Selector : public Piece
{
public:
  Selector(Direction rotation, LaserColor color) : Piece(PIECE_SELECTOR, rotation, color) { }
  
  bool blocksLaser(Laser &laser) override { return deltaDirection(laser)%4 != 0; }
  void receiveLaser(Field* field, Laser &laser) override;
};

class Splicer : public Piece
{
public:
  Splicer(Direction rotation, LaserColor color) : Piece(PIECE_SPLICER, rotation, color) { }
  
  bool blocksLaser(Laser &laser) override { return deltaDirection(laser)%4 != 0; }
  void receiveLaser(Field* field, Laser &laser) override;
};

class Bender : public Piece
{
public:
  Bender() : Piece(PIECE_BENDER, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    laser.rotateRight(1);
  }
  
  bool canBeRotated() const override { return false; }
};

class Twister : public Piece
{
public:
  Twister() : Piece(PIECE_TWISTER, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    laser.rotateLeft(2);
  }
};


class Filter : public Piece
{
public:
  Filter(LaserColor color) : Piece(PIECE_FILTER, NORTH, color) { }
  
  bool blocksLaser(Laser &laser) override { return (color_ & laser.color) == LaserColor::NONE; }
  void receiveLaser(Field* field, Laser &laser) override
  {
    laser.color = static_cast<LaserColor>(laser.color & color_);
  }
  
  bool canBeRotated() const override { return false; }
};

class RoundFilter : public Piece
{
public:
  RoundFilter() : Piece(PIECE_FILTER, NORTH, LaserColor::NONE) { }
  
  bool blocksLaser(Laser &laser) override
  {
    int delta = deltaDirection(laser) % 4;
    if (delta < 0) delta += 4;
    
    if (delta == 3)
      return true;
    else if (delta == 0)
      return (laser.color & LaserColor::RED) == LaserColor::NONE;
    else if (delta == 1)
      return (laser.color & LaserColor::GREEN) == LaserColor::NONE;
    else if (delta == 2)
      return (laser.color & LaserColor::BLUE) == LaserColor::NONE;
    else return true;
  }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser) % 4;
    if (delta < 0) delta += 4;
    
    if (delta == 0)
      laser.color = static_cast<LaserColor>(laser.color & LaserColor::RED);
    else if (delta == 1)
      laser.color = static_cast<LaserColor>(laser.color & LaserColor::GREEN);
    else if (delta == 2)
      laser.color = static_cast<LaserColor>(laser.color & LaserColor::BLUE);
  }
};

class Polarizer : public Piece
{
public:
  Polarizer(Direction rotation, LaserColor color) : Piece(PIECE_POLARIZER, rotation, color) { }
  
  bool blocksLaser(Laser &laser) override { return deltaDirection(laser)%4 != 0 || (color_ & laser.color) == LaserColor::NONE; }
  void receiveLaser(Field* field, Laser &laser) override
  {
    laser.color = static_cast<LaserColor>(laser.color & color_);
  }
};

class Tunnel : public Piece
{
public:
  Tunnel(Direction rotation) : Piece(PIECE_TUNNEL, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    if (deltaDirection(laser) != 0)
      laser.invalidate();
  }
};

class ColorShifter : public Piece
{
public:
  ColorShifter(Direction rotation) : Piece(PIECE_COLOR_SHIFTER, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta%4 != 0)
      laser.invalidate();
    else
    {
      if (delta == 0)
        laser.color = static_cast<LaserColor>((laser.color >> 1) | ((laser.color & LaserColor::RED) << 2));
      else if (delta == 4)
        laser.color = static_cast<LaserColor>(((laser.color << 1) & LaserColor::WHITE) | ((laser.color & LaserColor::BLUE) >> 2));
    }
  }
  
  Position gfxTile() { return Position(rotation_, 8); }
};

class ColorInverter : public Piece
{
public:
  ColorInverter(Direction rotation) : Piece(PIECE_COLOR_INVERTER, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta%4 != 0)
      laser.invalidate();
    else
    {
      laser.color = static_cast<LaserColor>(~laser.color & LaserColor::WHITE);
    }
  }
};

class CrossColorInverter : public Piece
{
public:
  CrossColorInverter(Direction rotation) : Piece(PIECE_CROSS_COLOR_INVERTER, rotation, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override
  {
    int delta = deltaDirection(laser);
    
    if (delta%2 != 0)
      laser.invalidate();
    else
    {
      laser.color = static_cast<LaserColor>(~laser.color & LaserColor::WHITE);
    }
  }
};


class Teleporter : public Piece
{
public:
  Teleporter() : Piece(PIECE_TELEPORTER, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
  
  bool canBeRotated() const override { return false; }
};

class TNT : public Piece
{
public:
  TNT() : Piece(PIECE_TNT, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override;
  
  bool canBeRotated() const override { return false; }
};

class Slime : public Piece
{
public:
  Slime() : Piece(PIECE_SLIME, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override { UNUSED(laser); }; // TODO
};

class Mine : public Piece
{
public:
  Mine() : Piece(PIECE_MINE, NORTH, LaserColor::NONE) { }
  
  void receiveLaser(Field* field, Laser &laser) override { UNUSED(laser); }; // TODO
  
  bool canBeRotated() const override { return false; }
};



class Goal : public Piece
{
protected:
  bool satisfied;
  u8 satisfyDirection;
  LaserColor satisfyColor;
  
public:
  Goal(PieceType type, LaserColor color);
  
  bool isSatisfied() const { return satisfied; }
  void reset() {
    satisfied = color_ == LaserColor::NONE;
    satisfyColor = LaserColor::NONE;
    satisfyDirection = 0;
  }
};

class StrictGoal : public Goal
{
private:
  
public:
  StrictGoal(LaserColor color) : Goal(PIECE_STRICT_GOAL, color) { }
  
  void receiveLaser(Field* field, Laser &laser)
  {
    satisfyColor = static_cast<LaserColor>(satisfyColor | laser.color);
    satisfyDirection |= 1 << laser.direction;
    
    u8 foundDirections = 0;
    for (int i = 0; i < 4; ++i)
      if ((satisfyDirection & 1<<(4+i)) || (satisfyDirection & 1<<(i)))
        ++foundDirections;
    
    if (color_ == LaserColor::NONE)
      satisfied = satisfyColor == LaserColor::NONE && foundDirections == 0;
    else
      satisfied = satisfyColor == color_ && foundDirections == 1;
  }
};


#endif
