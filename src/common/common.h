//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>

using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using byte_t = u8;

using s32 = int32_t;

typedef signed char s8;
typedef signed short s16;

enum LaserColor : u8
{
  NONE    = 0b000,
  RED     = 0b001,
  GREEN   = 0b010,
  BLUE    = 0b100,
  
  YELLOW  = RED | GREEN,
  MAGENTA = RED | BLUE,
  CYAN    = GREEN | BLUE,
  
  WHITE   = RED | GREEN | BLUE
};

inline LaserColor& operator|=(LaserColor& c1, LaserColor c2) { c1 = static_cast<LaserColor>((int)c1 | (int)c2); return c1; }

struct Color
{
private:
  using type_t = u8;
  type_t value;
  
  Color(type_t value) : value(value) { }
public:
  
  inline Color operator~() const { return Color(~value); }
  inline Color operator|(Color c) const { return Color(value | c.value); }
  inline Color operator&(Color c) const { return Color(value & c.value); }
  inline Color operator<<(int v) const { return Color((value << 1 | (value & 0b100) >> 2) & 0x0b111); }
  inline Color operator>>(int v) const { return Color((value >> 1 | (value & 0b001) << 2) & 0x0b111); }
  inline Color next() { return this->operator<<(1); }
  inline Color prev() { return this->operator>>(1); }
};

enum PieceType : u8
{
  PIECE_WALL = 0,
  PIECE_SOURCE,

  PIECE_MIRROR,
  PIECE_SKEW_MIRROR,
  PIECE_DOUBLE_MIRROR,
  PIECE_DOUBLE_SKEW_MIRROR,
  PIECE_DOUBLE_SPLITTER_MIRROR,
  PIECE_DOUBLE_PASS_MIRROR,
  PIECE_REFRACTOR,

  PIECE_SPLITTER,
  PIECE_ANGLED_SPLITTER,
  PIECE_PRISM,
  PIECE_THREE_WAY_SPLITTER,
  PIECE_STAR_SPLITTER,

  PIECE_GLASS,
  PIECE_FILTER,
  PIECE_FLIPPED_PRISM,
  PIECE_POLARIZER,

  PIECE_RIGHT_BENDER,
  PIECE_LEFT_BENDER,
  PIECE_RIGHT_TWISTER,
  PIECE_LEFT_TWISTER,

  PIECE_SELECTOR,
  PIECE_SPLICER,
  PIECE_COLOR_SHIFTER,
  PIECE_COLOR_INVERTER,

  PIECE_TNT,



  PIECE_TUNNEL,

  PIECE_CROSS_COLOR_INVERTER,
  PIECE_GATE,  // TODO
  PIECE_PRIMARY_GATE,  // TODO
  PIECE_QUANTUM_SPLITTER, // TODO
  PIECE_TELEPORTER, // TODO
  PIECE_COMPLEX_PRISM, // TODO
  PIECE_ROUND_FILTER,
  
  PIECE_UFO, // TODO (level pack 1, 1, 4)
  PIECE_CRYSTAL, // TODO (level pack 1, 1, 4)
  
  PIECE_SLIME, // TODO
  PIECE_MINE, // TODO
  PIECE_VOID_HOLE, // TODO

  PIECE_STRICT_GOAL,
  PIECE_LOOSE_GOAL,
  
  PIECES_COUNT
};
  
enum Direction
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

using Dir = Direction;

#include <cassert>

struct Position
{
  enum class Type : u8 { FIELD, INVENTORY, INVALID };
  
  Type type;
  u8 x, y;
  
  Position(Type type, s32 x, s32 y) : type(type), x(x), y(y) { assert(x >= 0 && y >= 0); }
  Position(s32 x, s32 y) : Position(Type::FIELD, x, y) { assert(x >= 0 && y >= 0);  }
  Position(Type type) : Position(Type::INVALID, 0, 0) { }
  
  bool isValid() const { return type != Position::Type::INVALID; }
  bool isInventory() const { return type == Position::Type::INVENTORY; }

  Position& operator+=(Direction dir) { x += directions[dir][0]; y += directions[dir][1]; return *this; }
  Position operator+(Direction dir) const { return Position(x + directions[dir][0], y + directions[dir][1]); }
  Position operator+(const Position& o) const { return Position(type, x + o.x, y + o.y); }
  
  static Position invalid() { return Position(Position::Type::INVALID); }
  static const s8 directions[8][2];

};

using Pos = Position;

  
#endif
