//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _COMMON_H_
#define _COMMON_H_

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

typedef signed char s8;
typedef signed short s16;

static const u16 FIELD_WIDTH = 16;
static const u16 FIELD_HEIGHT = 11;

static const u16 INVENTORY_WIDTH = 4;
static const u16 INVENTORY_HEIGHT = 6;

static const u16 TILE_SIZE = 15;
static const u16 PIECE_SIZE = 14;

#define KEY_LEFT (SDLK_LEFT)
#define KEY_RIGHT (SDLK_RIGHT)
#define KEY_UP (SDLK_UP)
#define KEY_DOWN (SDLK_DOWN)
#define KEY_X (SDLK_LSHIFT)
#define KEY_Y (SDLK_SPACE)
#define KEY_A (SDLK_LCTRL)
#define KEY_B (SDLK_LALT)
#define KEY_L (SDLK_TAB)
#define KEY_R (SDLK_BACKSPACE)
#define KEY_SELECT (SDLK_ESCAPE)
#define KEY_START (SDLK_RETURN)


enum LaserColor : u8
{
  COLOR_NONE = 0x00,
  COLOR_RED = 0x01,
  COLOR_GREEN = 0x02,
  COLOR_BLUE = 0x04,
  
  COLOR_YELLOW = 0x03,
  COLOR_MAGENTA = 0x05,
  COLOR_CYAN = 0x06,
  
  COLOR_WHITE = 0x07
};
  
  
enum PieceType : u8
{
  PIECE_WALL = 0,
  PIECE_GLASS,
  PIECE_SOURCE,
  PIECE_MIRROR,
  PIECE_SKEW_MIRROR,
  PIECE_DOUBLE_MIRROR,
  PIECE_DOUBLE_SKEW_MIRROR,
  PIECE_DOUBLE_SPLITTER_MIRROR,
  PIECE_DOUBLE_PASS_MIRROR, // TODO
  PIECE_SPLITTER,
  PIECE_DSPLITTER, // TODO: rename
  PIECE_PRISM,
  PIECE_FLIPPED_PRISM,
  PIECE_BENDER,
  PIECE_REFRACTOR,
  PIECE_TWISTER,
  PIECE_FILTER,
  PIECE_ROUND_FILTER,
  PIECE_POLARIZER,
  PIECE_TUNNEL,
  PIECE_COLOR_SHIFTER,
  PIECE_COLOR_INVERTER,
  PIECE_CROSS_COLOR_INVERTER,
  PIECE_STAR_SPLITTER,
  PIECE_GATE,  // TODO
  PIECE_PRIMARY_GATE,  // TODO
  PIECE_QUANTUM_SPLITTER, // TODO
  PIECE_TELEPORTER, // TODO
  PIECE_COMPLEX_PRISM, // TODO
  PIECE_THREE_WAY_SPLITTER, // TODO
  
  PIECE_SELECTOR,
  PIECE_SPLICER,
  
  PIECE_UFO, // TODO (level pack 1, 1, 4)
  PIECE_CRYSTAL, // TODO (level pack 1, 1, 4)
  
  PIECE_TNT, // TODO
  PIECE_SLIME, // TODO
  PIECE_MINE, // TODO
  PIECE_VOID_HOLE, // TODO

  PIECE_STRICT_GOAL,
  PIECE_LOOSE_GOAL,
  
  PIECES_COUNT
};
  
enum Direction : u8
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
  enum class Type : u8 { FIELD, INVENTORY, INVALID };
  
  Type type;
  s8 x, y;
  
  Position(Type type, s8 x, s8 y) : type(type), x(x), y(y) { }
  Position(s8 x, s8 y) : Position(Type::FIELD, x, y) { }
  Position(Type type) : Position(Type::INVALID, -1, -1) { }
  
  bool isValid() const { return type != Type::INVALID && x >= 0 && y >= 0 && x < FIELD_WIDTH && y < FIELD_HEIGHT; }

  Position& operator+=(Direction dir) { x += directions[dir][0]; y += directions[dir][1]; return *this; }
  Position operator+(Direction dir) const { return Position(x + directions[dir][0], y + directions[dir][1]); }
  
  static const s8 directions[8][2];
};

  
#endif
