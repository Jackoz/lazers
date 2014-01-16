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
#include <vector>
#include <string>

const int PIECE_INFO_SIZE = 1 + 1 + 1;

class Field;
struct PieceInfoSpec;

struct PieceInfo
{
  PieceInfoSpec *spec;
  u8 x, y;
  LaserColor color;
  Direction direction;
  bool moveable;
  bool roteable;
  bool inventory;
  
  PieceInfo(PieceType type);
};

struct PieceInfoSpec
{
  PieceType type;
  u8 mapping;
  bool canBeColored;
  bool canBeRotated;
};

struct PieceSaveInfo {
  u8 data[PIECE_INFO_SIZE];
};

struct LevelSpec {
  private:
    std::vector<PieceInfo> pieces;
  
  public:
    LevelSpec(std::string name) : name(name) { }
  
    void add(PieceInfo piece) { pieces.push_back(piece); }
    size_t count() const { return pieces.size(); }
    const PieceInfo *at(u32 index) const { return &pieces[index]; }
  
    std::string name;
};

class LevelPack {
  private:
    std::vector<LevelSpec> levels;
  
  public:
    LevelPack(std::string name, std::string author, std::vector<LevelSpec> lvls) : LevelPack(name, author) {
      levels.insert(levels.begin(), lvls.begin(), lvls.end());
    }
    LevelPack(std::string name, std::string author) : name(name), author(author), selected(0) { }
    void addLevel(LevelSpec level) { levels.push_back(level); }
    u32 count() { return static_cast<u32>(levels.size()); }
    LevelSpec *at(u32 index) { return &levels[index]; }
  
    std::string name;
    std::string author;
    u32 selected;
};

class Files
{
  static u8 charForPiece(PieceType type);
  static PieceType pieceForChar(u8 type);
  
  static LaserColor colorForChar(u8 color);
  static Direction directionForChar(u8 dir);
  
  static u8 charForColor(LaserColor color);
  static u8 charForDirection(Direction dir);
  
  static std::vector<LevelPack> packs;
  
  
  public:
    static void encode(const u8 *input, size_t length, char **outputPtr, size_t *outputLength);
    static void decode(const char *input, size_t length, u8 **outputPtr, size_t *outputLength);
  
    static PieceInfoSpec *specForPiece(PieceType type);
  
    static PieceInfo loadPiece(const u8 *ptr);
    static PieceSaveInfo savePiece(Piece *piece);
    static PieceSaveInfo savePiece(const PieceInfo* piece);
  
    static LevelSpec loadLevel(const u8 *ptr);
    static void saveLevel(const LevelSpec& level, u8 **ptr, size_t *length);

    static LevelPack* packAt(u32 index) { return &packs[index]; }
    static u32 packCount() { return static_cast<u32>(packs.size()); }
  

  friend struct PieceInfo;
};
  
#endif
