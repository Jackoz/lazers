#include "common/i18n.h"

const char* i18n::nameForPiece(PieceType type)
{
  switch (type)
  {
    case PIECE_MIRROR: return "Mirror";
    case PIECE_DOUBLE_MIRROR: return "Double Mirror";

    default: return "Unnamed Piece";
  }
}
