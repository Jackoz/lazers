//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "level.h"

#include <algorithm>

using namespace std;

Piece* Field::generatePiece(const PieceInfo& info)
{
  switch (info.type)
  {
    case PIECE_WALL: 
    case PIECE_SOURCE:

    case PIECE_MIRROR:
    case PIECE_DOUBLE_MIRROR:
    case PIECE_DOUBLE_PASS_MIRROR:
    case PIECE_DOUBLE_SKEW_MIRROR:
    case PIECE_SKEW_MIRROR:
    case PIECE_DOUBLE_SPLITTER_MIRROR:
    case PIECE_REFRACTOR:

    case PIECE_SPLITTER:
    case PIECE_ANGLED_SPLITTER:
    case PIECE_THREE_WAY_SPLITTER:
    case PIECE_STAR_SPLITTER:
    case PIECE_PRISM:
    case PIECE_FLIPPED_PRISM:

    case PIECE_GLASS:
    case PIECE_FILTER:
    case PIECE_POLARIZER:
    case PIECE_TUNNEL:

    case PIECE_RIGHT_BENDER:
    case PIECE_RIGHT_TWISTER:
    case PIECE_LEFT_BENDER:
    case PIECE_LEFT_TWISTER:

    case PIECE_SELECTOR:
    case PIECE_SPLICER:
    case PIECE_COLOR_SHIFTER:
    case PIECE_COLOR_INVERTER:

    case PIECE_TNT:

      return new Piece(info.type, info.direction, info.color);

    //case PIECE_ROUND_FILTER: return new RoundFilter(); //TODO: why empty constructor?

    case PIECE_STRICT_GOAL:
    {
      Goal* goal = new StrictGoal(info.color);
      goals.push_back(std::unique_ptr<Goal>(goal));
      return goal;
    }

  
      
    case PIECE_MINE: return new Mine();
    case PIECE_SLIME: return new Slime();
    // TODO: finire
    default:
      assert(false);
      return nullptr;
  }
  
  return nullptr;
}

void Field::load(const LevelSpec* level)
{
  printf("Loading %s\n", level->name.c_str());
  
  this->_level = level;
  
  u32 curInvSlot = 0;
  
  for (size_t i = 0; i < level->count(); ++i)
  {
    const PieceInfo& info = level->at(i);
    
    Piece* piece = generatePiece(info);
    
    if (piece)
    {
      //if (info->spec->canBeRotated)
      piece->setCanBeRotated(info.roteable);
      piece->setCanBeMoved(info.moveable);
      
      if (info.inventory)
      {
        place(Position(Position::Type::INVENTORY, curInvSlot%_invWidth, curInvSlot/_invWidth), piece);
        ++curInvSlot;
      }
      else
        place(Position(info.x, info.y), piece);
    }
    else
    {
      printf("Missing allocation: %c (%s)\n", info.type, level->name.c_str());
    }
  }
  
  updateLasers();
}

void Field::generateBeam(Position position, Direction direction, LaserColor color)
{
  Laser beam = Laser(position + direction, direction, color);
  
  if (isInside(beam.position))
  {
    lasers.push_back(beam);
    
    Tile *tile = tileAt(position);
    
    if (!tile->piece() || tile->piece()->type() != PIECE_SOURCE)
      tileAt(position)->colors[direction] |= color;
  }
}

void Field::updateLasers()
{
  beams.clear();

  std::for_each(goals.begin(), goals.end(), [](auto& g) { g->reset(); });

  for (int i = 0; i < _width; ++i)
    for (int j = 0; j < _height; ++j)
    {
      Tile *tile = tileAt(Position(i, j));
      tile->resetLasers();
      const auto& piece = tile->piece();
      
      if (piece)
      {
        Laser laser = piece->produceLaser();
        
        if (laser.color != LaserColor::NONE)
          generateBeam(laser.position + Position(i, j), laser.direction, laser.color);

      }
    }
  
  list<Laser>::iterator it = lasers.begin();
  while (!lasers.empty())
  {
    Laser& laser = (*it);

    while (isInside(laser.position))
    {
      if (beams.find(laser) != beams.end())
        break;
      
      beams.insert(laser);

      Tile *tile = tileAt(laser.position);
      
      const auto& piece = tile->piece();
      
      if (piece && piece->blocksLaser(laser))
        break;
           
      // place first half of laser if piece doesn't block it
      tile->colors[(laser.direction+4)%8] |= laser.color;

      // update existing laser or add new lasers according to piece behavior
      if (piece)
        piece->receiveLaser(this, laser);
      
      // keep drawing the other laser if receiveLaser didn't invalidate it
      if (isInside(laser.position))
      {
        tile->colors[laser.direction] |= laser.color;
        laser.advance();
      }
    }
    
    it = lasers.erase(it);
  }
}

void Field::checkForWin()
{
  won = std::all_of(goals.begin(), goals.end(), [](const auto& goal) { return goal->isSatisfied(); });
}

void Field::generateDummy()
{
  reset();

  static const PieceType pieces[] = { 
    PIECE_WALL, PIECE_SOURCE,
    PIECE_MIRROR, PIECE_DOUBLE_MIRROR, PIECE_DOUBLE_PASS_MIRROR, PIECE_SKEW_MIRROR, PIECE_DOUBLE_SKEW_MIRROR, PIECE_DOUBLE_SPLITTER_MIRROR,
    PIECE_REFRACTOR, PIECE_SPLITTER, PIECE_ANGLED_SPLITTER, PIECE_THREE_WAY_SPLITTER, PIECE_STAR_SPLITTER, PIECE_PRISM, PIECE_FLIPPED_PRISM,
    PIECE_GLASS, PIECE_FILTER, PIECE_POLARIZER, PIECE_TUNNEL,
    PIECE_RIGHT_BENDER, PIECE_LEFT_BENDER, PIECE_RIGHT_TWISTER, PIECE_LEFT_TWISTER,
    PIECE_SELECTOR, PIECE_SPLICER, PIECE_COLOR_SHIFTER, PIECE_COLOR_INVERTER,
    PIECE_TNT
  };

  for (size_t i = 0; i < sizeof(pieces) / sizeof(pieces[0]); ++i)
  {
    Pos base = Pos(Pos::Type::INVENTORY, i % invWidth(), i / invWidth());
    const auto* mechanics = PieceMechanics::mechanicsForType(pieces[i]);


    PieceInfo info = PieceInfo(pieces[i]);
    info.inventory = true;
    info.moveable = true;
    info.roteable = true;
    info.color = mechanics->canBeColored() ? LaserColor::RED : LaserColor::NONE;
    info.direction = Dir::NORTH;
    Piece* piece = generatePiece(info);

    piece->setCanBeMoved(true);
    piece->setCanBeColored(true);
    piece->setInfinite(true);

    place(base, piece);

  }

}
