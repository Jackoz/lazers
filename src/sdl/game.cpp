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

#include "files/aargon.h"

Game::Game() : field(new Field(16, 11, 4, 11)), running(true), views{new LevelView(this), new LevelSelectView(this), new PackSelectList(this), new HelpView(this)}, view(views[0]), overView(nullptr)
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

  view = views[0];
  
  view->activate();
}

void Game::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    switch (event.type) {
      case SDL_QUIT: quit(); break;
      default:
      {
        if (overView) overView->handleEvent(event);
        else view->handleEvent(event);
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
    view->draw();
    if (overView) overView->draw();
    
    handleEvents();
  }
}
