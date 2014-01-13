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

class Files
{
  
  public:
    u8 charForPiece(PieceType type);
    PieceType pieceForChar(u8 type);
  
    void loadPiece(std::istream is, Field *field);
};
  
#endif
