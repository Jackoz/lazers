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

Game::Game() : running(true), views{new LevelView(this), new LevelSelectView(this)}, view(views[1]), overView(nullptr) { }

void Game::init()
{
  const char* test = "any carnal pleasure.";
  char *output = nullptr;
  size_t length;
  Files::encode(test, strlen(test), &output, &length);
  printf("\n%*s\n",static_cast<int>(length),output);
  
  Gfx::init();
  running = true;
  
  pack = Files::packAt(0);
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



void Game::loop()
{
  while (running)
  {
    view->draw();
    if (overView) overView->draw();
    
    handleEvents();
  }
}