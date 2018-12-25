//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "view_packselect.h"

#include "view_level.h"
#include "gfx.h"
#include "SDL.h"

const u16 LIST_X = 20;
const u16 LIST_Y = 30;
const u16 LIST_DY = 10;
const u16 LIST_SIZE = 14;






PackSelectList::PackSelectList(Game *game) : View(game), levelList(PackList(game))
{

}

void PackSelectList::activate()
{

}

void PackSelectList::draw()
{
  Gfx::clear(BACKGROUND_COLOR);
  
  Gfx::drawString(20, 10, false, "CHOOSE A LEVEL PACK");

  Gfx::drawString(20, 220, false, "B: enter pack    \x1F\x1E: choose pack    A: back", game->pack->name.c_str(), game->pack->author.c_str());

  for (int i = 0; levelList.hasNext(i); ++i)
  {
    LevelPack *spec = levelList.get(i);
    
    Gfx::drawString(LIST_X, LIST_Y+LIST_DY*i, false, "%s - %d of %d", spec->name.c_str(), spec->solvedCount, spec->count());
    
    if (levelList.isSelected(i))
      Gfx::blit(Gfx::ui, 0, 0, 4, 7, LIST_X-8, LIST_Y+LIST_DY*i);
  }
  
  Gfx::drawString(LIST_X+30, LIST_Y+LIST_DY*levelList.LIST_SIZE+10, false, "%d of %d", levelList.current()+1, levelList.count());

  /*Gfx::lock();
  Gfx::rect(150, 30, 160, 150, Gfx::ccc(180, 0, 0));
  Gfx::unlock();*/
  
  Gfx::postDraw();
}

void PackSelectList::handleEvent(SDL_Event &event)
{
  switch(event.type)
  {
    case SDL_KEYDOWN:			// Button press
    {
      switch(event.key.keysym.sym)
      {
        case KEY_START:
        {
          game->quit();
          break;
        }
          
        case KEY_DOWN:
        {
          if (levelList.down())
            ;
          break;
        }
        case KEY_UP:
        {
          if (levelList.up())
            ;
          break;
        }
          
        case KEY_L:
        {          
          if (levelList.prevPage())
            ;
          break;
        }
          
        case KEY_R:
        {
          if (levelList.nextPage())
            ;
          break;
        }
          
        case KEY_B:
        {
          game->pack = Files::packAt(Files::selectedPack);
          game->switchView(VIEW_LEVEL_SELECT);
        }
          
        default: break;
      }
    }
  }
}
