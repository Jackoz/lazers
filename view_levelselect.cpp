//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "view_levelselect.h"

#include "gfx.h"
#include <SDL/SDL.h>

const u16 LIST_X = 20;
const u16 LIST_Y = 30;
const u16 LIST_DY = 10;
const u16 LIST_SIZE = 4;

void LevelSelectView::draw()
{
  Gfx::clear(BACKGROUND_COLOR);
  
  Gfx::drawString(20, 10, "Pack name: %s (%s)", game->pack->name.c_str(), game->pack->author.c_str());
  
  for (int i = 0; i < LIST_SIZE && i + offset < game->pack->count(); ++i)
  {
    LevelSpec *spec = game->pack->at(offset+i);
    
    Gfx::drawString(LIST_X, LIST_Y+LIST_DY*i, spec->name.c_str());
    
    if (i+offset == game->pack->selected)
      Gfx::blit(Gfx::ui, 0, 0, 4, 7, LIST_X-8, LIST_Y+LIST_DY*i);
  }
  

  Gfx::lock();
  Gfx::rect(150, 30, 160, 150, Gfx::ccc(180, 0, 0));
  Gfx::unlock();
  
  Gfx::postDraw();
}

void LevelSelectView::handleEvent(SDL_Event &event)
{
  u32 minOffset = offset;
  u32 maxOffset = offset + LIST_SIZE - 1;
  
  switch(event.type)
  {
    case SDL_KEYDOWN:			// Button press
    {
      switch(event.key.keysym.sym)
      {
        case KEY_DOWN:
        {
          if (game->pack->selected < game->pack->count() - 1)
          {
            ++game->pack->selected;
            
            if (game->pack->selected > maxOffset)
              ++offset;
          }
          break;
        }
        case KEY_UP:
        {
          if (game->pack->selected > 0)
          {
            --game->pack->selected;
            
            if (game->pack->selected < minOffset)
              --offset;
          }
          break;
        }
          
        case KEY_L:
        {          
          if (game->pack->selected > minOffset)
            game->pack->selected = minOffset;
          else
          {
            if (offset > 0)
            {
              u8 delta = LIST_SIZE <= offset ? LIST_SIZE : offset;
              offset -= delta;
              game->pack->selected -= delta;
            }
          }
          break;
        }
          
        default: break;
      }
    }
  }
}