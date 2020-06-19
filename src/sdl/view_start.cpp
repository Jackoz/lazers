//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "view_start.h"

#include "gfx.h"
#include "ui.h"

#include "SDL.h"

StartView::StartView(Game *game) : View(game)
{
  entries = { 
    { 
      "Play" 
    }, 
    { 
      "Help" 
    },
    { 
      "Options"
    },
    {
      "About" 
    },
    { 
      "Quit" 
    } 
  };
  selected = entries.end();
}

void StartView::activate()
{

}

void StartView::draw()
{
  Gfx::clear(BACKGROUND_COLOR);
  Gfx::drawString(Gfx::width() / 2, Gfx::height() * 0.15f, true, "^f00L^^a^ff0z^^e^0f0r^^s");

  int c = 0;
  for (const auto& entry : entries)
  {
    Gfx::drawString(Gfx::width() / 2, Gfx::height() * 0.4f + 10 * c, true, entry.action ? entry.caption : ("^bbb" + entry.caption));
    ++c;
  }
}

void StartView::handleEvent(SDL_Event &event)
{
  switch(event.type)
  {
    case SDL_MOUSEMOTION:
    {
   

      break;
    }
      
    case SDL_MOUSEBUTTONDOWN:
    {
    

      break;
    }

    case SDL_MOUSEWHEEL:
    {

      break;
    }
      
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

          break;
        }
        case KEY_UP:
        {

          break;
        }
          
        case KEY_L:
        {          

          break;
        }
          
        case KEY_R:
        {

          break;
        }
          
        case KEY_B:
        {

        }
          
        default: break;
      }
    }
  }
}
