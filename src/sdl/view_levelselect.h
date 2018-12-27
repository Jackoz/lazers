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

struct SDL_Surface;

class View;

class LevelList : public OffsettableList<LevelSpec*>
{
  private:
    Game *game;
    
  public:
    LevelList(Game *game) : OffsettableList(14), game(game) { }
    
    u32 current() { return game->pack->selected; }
    u32 count() { return game->pack->count(); }
    void set(u32 i) { game->pack->selected = i; }
    LevelSpec* get(u32 i) { return game->pack->at(offset+i); }
};

class LevelSelectView : public View
{
private:
  SDL_Surface *preview;
  SDL_Surface *scaledPreview;
  Field *field;
  
  LevelList levelList;
  
public:
  LevelSelectView(Game *game);
  void handleEvent(SDL_Event &event);
  void draw();
  void rebuildPreview();
  
  void activate();
};

#endif
