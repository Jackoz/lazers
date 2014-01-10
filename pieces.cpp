//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "pieces.h"

#include "lazers.h"

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