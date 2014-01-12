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
static const u16 FIELD_HEIGHT = 15;

static const u16 INVENTORY_WIDTH = 4;
static const u16 INVENTORY_HEIGHT = 6;

static const u16 TILE_SIZE = 15;
static const u16 PIECE_SIZE = 14;

static const u16 GFX_FIELD_POS_X = 0;
static const u16 GFX_FIELD_POS_Y = 5;
static const u16 GFX_INVENTORY_POS_X = TILE_SIZE*FIELD_WIDTH + 10;




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
  PIECE_AIR,
  PIECE_WALL,
  PIECE_SOURCE,
  PIECE_TARGET,
  PIECE_MIRROR,
  PIECE_DOUBLE_MIRROR,
  PIECE_SPLITTER,
  PIECE_DSPLITTER, // TODO: rename
  PIECE_PRISM,
  PIECE_BENDER,
  PIECE_FILTER,
  PIECE_POLARIZER,
  PIECE_TUNNEL,
  PIECE_COLOR_SHIFTER,
  PIECE_COLOR_INVERTER,
  
  PIECE_STRICT_GOAL,
  PIECE_SPLIT_GOAL
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
  s8 x, y;
  
  Position(s8 x, s8 y) : x(x), y(y) { }
  
  void shift(Direction direction) { x += directions[direction][0]; y += directions[direction][1]; }
  Position derived(Direction direction) { return Position(x+directions[direction][0], y+directions[direction][1]); }
  
  static const s8 directions[8][2];
};

  
#endif
