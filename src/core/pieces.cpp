//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "pieces.h"

#include "level.h"

void DoubleSplitterMirror::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == -1 || delta == 3)
    field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
  else if (delta == 1 || delta == -3)
    field->generateBeam(laser.position, laser.rotatedDirection(2), laser.color);
  else
    laser.invalidate();
}

void Splitter::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    // first generate a second beam for left split
    field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
    laser.rotateRight(2);
  }
  else
    laser.invalidate();
}

void ThreeWaySplitter::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    // first generate a second beam for left split
    field->generateBeam(laser.position, laser.rotatedDirection(-2), laser.color);
    field->generateBeam(laser.position, laser.rotatedDirection(2), laser.color);
  }
  else
    laser.invalidate();
}

void StarSplitter::receiveLaser(Laser &laser)
{
  for (int i = 0; i < 4; ++i)
    field->generateBeam(laser.position, laser.rotatedDirection(1+i*2), laser.color);

  laser.invalidate();
}

void DSplitter::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
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

void Prism::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    if (laser.color & COLOR_RED)
      field->generateBeam(laser.position, laser.direction, COLOR_RED);
    if (laser.color & COLOR_GREEN)
      field->generateBeam(laser.position, laser.rotatedDirection(1), COLOR_GREEN);
    if (laser.color & COLOR_BLUE)
      field->generateBeam(laser.position, laser.rotatedDirection(2), COLOR_BLUE);

  }
  else if (delta == 4)
  {
    if (laser.color & COLOR_RED)
      field->generateBeam(laser.position, laser.direction, COLOR_RED);
  }
  else if (delta == -3)
  {
    if (laser.color & COLOR_GREEN)
      field->generateBeam(laser.position, laser.rotatedDirection(-1), COLOR_GREEN);
  }
  else if (delta == -2)
  {
    if (laser.color & COLOR_BLUE)
      field->generateBeam(laser.position, laser.rotatedDirection(-2), COLOR_BLUE);
  }
  
  laser.invalidate();
}

void FlippedPrism::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == 0)
  {
    if (laser.color & COLOR_RED)
      field->generateBeam(laser.position, laser.direction, COLOR_RED);
    if (laser.color & COLOR_GREEN)
      field->generateBeam(laser.position, laser.rotatedDirection(-1), COLOR_GREEN);
    if (laser.color & COLOR_BLUE)
      field->generateBeam(laser.position, laser.rotatedDirection(-2), COLOR_BLUE);
    
  }
  else if (delta == 4)
  {
    if (laser.color & COLOR_RED)
      field->generateBeam(laser.position, laser.direction, COLOR_RED);
  }
  else if (delta == 3)
  {
    if (laser.color & COLOR_GREEN)
      field->generateBeam(laser.position, laser.rotatedDirection(+1), COLOR_GREEN);
  }
  else if (delta == 2)
  {
    if (laser.color & COLOR_BLUE)
      field->generateBeam(laser.position, laser.rotatedDirection(+2), COLOR_BLUE);
  }
  
  laser.invalidate();
}

void Selector::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == 4)
    ;
  else if (delta == 0)
  {
    LaserColor deflected = static_cast<LaserColor>(laser.color & color());
    LaserColor kept = static_cast<LaserColor>((laser.color & ~color()) & COLOR_WHITE);

    if (deflected != COLOR_NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(1), deflected);
    if (kept != COLOR_NONE)      
      field->generateBeam(laser.position, laser.direction, kept);

    laser.invalidate();
  }
}

void Splicer::receiveLaser(Laser &laser)
{
  s8 delta = deltaDirection(laser);
  
  if (delta == 4)
    ;
  else if (delta == 0)
  {
    LaserColor kept = static_cast<LaserColor>(laser.color & color());
    LaserColor deflected = static_cast<LaserColor>((laser.color & ~color()) & COLOR_WHITE);
    
    if (deflected != COLOR_NONE)
      field->generateBeam(laser.position, laser.rotatedDirection(1), deflected);
    if (kept != COLOR_NONE)
      field->generateBeam(laser.position, laser.direction, kept);
    
    laser.invalidate();
  }
}


void Teleporter::receiveLaser(Laser &laser)
{
  Direction dir = laser.direction;
  Position p = laser.position;

  p += dir;
  
  while (p.isValid())
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
void TNT::receiveLaser(Laser &laser) { field->fail(); UNUSED(laser); }



Goal::Goal(PieceType type, LaserColor color, Field *field) : Piece(type, NORTH, color, field), satisfied(false), satisfyDirection(0), satisfyColor(COLOR_NONE)
{
  field->addGoal(this);
}
