//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "pieces.h"

#include "level.h"

#include <unordered_map>

static PieceMechanics::on_laser_receive_t prismMechanics(bool flipped) {
  return [flipped](Field* field, const Piece* piece, Laser& laser)
  {
    const int sgn = flipped ? -1 : 1;
    const int delta = piece->deltaDirection(laser);

    /* forward direction: split color into channels */
    if (delta == 0)
    {
      if ((laser.color & LaserColor::RED) != LaserColor::NONE)
        field->generateBeam(laser.position, laser.direction, LaserColor::RED);
      if ((laser.color & LaserColor::GREEN) != LaserColor::NONE)
        field->generateBeam(laser.position, laser.rotatedDirection(sgn*1), LaserColor::GREEN);
      if ((laser.color & LaserColor::BLUE) != LaserColor::NONE)
        field->generateBeam(laser.position, laser.rotatedDirection(sgn*2), LaserColor::BLUE);

    }
    /* opposite direction: just red */
    else if (delta == 4)
    {
      if ((laser.color & LaserColor::RED) != LaserColor::NONE)
        field->generateBeam(laser.position, laser.direction, LaserColor::RED);
    }
    /* diagonal direction: green */
    else if (delta == -3 * sgn)
    {
      if ((laser.color & LaserColor::GREEN) != LaserColor::NONE)
        field->generateBeam(laser.position, laser.rotatedDirection(-1), LaserColor::GREEN);
    }
    /* orthogonal direction: blue*/
    else if (delta == -2 * sgn)
    {
      if ((laser.color & LaserColor::BLUE) != LaserColor::NONE)
        field->generateBeam(laser.position, laser.rotatedDirection(-2), LaserColor::BLUE);
    }

    laser.invalidate();
  };
}
  


