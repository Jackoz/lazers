//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "game.h"

#include "gfx.h"
#include "view_level.h"
#include "view_levelselect.h"
#include "view_packselect.h"
#include "view_help.h"
#include "view_start.h"

#include "files/aargon.h"

Game::Game() : field(new Field(16, 11, 4, 11)), running(true), views{new LevelView(this), new LevelSelectView(this), new PackSelectList(this), new HelpView(this), new StartView(this)}, view(views[0]), overView(nullptr)
{

}

void Game::init()
{
  Gfx::init();
  running = true;  

  
  auto apacks = Aargon::parseLevels();
  packs.move(apacks);
  apacks.clear();
  
  /*for (const auto& pack : packs)
    Files::savePack(pack);*/
  
  /*auto lpacks = Files::loadPacks();
  this->packs.move(lpacks);*/
  
  //Files::loadSolvedStatus();
  
  field->generateDummy();

  if (packs.packCount() > 0)
    pack = &packs[0];

  view = views[VIEW_START];
  
  view->activate();
}

void Game::handleEvents()
{
  SDL_Event event;
  
  auto view = overView ? overView : this->view;
  
  while (SDL_PollEvent(&event))
  {
    switch (event.type) {
      case SDL_QUIT: quit(); break;
        
      case SDL_MOUSEMOTION:
      {
        view->handleMouseEvent(EventType::MOUSE_MOTION, event.motion.x / SCALE, event.motion.y / SCALE, SDL_BUTTON_LEFT);
        break;
      }
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
      {
        auto type = event.type == SDL_MOUSEBUTTONUP ? EventType::MOUSE_UP : EventType::MOUSE_DOWN;
        view->handleMouseEvent(type, event.button.x / SCALE, event.button.y / SCALE, event.button.button);
      }
        
      default:
      {
        view->handleEvent(event);
        break;
      }
    }
  }
}

void Game::switchView(ViewType type)
{
  view->deactivate();
  view = views[type];
  view->activate();
}



void Game::loop()
{
  while (running)
  {
    SDL_RenderClear(Gfx::renderer);
    view->draw();
    SDL_RenderPresent(Gfx::renderer);

    if (overView) overView->draw();
    
    handleEvents();
  }
}
