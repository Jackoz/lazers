//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _VIEW_LEVEL_SELECT_H_
#define _VIEW_LEVEL_SELECT_H_

#include "game.h"

class View;

class LevelSelectView : public View
{
private:
  u32 offset;
  
public:
  LevelSelectView(Game *game) : View(game), offset(0) { }
  void handleEvent(SDL_Event &event);
  void draw();
};

#endif
