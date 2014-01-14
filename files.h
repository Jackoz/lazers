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
#include "pieces.h"
#include <fstream>

const int PIECE_INFO_SIZE = 1 + 1 + 1 + 1 + 1 + 1;

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

struct PieceSaveInfo {
  u8 data[PIECE_INFO_SIZE];
};

class Files
{
  
  public:
    static u8 charForPiece(PieceType type);
    static PieceType pieceForChar(u8 type);
    static PieceInfoSpec *specForPiece(PieceType type);
  
    static LaserColor colorForChar(u8 color);
    static Direction directionForChar(u8 dir);
  
    static u8 charForColor(LaserColor color);
    static u8 charForDirection(Direction dir);
  
  
    static PieceInfo loadPiece(std::istream is, Field *field);
    static PieceSaveInfo savePiece(Piece *piece);
  
};
  
#endif
