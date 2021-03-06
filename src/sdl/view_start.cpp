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
      "Play",
      [] (Game* game) { game->switchView(VIEW_PACK_SELECT); }
    }, 
    { 
      "Help",
      [] (Game* game) { game->switchView(VIEW_HELP); }
    },
    { 
      "Options"
    },
    {
      "About" 
    },
    { 
      "Quit",
      [] (Game* game) { game->quit(); }
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

  const int spacing = Gfx::stringHeight("") + 1;
  
  int c = 0;
  for (auto it = entries.begin(); it != entries.end(); ++it)
  {
    std::string prefix;
    
    if (!it->action)
      prefix = "^bbb";
    else if (it == selected)
      prefix = "^ff0";
    
    Gfx::drawString(Gfx::width() / 2, Gfx::height() * 0.4f + spacing * c, true, prefix + it->caption);
    ++c;
  }
}

void StartView::handleMouseEvent(EventType type, int x, int y, int button)
{
  const int base = Gfx::height() * 0.4f;
  const int spacing = Gfx::stringHeight("") + 1;
  int w = (y - base) / spacing;
  auto hover = entries.end();
  
  if (w >= 0 && w < entries.size() && x > Gfx::width()/2 - 20 && x < Gfx::width()/2 + 20)
  {
    hover = entries.begin() + w;
  }
  
  if (type == EventType::MOUSE_MOTION)
  {
    selected = hover;
  }
  else if (type == EventType::MOUSE_DOWN)
  {
    selected = hover;
    if (selected != entries.end() && selected->action)
      selected->action(game);
  }
}

void StartView::handleEvent(SDL_Event &event)
{
  switch(event.type)
  {
    case SDL_KEYDOWN:			// Button press
    {
      switch(event.key.keysym.sym)
      {
        case KEY_START:
        {
          break;
        }
          
        case KEY_SELECT:
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
