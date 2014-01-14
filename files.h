//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _FILES_H_
#define _FILES_H_

#include "common.h"
#include <fstream>

class Field;

struct PieceInfoSpec
{
  PieceType type;
  u8 mapping;
  bool canBeColored;
  bool canBeRotated;
};

struct PieceInfo
{
  PieceInfoSpec *spec;
  u8 x, y;
  LaserColor color;
  Direction direction;
  bool moveable;
  bool roteable;
};

class Files
{
  
  public:
    u8 charForPiece(PieceType type);
    PieceType pieceForChar(u8 type);
    PieceInfoSpec *specForPiece(PieceType type);
  
    LaserColor colorForChar(u8 color);
    Direction directionForChar(u8 dir);
  
  
    PieceInfo loadPiece(std::istream is, Field *field);
};
  
#endif
