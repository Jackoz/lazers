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



void Field::generateBeam(Position position, Direction direction, LaserColor color)
{
  Laser beam = Laser(position.derived(direction), direction, color);
  
  if (beam.isValid())
  {
    lasers.push_back(beam);
    tileAt(position.x, position.y)->colors[direction] |= color;
  }
}

void Field::updateLasers()
{
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
        Laser laser = Laser(Position(i+Position::directions[piece->rotation()][0],j+Position::directions[piece->rotation()][1]), piece->rotation(), piece->color());
        lasers.push_back(laser);
      }
    }
  
  list<Laser>::iterator it = lasers.begin();
  while (!lasers.empty())
  {
    Laser& laser = (*it);
    
    while (laser.isValid())
    {
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
}

bool Field::checkForWin()
{
  return std::all_of(goals.begin(), goals.end(), [](const Goal *goal){ return goal->isSatisfied(); });
}




