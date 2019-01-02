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

#include <cassert>

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

class Piece;
class Field;

class PieceMechanics
{
public:
  using on_laser_receive_t = std::function<void(Field*, const Piece*, Laser&)>;
  using on_laser_generate_t = std::function<Laser(const Piece*)>;
  using blocks_laser_predicate = std::function<bool(const Piece*, const Laser&)>;

private:
  bool _canBeColored, _canBeRotated;
  blocks_laser_predicate _doesBlockLaser;
  on_laser_receive_t _onLaserReceive;
  on_laser_generate_t _onLaserGeneration;

public:
  PieceMechanics(bool canBeRotated, bool canBeColored, blocks_laser_predicate doesBlockLaser) : PieceMechanics(canBeRotated, canBeColored, doesBlockLaser, emptyMechanics(), emptyGenerator()) { }
  PieceMechanics(bool canBeRotated, bool canBeColored, blocks_laser_predicate doesBlockLaser, on_laser_receive_t onLaserReceive) : PieceMechanics(canBeRotated, canBeColored, doesBlockLaser, onLaserReceive, emptyGenerator()) { }
  PieceMechanics(bool canBeRotated, bool canBeColored, blocks_laser_predicate doesBlockLaser, on_laser_receive_t onLaserReceive, on_laser_generate_t onLaserGeneration) :
    _canBeColored(canBeColored), _canBeRotated(canBeRotated), _doesBlockLaser(doesBlockLaser), _onLaserReceive(onLaserReceive), _onLaserGeneration(onLaserGeneration) { }

  inline bool canBeColored() const { return _canBeColored; }
  inline bool canBeRotated() const { return _canBeRotated; }
  inline bool doesBlockLaser(const Piece* piece, const Laser& laser) const { return _doesBlockLaser(piece, laser); }

  inline void onLaserReceive(Field* field, const Piece* piece, Laser& laser) const { _onLaserReceive(field, piece, laser); }
  inline Laser onLaserGeneration(const Piece* piece) const { return _onLaserGeneration(piece); }

  static const PieceMechanics* mechanicsForType(PieceType type);

private:
  static on_laser_receive_t emptyMechanics() { return [](Field*, const Piece*, Laser&) {}; }
  static on_laser_generate_t emptyGenerator() { return [](const Piece*) { return Laser(Pos(-1, -1), Dir::NORTH, LaserColor::NONE); }; }

  static inline blocks_laser_predicate always() { return [](const Piece*, const Laser&) { return true;  }; }
  static inline blocks_laser_predicate never() { return [](const Piece*, const Laser&) { return false;  }; }
};

class Tile;

class Piece
{
protected:
  const PieceMechanics* mechanics;
  PieceType type_;
  Direction rotation_;
  LaserColor color_;
  bool movable, roteable, colorable, infinite;
  
public:
  Piece(PieceType type) : Piece(type, Dir::NORTH, LaserColor::NONE) { }
  Piece(PieceType type, Dir orientation) : Piece(type, orientation, LaserColor::NONE) { }
  Piece(PieceType type, LaserColor color) : Piece(type, Dir::NORTH, color) { }

  Piece(PieceType type, Direction orientation, LaserColor color) :
    mechanics(PieceMechanics::mechanicsForType(type)),
    type_(type), rotation_(orientation), color_(color),
    movable(true), roteable(true), infinite(false), colorable(false)
  { 
    if (mechanics)
    {
      assert(mechanics->canBeRotated() || orientation == Dir::NORTH);
      assert(mechanics->canBeColored() || color == LaserColor::NONE);
    }
  }
  
  Piece* dupe() const {
    Piece* piece = new Piece(type_);
    piece->mechanics = this->mechanics;
    piece->rotation_ = this->rotation_;
    piece->color_ = this->color_;
    piece->movable = this->movable;
    piece->roteable = this->roteable;
    piece->colorable = this->colorable;
    piece->infinite = this->infinite;
    return piece;
  }

  virtual ~Piece() { }
  
  Direction orientation() const { return rotation_;  }
  Direction rotation() const { return rotation_; }
  PieceType type() const { return type_; }
  LaserColor color() const { return color_; }
  
  void rotateLeft() { rotation_ = rotation_ == NORTH ? NORTH_WEST : static_cast<Direction>(rotation_-1); }
  void rotateRight() { rotation_ = rotation_ == NORTH_WEST ? NORTH : static_cast<Direction>(rotation_+1); }
  void setColor(LaserColor color) { color_ = color;  }
  
  virtual Laser produceLaser() const { 
    return mechanics ? mechanics->onLaserGeneration(this) : Laser(Position(-1, -1), Direction::NORTH, LaserColor::NONE);
  }
  virtual bool blocksLaser(const Laser &laser) { return mechanics ? mechanics->doesBlockLaser(this, laser) : false; }
  virtual void receiveLaser(Field* field, Laser& laser) { if (mechanics) mechanics->onLaserReceive(field, this, laser); }
  
  void setCanBeMoved(bool value) { movable = value; };
  void setCanBeRotated(bool value) { roteable = value; }
  void setInfinite(bool value) { infinite = value; }
  void setCanBeColored(bool value) { colorable = value;  }
  
  virtual bool canBeMoved() const { return movable; }
  virtual bool canBeRotated() const { return roteable; }
  bool canBeColored() const { return colorable; }
  bool isInfinite() const { return infinite; }
  
  int deltaDirection(const Laser& laser) const
  {
    int delta = laser.direction - rotation_;
    
    if (delta > 4) delta -= 8;
    else if (delta < -4) delta += 8;
    else if (delta == -4) delta = 4;
    
    return delta;
  }
};


class RoundFilter : public Piece
{
public:
  RoundFilter() : Piece(PIECE_FILTER, NORTH, LaserColor::NONE) { }
  
  bool blocksLaser(const Laser &laser) override
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
