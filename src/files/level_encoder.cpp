#include "level_encoder.h"

#include "files.h"

#include <array>
#include <cassert>

// TODO: mancano pezzi nuovi
PieceInfoSpec specs[PIECES_COUNT] =
{
  { PIECE_WALL, 'W', false, false },
  { PIECE_GLASS, 'y', false, false },
  { PIECE_SOURCE, 'S', true, true },
  { PIECE_MIRROR, 'M', false, true },
  { PIECE_SKEW_MIRROR, 'K', false, true },
  { PIECE_DOUBLE_MIRROR, 'D', false, true },
  { PIECE_DOUBLE_SKEW_MIRROR, 'J', false, true },
  { PIECE_DOUBLE_PASS_MIRROR, 'P', false, true },
  { PIECE_DOUBLE_SPLITTER_MIRROR, 'd', false, true },
  { PIECE_SPLITTER, 'T', false, true },
  { PIECE_ANGLED_SPLITTER, 'U', false, true },
  { PIECE_REFRACTOR, 'r', false, true },
  { PIECE_PRISM, 'p', false, true },
  { PIECE_FLIPPED_PRISM, 'p', false, true },
  { PIECE_SELECTOR, 's', true, true },
  { PIECE_SPLICER, ' ', true, true },
  { PIECE_RIGHT_BENDER, 'B', false, false },
  { PIECE_RIGHT_TWISTER, 'b', false, false },
  { PIECE_FILTER, 'F', true, false },
  { PIECE_ROUND_FILTER, 'f', false, true },
  { PIECE_POLARIZER, 'Z', true, true },
  { PIECE_TUNNEL, 'u', false, true },
  { PIECE_COLOR_SHIFTER, 'H', false, true },
  { PIECE_COLOR_INVERTER, 'I', false, true },
  { PIECE_CROSS_COLOR_INVERTER, 'X', false, true },
  { PIECE_STAR_SPLITTER, 'x', false, false },
  { PIECE_GATE, 'G', false, true },
  { PIECE_PRIMARY_GATE, 'g', false, true },
  { PIECE_QUANTUM_SPLITTER, 'Q', false, true },
  { PIECE_TELEPORTER, 'E', false, false },
  { PIECE_COMPLEX_PRISM, 'i', false, true },
  { PIECE_THREE_WAY_SPLITTER, 'r', false, true },
  { PIECE_TNT, 'N', false, false },
  { PIECE_SLIME, 'm', false, false },
  { PIECE_MINE, 'e', false, false },
  { PIECE_STRICT_GOAL, 'V', true, false },
  { PIECE_LOOSE_GOAL, 'v', true, false },

  { PIECE_UFO, 'f', false, false },
  { PIECE_CRYSTAL, 'Y', false, false },
  { PIECE_VOID_HOLE, 'a', false, false }
};


char LevelEncoder::charForPiece(PieceType type)
{
  for (int i = 0; i < sizeof(specs) / sizeof(specs[0]); ++i)
    if (specs[i].type == type)
      return specs[i].mapping;

  return 0;
}

PieceType LevelEncoder::pieceForChar(char type)
{
  for (int i = 0; i < sizeof(specs) / sizeof(specs[0]); ++i)
    if (specs[i].mapping == type)
      return specs[i].type;

  return PIECE_WALL;
}

PieceInfoSpec* LevelEncoder::specForPiece(PieceType type)
{
  for (int i = 0; i < sizeof(specs) / sizeof(specs[0]); ++i)
    if (specs[i].type == type)
      return &specs[i];

  return nullptr;
}

LaserColor LevelEncoder::colorForChar(char color)
{
  switch (color) {
    case 'n': case 'N': case '0': case ' ': return LaserColor::NONE;
    case 'r': case 'R': case '1': return LaserColor::RED;
    case 'g': case 'G': case '2': return LaserColor::GREEN;
    case 'b': case 'B': case '4': return LaserColor::BLUE;
    case 'c': case 'C': case '6': return LaserColor::CYAN;
    case 'm': case 'M': case '5': return LaserColor::MAGENTA;
    case 'y': case 'Y': case '3': return LaserColor::YELLOW;
    case 'w': case 'W': case '7': return LaserColor::WHITE;
    default: 
      assert(false);
      return LaserColor::WHITE;
  }
}

char LevelEncoder::charForColor(LaserColor color)
{
  switch (color) {
    case LaserColor::NONE: return 'n';
    case LaserColor::RED: return 'r';
    case LaserColor::GREEN: return 'g';
    case LaserColor::BLUE: return 'b';
    case LaserColor::CYAN: return 'c';
    case LaserColor::MAGENTA: return 'm';
    case LaserColor::YELLOW: return 'y';
    case LaserColor::WHITE: return 'w';
    default:
      assert(false);
  }
}

Direction LevelEncoder::directionForChar(char dir)
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
    default: 
      assert(false);
      return NORTH;
  }
}

char LevelEncoder::charForDirection(Direction dir)
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
    default:
      assert(false);
  }
}

char LevelEncoder::charForFlags(const PieceInfo& info)
{
  //TODO
  return ' ';
}

void LevelEncoder::flagsForChar(char flags, PieceInfo& info)
{
  switch (flags)
  {
    case ' ': break;
  }
}


u32 LevelEncoder::coordForChar(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'z')
    return c - 'a';
  else if (c >= 'A' && c <= 'Z')
    return c - 'A';
  else
    assert(false);
}

char LevelEncoder::charForCoord(u32 coord)
{
   //TODO
  return ' ';
}

/* type x y dir col flags*/
PieceInfo LevelEncoder::decodePieceFromString(const char* data, bool withFlags)
{
  PieceType type = pieceForChar(data[0]);
  PieceInfo info = PieceInfo(type);
  info.x = coordForChar(data[1]);
  info.y = coordForChar(data[2]);
  info.direction = directionForChar(data[3]);
  info.color = colorForChar(data[4]);

  if (withFlags)
    flagsForChar(data[5], info);

  return info;
}

std::array<char, 4> encodePieceToString(const PieceInfo& info, bool asciiCoords)
{
  return std::array<char, 4>();
}
