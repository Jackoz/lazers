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

Game::Game() : running(true), views{new LevelView(this), new LevelSelectView(this)}, view(views[1]), overView(nullptr)
{
  
}

void Game::init()
{
  Gfx::init();
  running = true;
  view->activate();
  
  const char *input = "foobar testaaa";
  char *encoded, *decoded;
  size_t encodedLength, decodedLength;
  Files::encode(reinterpret_cast<const u8*>(input), strlen(input), &encoded, &encodedLength);
  Files::decode(encoded, encodedLength, reinterpret_cast<u8**>(&decoded), &decodedLength);
  printf("  INPUT: %s (%d)\n",input, (u32)strlen(input));
  printf("ENCODED: %.*s (%d)\n",(u32)encodedLength, encoded, (u32)encodedLength);
  printf("DECODED: %.*s (%d)\n",(u32)decodedLength, decoded, (u32)decodedLength);
  
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