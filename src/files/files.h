//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _FILES_H_
#define _FILES_H_

#include "common/common.h"
#include "core/pieces.h"
#include "files/repository.h"

#include <fstream>
#include <vector>
#include <string>

class Field;

class Files
{
  static u8 charForPiece(PieceType type);
  static PieceType pieceForChar(u8 type);
  
  static LaserColor colorForChar(u8 color);
  static Direction directionForChar(u8 dir);
  
  static u8 charForColor(LaserColor color);
  static u8 charForDirection(Direction dir);
  
public:
  static void encode(const u8 *input, size_t length, char **outputPtr, size_t *outputLength);
  static void decode(const char *input, size_t length, u8 **outputPtr, size_t *outputLength);
  
  static PieceInfoSpec *specForPiece(PieceType type);

  static LevelSpec loadLevel(const byte_t *ptr);
  static void saveLevel(const LevelSpec* level, byte_t **ptr, size_t *length);
  
  static void loadSolvedStatus();
  static void saveSolvedStatus();
  
  static std::vector<std::string> findFiles(std::string path, const char *ext);
  
  static std::vector<LevelPack> loadPacks();
  static LevelPack loadPack(const std::string& filename);
  static void savePack(const LevelPack& pack);

  static u32 selectedPack;
  
  
  friend struct PieceInfo;
};

#endif