const PieceMechanics* PieceMechanics::mechanicsForType(PieceType type)
{
  static const std::unordered_map<PieceType, PieceMechanics, enum_hash> mechanics = {
    { PIECE_WALL, PieceMechanics(false, false, always(), emptyMechanics(), emptyGenerator()) },
    { PIECE_SOURCE, PieceMechanics(true, true, always(), emptyMechanics(), [](const Piece* piece) { return Laser(Position(0,0), piece->orientation(), piece->color()); })}, //TODO: why 0,0?

    /* mirrors */
    { PIECE_MIRROR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) 
      {
        int delta = piece->deltaDirection(laser);

        switch (delta) {
          case 0: laser.flip(); break;
          case -1: laser.rotateLeft(2); break;
          case 1: laser.rotateRight(2); break;
          default: laser.invalidate();
        }
      })
    },
    { PIECE_SKEW_MIRROR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        switch (delta) {
          case 0: laser.rotateRight(3); break;
          case -1: laser.rotateLeft(3); break;
          case -2: laser.rotateLeft(1); break;
          case 1: laser.rotateRight(1); break;
          default: laser.invalidate();
        }
      })
    },
    { PIECE_DOUBLE_MIRROR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        switch (delta) {
          case 0: case 4: laser.flip(); break;
          case -1: case 3: laser.rotateLeft(2); break;
          case 1: case -3: laser.rotateRight(2); break;
          default: laser.invalidate();
        }
      })
    },
    { PIECE_DOUBLE_SKEW_MIRROR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser) % 4;
        if (delta < 0) delta += 4;

        switch (delta) {
          case 0: laser.rotateRight(3); break;
          case 3: laser.rotateLeft(3); break;
          case 2: laser.rotateLeft(1); break;
          case 1: laser.rotateRight(1); break;
          default: laser.invalidate();
        }
      })
    },

    { PIECE_DOUBLE_PASS_MIRROR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        switch (delta) {
          case 0: case 4: laser.flip(); break;
          case -1: case 3: laser.rotateLeft(2); break;
          case 1: case -3: laser.rotateRight(2); break;
        }
      })
    },

    { PIECE_DOUBLE_SPLITTER_MIRROR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        switch (delta) {
          case -1: case 3: field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color); break;
          case 1: case -3: field->generateBeam(laser.position, laser.rotatedDirection(2), laser.color); break;
          default: laser.invalidate();
        }
      })
    },


    { PIECE_REFRACTOR, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser) % 4;
        if (delta < 0) delta += 4;

        switch (delta) {
          case 0: laser.rotateRight(1); break;
          case 1: laser.rotateLeft(1); break;
          default: laser.invalidate(); break;
        }
      })
    },

    { PIECE_GLASS, PieceMechanics(false, false, never(), emptyMechanics(), emptyGenerator()) },
    { PIECE_FILTER, PieceMechanics(false, true, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.color = static_cast<LaserColor>(laser.color & piece->color()); })},
    { PIECE_POLARIZER, PieceMechanics(false, true, 
      [](const Piece* piece, const Laser& laser) { return piece->deltaDirection(laser) % 4 != 0 || (piece->color() & laser.color) == LaserColor::NONE; },
      [](Field* field, const Piece* piece, Laser& laser) { laser.color = static_cast<LaserColor>(laser.color & piece->color()); })
    },
    { PIECE_TUNNEL, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        if (piece->deltaDirection(laser) != 0)
          laser.invalidate();
      })
    },

    { PIECE_RIGHT_BENDER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateRight(1); }) },
    { PIECE_LEFT_BENDER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateLeft(1); }) },
    { PIECE_RIGHT_TWISTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateRight(2); }) },
    { PIECE_LEFT_TWISTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateLeft(2); }) },


    /* splitters */
    { PIECE_SPLITTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        if (delta == 0)
        {
          field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
          laser.rotateRight(2);
        }
        else
          laser.invalidate();
      })
    },

    { PIECE_ANGLED_SPLITTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        switch (delta) {
          case 0:
            field->generateBeam(laser.position, laser.rotatedDirection(-1), laser.color);
            laser.rotateRight(1);
            break;
          case -3: laser.rotateLeft(1); break;
          case 3: laser.rotateRight(1); break;
          default: laser.invalidate();
        }
      })
    },

    { PIECE_THREE_WAY_SPLITTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        if (delta == 0)
        {
          // first generate a second beam for left split
          field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
          field->generateBeam(laser.position, laser.rotatedDirection(2), laser.color);
        }
        else
          laser.invalidate();
      })
    },

    { PIECE_STAR_SPLITTER, PieceMechanics(false, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        for (int i = 0; i < 4; ++i)
          field->generateBeam(laser.position, laser.rotatedDirection(1 + i * 2), laser.color);

        laser.invalidate();
      })
    },


    { PIECE_PRISM, PieceMechanics(true, false, never(), prismMechanics(false)) },
    { PIECE_FLIPPED_PRISM, PieceMechanics(true, false, never(), prismMechanics(true)) },

    { PIECE_SELECTOR, PieceMechanics(true, true, [](const Piece* piece, const Laser&) { return piece->orientation() % 4 != 0; }, [](Field* field, const Piece* piece, Laser& laser)
      {
        int delta = piece->deltaDirection(laser);

        if (delta == 4)
          ;
        else if (delta == 0)
        {
          LaserColor deflected = static_cast<LaserColor>(laser.color & piece->color());
          LaserColor kept = static_cast<LaserColor>((laser.color & ~piece->color()) & LaserColor::WHITE);

          if (deflected != LaserColor::NONE)
            field->generateBeam(laser.position, laser.rotatedDirection(1), deflected);
          if (kept != LaserColor::NONE)
            field->generateBeam(laser.position, laser.direction, kept);

          laser.invalidate();
        }
      }
    )},

    { PIECE_SPLICER, PieceMechanics(true, true, [](const Piece* piece, const Laser&) { return piece->orientation() % 4 != 0; }, [](Field* field, const Piece* piece, Laser& laser)
      {
        const int delta = piece->deltaDirection(laser);

        if (delta == 4)
          ;
        else if (delta == 0)
        {
          LaserColor kept = static_cast<LaserColor>(laser.color & piece->color());
          LaserColor deflected = static_cast<LaserColor>((laser.color & ~piece->color()) & LaserColor::WHITE);

          if (deflected != LaserColor::NONE)
            field->generateBeam(laser.position, laser.rotatedDirection(1), deflected);
          if (kept != LaserColor::NONE)
            field->generateBeam(laser.position, laser.direction, kept);

          laser.invalidate();
        }
      }
    )},

    { PIECE_COLOR_SHIFTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        const int delta = piece->deltaDirection(laser);

        if (delta % 4 != 0)
          laser.invalidate();
        else
        {
          if (delta == 0)
            laser.color = static_cast<LaserColor>((laser.color >> 1) | ((laser.color & LaserColor::RED) << 2));
          else if (delta == 4)
            laser.color = static_cast<LaserColor>(((laser.color << 1) & LaserColor::WHITE) | ((laser.color & LaserColor::BLUE) >> 2));
        }
      })
    },

    { PIECE_COLOR_INVERTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser)
      {
        const int delta = piece->deltaDirection(laser);

        if (delta % 4 != 0 || laser.color == LaserColor::WHITE)
          laser.invalidate();
        else
          laser.color = static_cast<LaserColor>(~laser.color & LaserColor::WHITE);
      })
    },

    { PIECE_TNT, PieceMechanics(false, false, never(), [](Field* field, const Piece* piece, Laser& laser) { field->fail(); }) },

   };

  auto it = mechanics.find(type);

  return it != mechanics.end() ? &it->second : nullptr;
}

void Teleporter::receiveLaser(Field* field, Laser &laser)
{
  Direction dir = laser.direction;
  Position p = laser.position;

  p += dir;
  
  while (field->isInside(p))
  {
    Tile *tile = field->tileAt(p);
    
    if (tile->piece() && tile->piece()->type() == PIECE_TELEPORTER)
    {
      field->generateBeam(p, laser.direction, laser.color);
      break;
    }
    
    p += dir;
  }
  
  laser.invalidate();
}

Goal::Goal(PieceType type, LaserColor color) : Piece(type, NORTH, color), satisfied(false), satisfyDirection(0), satisfyColor(LaserColor::NONE) { }
