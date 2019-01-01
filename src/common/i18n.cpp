#include "common/i18n.h"

const char* i18n::nameForPiece(PieceType type)
{
  switch (type)
  {
    case PIECE_WALL: return "Wall";
    case PIECE_SOURCE: return "Source";

    case PIECE_MIRROR: return "Mirror";
    case PIECE_DOUBLE_MIRROR: return "Double Mirror";
    case PIECE_SKEW_MIRROR: return "Skew Mirror";
    case PIECE_DOUBLE_SKEW_MIRROR: return "Double Skew Mirror";
    case PIECE_DOUBLE_PASS_MIRROR: return "Double Pass Mirror";
    case PIECE_DOUBLE_SPLITTER_MIRROR: return "Double Splitter Mirror";
    case PIECE_REFRACTOR: return "Refractor";

    case PIECE_SPLITTER: return "Splitter";
    case PIECE_ANGLED_SPLITTER: return "Angled Splitter";
    case PIECE_PRISM: return "Prism";

    case PIECE_GLASS: return "Glass";
    case PIECE_FILTER: return "Filter";
    case PIECE_POLARIZER: return "Polarizer";

    case PIECE_RIGHT_BENDER: return "Right Bender";
    case PIECE_LEFT_BENDER: return "Left Bender";
    case PIECE_RIGHT_TWISTER: return "Right Twister";
    case PIECE_LEFT_TWISTER: return "Left Twister";

    case PIECE_SELECTOR: return "Selector";
    case PIECE_SPLICER: return "Splicer";


    default: return "Unnamed Piece";
  }
}
