#pragma once

#include "files/repository.h"

class LevelEncoder
{
private:
  char charForPiece(PieceType type);
  PieceType pieceForChar(char type);

  LaserColor colorForChar(char color);
  char charForColor(LaserColor color);

  Direction directionForChar(char dir);
  char charForDirection(Direction dir);

  char charForFlags(const PieceInfo& info);
  void flagsForChar(char flags, PieceInfo& info);

  u32 coordForChar(char coord);
  char charForCoord(u32 coord);

  static PieceInfoSpec* specForPiece(PieceType type);

public:
  PieceInfo decodePieceFromString(const char* data, bool withFlags);
  std::array<char, 4> encodePieceToString(const PieceInfo& info, bool asciiCoords);
};