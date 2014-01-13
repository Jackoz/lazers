//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "files.h"
#include "lazers.h"

struct PieceInfoSpec
{
  PieceType type;
  u8 mapping;
  bool canBeColored;
  bool canBeRotated;
};

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

const int PIECE_INFO_SIZE = 1 + 2 + 2 + 1 + 1 + 1 + 1;

union PieceInfo
{
  char buffer[PIECE_INFO_SIZE];
  struct {
    u8 type;
    u8 x1;
    u8 x2;
    u8 y1;
    u8 y2;
    u8 dir;
    u8 col;
    u8 mov;
    u8 rot;
  };
};



void Files::loadPiece(std::istream is, Field *field)
{
  /*char buffer[PIECE_INFO_SIZE];
  is.read(buffer, PIECE_INFO_SIZE);
  
  PieceType type = pieceForChar(buffer[0]);
  Piece *piece;
  
  switch*/
}
