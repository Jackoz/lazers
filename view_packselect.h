//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _VIEW_PACK_SELECT_H_
#define _VIEW_PACK_SELECT_H_

#include "game.h"

class View;

class PackList : public OffsettableList<LevelPack*>
{
  private:
    Game *game;
    
  public:
    PackList(Game *game) : OffsettableList(14), game(game) { }
    
    u32 current() { return Files::selectedPack; }
    u32 count() { return Files::packCount(); }
    void set(u32 i) { Files::selectedPack = i; }
    LevelPack* get(u32 i) { return Files::packAt(i); }
};

class PackSelectList : public View
{
private:
  PackList levelList;
  
public:
  PackSelectList(Game *game);
  void handleEvent(SDL_Event &event);
  void draw();
  
  void activate();
};

#endif
