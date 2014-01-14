//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "files.h"

#include "level.h"



PieceInfoSpec specs[PIECES_COUNT] =
{
  {PIECE_WALL, 'W', false, false},
  {PIECE_GLASS, 'y', false, false},
  {PIECE_SOURCE, 'S', true, true},
  {PIECE_MIRROR, 'M', false, true},
  {PIECE_SKEW_MIRROR, 'K', false, true},
  {PIECE_DOUBLE_MIRROR, 'D', false, true},
  {PIECE_DOUBLE_SKEW_MIRROR, 'J', false, true},
  {PIECE_DOUBLE_PASS_MIRROR, 'P', false, true},
  {PIECE_DOUBLE_SPLITTER_MIRROR, 'd', false, true},
  {PIECE_SPLITTER, 'T', false, true},
  {PIECE_DSPLITTER, 'U', false, true},
  {PIECE_PRISM, 'p', false, true},
  {PIECE_BENDER, 'B', false, false},
  {PIECE_TWISTER, 'b', false, false},
  {PIECE_FILTER, 'F', true, false},
  {PIECE_ROUND_FILTER, 'f', false, true},
  {PIECE_POLARIZER, 'Z', true, true},
  {PIECE_TUNNEL, 'u', false, true},
  {PIECE_COLOR_SHIFTER, 'H', false, true},
  {PIECE_COLOR_INVERTER, 'I', false, true},
  {PIECE_CROSS_COLOR_INVERTER, 'X', false, true},
  {PIECE_STAR_SPLITTER, 'x', false, false},
  {PIECE_GATE, 'G', false, true},
  {PIECE_PRIMARY_GATE, 'g', false, true},
  {PIECE_QUANTUM_SPLITTER, 'Q', false, true},
  {PIECE_TELEPORTER, 'E', false, false},
  {PIECE_COMPLEX_PRISM, 'i', false, true},
  {PIECE_THREE_WAY_SPLITTER, 'r', false, true},
  {PIECE_TNT, 'N', false, false},
  {PIECE_SLIME, 'm', false, false},
  {PIECE_MINE, 'e', false, false},
  {PIECE_STRICT_GOAL, 'V', true, false},
  {PIECE_LOOSE_GOAL, 'v', true, false}
};

u8 Files::charForPiece(PieceType type)
{
  for (int i = 0; i < sizeof(specs)/sizeof(specs[0]); ++i)
    if (specs[i].type == type)
      return specs[i].mapping;
  
  return 0;
}

PieceType Files::pieceForChar(u8 type)
{
  for (int i = 0; i < sizeof(specs)/sizeof(specs[0]); ++i)
    if (specs[i].mapping == type)
      return specs[i].type;
  
  return PIECE_WALL;
}

PieceInfoSpec* Files::specForPiece(PieceType type)
{
  for (int i = 0; i < sizeof(specs)/sizeof(specs[0]); ++i)
    if (specs[i].type == type)
      return &specs[i];
  
  return nullptr;
}

LaserColor Files::colorForChar(u8 color)
{
  switch (color) {
    case 'n': case 'N': return COLOR_NONE;
    case 'r': case 'R': return COLOR_RED;
    case 'g': case 'G': return COLOR_GREEN;
    case 'b': case 'B': return COLOR_BLUE;
    case 'c': case 'C': return COLOR_CYAN;
    case 'm': case 'M': return COLOR_MAGENTA;
    case 'y': case 'Y': return COLOR_YELLOW;
    case 'w': case 'W': return COLOR_WHITE;
    default: return COLOR_WHITE;
  }
}

Direction Files::directionForChar(u8 dir)
{
  switch (dir) {
    case 'N': case 'n': case '0': return NORTH;
    case 'O': case 'o': case '1': return NORTH_EAST;
    case 'E': case 'e': case '2': return EAST;
    case 'A': case 'a': case '3': return SOUTH_EAST;
    case 'S': case 's': case '4': return SOUTH;
    case 'U': case 'u': case '5': return SOUTH_WEST;
    case 'W': case 'w': case '6': return WEST;
    case 'R': case 'r': case '7': return NORTH_WEST;
    default: return NORTH;
  }
}

const int PIECE_INFO_SIZE = 1 + 2 + 2 + 1 + 1 + 1 + 1;
// type x y color direction roteable moveable


PieceInfo Files::loadPiece(std::istream is, Field *field)
{
  char buffer[PIECE_INFO_SIZE];
  is.read(buffer, PIECE_INFO_SIZE);
  
  PieceType type = pieceForChar(buffer[0]);
  PieceInfoSpec *spec = specForPiece(type);
  PieceInfo info;
  
  if (spec)
  {
    info.spec = spec;
    info.x = (buffer[1] - '0')*10 + (buffer[2]-'0');
    info.y = (buffer[3] - '0')*10 + (buffer[4]-'0');
    info.color = colorForChar(buffer[5]);
    info.direction = directionForChar(buffer[6]);
    info.roteable = buffer[7] == 'y' || buffer[7] == 'Y' ? true : false;
    info.moveable = buffer[8] == 'y' || buffer[8] == 'Y' ? true : false;
  }
  else
    info.spec = nullptr;
  
  return info;
}
