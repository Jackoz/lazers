//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//
#pragma once

#include "game.h"

#include <vector>
#include <functional>

class StartView : public View
{
private:
  struct MenuEntry
  {
    std::string caption;
    std::function<void(void)> action;
  };

  std::vector<MenuEntry> entries;
  decltype(entries)::const_iterator selected;

public:
  StartView(Game *game);
  void handleEvent(SDL_Event &event) override;
  void draw() override;
  
  void activate() override;
};
