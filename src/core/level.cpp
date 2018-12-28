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

Piece* Field::generatePiece(const PieceInfo *info)
{
  if (info->spec)
  {
  switch (info->spec->type)
  {
    case PIECE_MIRROR: return new Mirror(info->direction);
    case PIECE_DOUBLE_PASS_MIRROR: return new DoublePassMirror(info->direction);
    case PIECE_STRICT_GOAL:
    {
      Goal* goal = new StrictGoal(info->color);
      addGoal(goal);
      return goal;
    }
    case PIECE_SOURCE: return new LaserSource(info->direction, info->color);
    case PIECE_SPLITTER: return new Splitter(info->direction);
    case PIECE_DSPLITTER: return new DSplitter(info->direction);
    case PIECE_REFRACTOR: return new Refractor(info->direction);
    case PIECE_FILTER: return new Filter(info->color);
    case PIECE_WALL: return new Wall();
    case PIECE_GLASS: return new Glass();
    case PIECE_COLOR_SHIFTER: return new ColorShifter(info->direction);
    case PIECE_PRISM: return new Prism(info->direction);
    case PIECE_FLIPPED_PRISM: return new FlippedPrism(info->direction);
    case PIECE_TUNNEL: return new Tunnel(info->direction);
    case PIECE_DOUBLE_MIRROR: return new DoubleMirror(info->direction);
    case PIECE_POLARIZER: return new Polarizer(info->direction, info->color);
    case PIECE_SELECTOR: return new Selector(info->direction, info->color);
    case PIECE_SPLICER: return new Splicer(info->direction, info->color);
    case PIECE_BENDER: return new Bender();
    case PIECE_THREE_WAY_SPLITTER: return new ThreeWaySplitter(info->direction);

      
    case PIECE_MINE: return new Mine();
    case PIECE_TNT: return new TNT();
    case PIECE_SLIME: return new Slime();
    // TODO: finire
    default: return nullptr;
  }
  }
  return nullptr;
}

void Field::load(LevelSpec* level)
{
  this->_level = level;
  
  u8 curInvSlot = 0;
  
  for (u32 i = 0; i < level->count(); ++i)
  {
    const PieceInfo *info = level->at(i);
    
    Piece *piece = generatePiece(info);
    
    if (piece)
    {
      //if (info->spec->canBeRotated)
      piece->setCanBeRotated(info->roteable);
      piece->setCanBeMoved(info->moveable);
      
      if (info->inventory)
      {
        place(Position(Position::Type::INVENTORY, curInvSlot%_invWidth, curInvSlot/_invWidth), piece);
        ++curInvSlot;
      }
      else
        place(Position(info->x, info->y), piece);
    }
    else
    {
      printf("Missing allocation: %c (%s)\n",info->spec->mapping,level->name.c_str());
    }
  }
  
  updateLasers();
}

void Field::generateBeam(Position position, Direction direction, LaserColor color)
{
  Laser beam = Laser(position + direction, direction, color);
  
  if (beam.isValid())
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
  
  for (auto g : goals)
    g->reset();
  
  for (int i = 0; i < _width; ++i)
    for (int j = 0; j < _height; ++j)
    {
      Tile *tile = tileAt(Position(i, j));
      tile->resetLasers();
      const auto& piece = tile->piece();
      
      if (piece)
      {
        Laser laser = piece->produceLaser();
        
        if (laser.color != COLOR_NONE)
          generateBeam(laser.position + Position(i, j), laser.direction, laser.color);

      }
    }
  
  list<Laser>::iterator it = lasers.begin();
  while (!lasers.empty())
  {
    Laser& laser = (*it);

    while (laser.isValid())
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
      if (laser.isValid())
      {
        tile->colors[laser.direction] |= laser.color;
        laser.advance();
      }
    }
    
    it = lasers.erase(it);
  }
  
  if (checkForWin() && _level && !_level->solved)
  {
    _level->solved = true;
    //TODO: non-sense here, need to be managed somewhere else
    //++Files::packAt(Files::selectedPack)->solvedCount;
  }
}

bool Field::checkForWin()
{
  return std::all_of(goals.begin(), goals.end(), [](const Goal *goal){ return goal->isSatisfied(); });
}




