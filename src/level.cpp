//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "level.h"

#include "gfx.h"

using namespace std;

Piece* Field::generatePiece(const PieceInfo *info)
{
  if (info->spec)
  {
  switch (info->spec->type)
  {
    case PIECE_MIRROR: return new Mirror(info->direction, this);
    case PIECE_DOUBLE_PASS_MIRROR: return new DoublePassMirror(info->direction, this);
    case PIECE_STRICT_GOAL: return new StrictGoal(info->color, this);
    case PIECE_SOURCE: return new LaserSource(info->direction, info->color, this);
    case PIECE_SPLITTER: return new Splitter(info->direction, this);
    case PIECE_DSPLITTER: return new DSplitter(info->direction, this);
    case PIECE_REFRACTOR: return new Refractor(info->direction, this);
    case PIECE_FILTER: return new Filter(info->color, this);
    case PIECE_WALL: return new Wall(this);
    case PIECE_GLASS: return new Glass(this);
    case PIECE_COLOR_SHIFTER: return new ColorShifter(info->direction, this);
    case PIECE_PRISM: return new Prism(info->direction, this);
    case PIECE_FLIPPED_PRISM: return new FlippedPrism(info->direction, this);
    case PIECE_TUNNEL: return new Tunnel(info->direction, this);
    case PIECE_DOUBLE_MIRROR: return new DoubleMirror(info->direction, this);
    case PIECE_POLARIZER: return new Polarizer(info->direction, info->color, this);
    case PIECE_SELECTOR: return new Selector(info->direction, info->color, this);
    case PIECE_SPLICER: return new Splicer(info->direction, info->color, this);
    case PIECE_BENDER: return new Bender(this);
    case PIECE_THREE_WAY_SPLITTER: return new ThreeWaySplitter(info->direction, this);

      
    case PIECE_MINE: return new Mine(this);
    case PIECE_TNT: return new TNT(this);
    case PIECE_SLIME: return new Slime(this);
    // TODO: finire
    default: return nullptr;
  }
  }
  return nullptr;
}

void Field::load(LevelSpec* level)
{
  this->level = level;
  
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
        place(FIELD_WIDTH + curInvSlot%INVENTORY_WIDTH, curInvSlot/INVENTORY_WIDTH, piece);
        ++curInvSlot;
      }
      else
        place(info->x, info->y, piece);
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
  Laser beam = Laser(position.derived(direction), direction, color);
  
  if (beam.isValid())
  {
    lasers.push_back(beam);
    
    Tile *tile = tileAt(position.x, position.y);
    
    if (!tile->piece() || tile->piece()->type() != PIECE_SOURCE)
      tileAt(position.x, position.y)->colors[direction] |= color;
  }
}

void Field::updateLasers()
{
  beams.clear();
  
  for (auto g : goals)
    g->reset();
  
  for (int i = 0; i < FIELD_WIDTH; ++i)
    for (int j = 0; j < FIELD_HEIGHT; ++j)
    {
      Tile *tile = tileAt(i,j);
      tile->resetLasers();
      Piece *piece = tile->piece();
      
      if (piece && piece->produceLaser())
      {
        generateBeam(Position(i,j), piece->rotation(), piece->color());
        //Laser laser = Laser(Position(i+Position::directions[piece->rotation()][0],j+Position::directions[piece->rotation()][1]), piece->rotation(), piece->color());
        //lasers.push_back(laser);
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
      
      Piece *piece = tile->piece();
      
      if (piece && piece->blocksLaser(laser))
        break;
           
      // place first half of laser if piece doesn't block it
      tile->colors[(laser.direction+4)%8] |= laser.color;

      // update existing laser or add new lasers according to piece behavior
      if (piece)
        piece->receiveLaser(laser);
      
      // keep drawing the other laser if receiveLaser didn't invalidate it
      if (laser.isValid())
      {
        tile->colors[laser.direction] |= laser.color;
        laser.position.shift(laser.direction);
      }
    }
    
    it = lasers.erase(it);
  }
  
  if (checkForWin() && !level->solved)
  {
    level->solved = true;
    ++Files::packAt(Files::selectedPack)->solvedCount;
  }
}

bool Field::checkForWin()
{
  return std::all_of(goals.begin(), goals.end(), [](const Goal *goal){ return goal->isSatisfied(); });
}




