//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "files.h"

#include "level.h"

const u8 base64map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void Files::encode(const char *input, size_t length, char **outputPtr)
{
  size_t steps = length/3;

  char *output = new char [steps*4];
  *outputPtr = output;
  
  for (size_t i = 0; i < steps; ++i)
  {
    const char *inputPiece = input+(i*3);
    char *outputPiece = output+(i*4);

    outputPiece[0] = base64map[ (inputPiece[0] & 0xFC) >> 2 ];
    outputPiece[1] = base64map[ ((inputPiece[0] & 0x03) << 4) | ((inputPiece[1] & 0xF0) >> 4) ];
    outputPiece[2] = base64map[ ((inputPiece[1] & 0x0F) << 2) | ((inputPiece[2] & 0xC0) >> 6) ];
    outputPiece[3] = base64map[ (inputPiece[2] & 0x3F) ];
  }
}



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

u8 Files::charForColor(LaserColor color)
{
  switch (color) {
    case COLOR_NONE: return 'n';
    case COLOR_RED: return 'r';
    case COLOR_GREEN: return 'g';
    case COLOR_BLUE: return 'b';
    case COLOR_CYAN: return 'c';
    case COLOR_MAGENTA: return 'm';
    case COLOR_YELLOW: return 'y';
    case COLOR_WHITE: return 'w';
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

u8 Files::charForDirection(Direction dir)
{
  switch (dir) {
    case NORTH: return 'n';
    case NORTH_EAST: return 'o';
    case EAST: return 'e';
    case SOUTH_EAST: return 'a';
    case SOUTH: return 's';
    case SOUTH_WEST: return 'u';
    case WEST: return 'w';
    case NORTH_WEST: return 'r';
  }
}

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
    info.x = buffer[1] >= 'A' ? 10 + buffer[1] - 'A' : buffer[1] - '0';
    info.y = buffer[2] >= 'A' ? 10 + buffer[2] - 'A' : buffer[2] - '0';
    info.color = colorForChar(buffer[3]);
    info.direction = directionForChar(buffer[4]);
    if (buffer[5] == 'b')
    {
      info.roteable = true;
      info.moveable = true;
    }
    else if (buffer[5] == 'm')
    {
      info.roteable = false;
      info.moveable = true;
    }
    else if (buffer[5] == 'r')
    {
      info.roteable = true;
      info.moveable = false;
    }
    else
    {
      info.moveable = false;
      info.roteable = false;
    }
  }
  else
    info.spec = nullptr;
  
  return info;
}

PieceSaveInfo Files::savePiece(Piece *piece)
{
  PieceSaveInfo info;
  
  info.data[0] = charForPiece(piece->type());
  Tile *tile = piece->getTile();
  info.data[1] = tile->x >= 10 ? tile->x - 10 + 'A' : tile->x + '0';
  info.data[2] = tile->y >= 10 ? tile->y - 10 + 'A' : tile->y + '0';
  info.data[3] = charForColor(piece->color());
  info.data[4] = charForDirection(piece->rotation());
  if (piece->canBeMoved() && piece->canBeRotated()) info.data[5] = 'b';
  else if (piece->canBeMoved()) info.data[5] = 'm';
  else if (piece->canBeRotated()) info.data[5] = 'r';
  else info.data[5] = 'n';
  return info;
}
