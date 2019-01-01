//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "pieces.h"

#include "level.h"

const PieceMechanics* PieceMechanics::mechanicsForType(PieceType type)
{
  static const std::unordered_map<PieceType, PieceMechanics> mechanics = {
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

    { PIECE_GLASS, PieceMechanics(false, false, never(), emptyMechanics(), emptyGenerator()) },


    { PIECE_RIGHT_BENDER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateRight(1); }) },
    { PIECE_LEFT_BENDER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateLeft(1); }) },
    { PIECE_RIGHT_TWISTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateRight(2); }) },
    { PIECE_LEFT_TWISTER, PieceMechanics(true, false, never(), [](Field* field, const Piece* piece, Laser& laser) { laser.rotateLeft(2); }) },

    { PIECE_SELECTOR, PieceMechanics(true, true, [](const Piece* piece) { return piece->orientation() % 4 != 0; }, [](Field* field, const Piece* piece, Laser& laser)
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

    { PIECE_SPLICER, PieceMechanics(true, true, [](const Piece* piece) { return piece->orientation() % 4 != 0; }, [](Field* field, const Piece* piece, Laser& laser)
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
    )}
   };

  auto it = mechanics.find(type);

  return it != mechanics.end() ? &it->second : nullptr;
}

void Splitter::receiveLaser(Field* field, Laser &laser)
{
  int delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    // first generate a second beam for left split
    field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
    laser.rotateRight(2);
  }
  else
    laser.invalidate();
}

void ThreeWaySplitter::receiveLaser(Field* field, Laser &laser)
{
  int delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    // first generate a second beam for left split
    field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
    field->generateBeam(laser.position, laser.rotatedDirection(2), laser.color);
  }
  else
    laser.invalidate();
}

void StarSplitter::receiveLaser(Field* field, Laser &laser)
{
  for (int i = 0; i < 4; ++i)
    field->generateBeam(laser.position, laser.rotatedDirection(1+i*2), laser.color);

  laser.invalidate();
}

void DSplitter::receiveLaser(Field* field, Laser &laser)
{
  int delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    // first generate a second beam for left split
    field->generateBeam(laser.position, laser.rotatedDirection(-1), laser.color);
    laser.rotateRight(1);
  }
  else if (delta == -3)
  {
    laser.rotateLeft(1);
  }
  else if (delta == 3)
  {
    laser.rotateRight(1);
  }
  else
    laser.invalidate();
}

void Prism::receiveLaser(Field* field, Laser &laser)
{
  int delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    if ((laser.color & LaserColor::RED) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.direction, LaserColor::RED);
    if ((laser.color & LaserColor::GREEN) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(1), LaserColor::GREEN);
    if ((laser.color & LaserColor::BLUE) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(2), LaserColor::BLUE);

  }
  else if (delta == 4)
  {
    if ((laser.color & LaserColor::RED) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.direction, LaserColor::RED);
  }
  else if (delta == -3)
  {
    if ((laser.color & LaserColor::GREEN) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(-1), LaserColor::GREEN);
  }
  else if (delta == -2)
  {
    if ((laser.color & LaserColor::BLUE) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(-2), LaserColor::BLUE);
  }
  
  laser.invalidate();
}

void FlippedPrism::receiveLaser(Field* field, Laser &laser)
{
  int delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    if ((laser.color & LaserColor::RED) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.direction, LaserColor::RED);
    if ((laser.color & LaserColor::GREEN) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(-1), LaserColor::GREEN);
    if ((laser.color & LaserColor::BLUE) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(-2), LaserColor::BLUE);
    
  }
  else if (delta == 4)
  {
    if ((laser.color & LaserColor::RED) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.direction, LaserColor::RED);
  }
  else if (delta == 3)
  {
    if ((laser.color & LaserColor::GREEN) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(+1), LaserColor::GREEN);
  }
  else if (delta == 2)
  {
    if ((laser.color & LaserColor::BLUE) != LaserColor::NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(+2), LaserColor::BLUE);
  }
  
  laser.invalidate();
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

//TODO: check?
void TNT::receiveLaser(Field* field, Laser &laser) { field->fail(); UNUSED(laser); }



Goal::Goal(PieceType type, LaserColor color) : Piece(type, NORTH, color), satisfied(false), satisfyDirection(0), satisfyColor(LaserColor::NONE) { }
