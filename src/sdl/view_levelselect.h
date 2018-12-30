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

class LevelList : public OffsettableList<const LevelSpec*>
{
  private:
    Game *game;
    
  public:
    LevelList(Game *game) : OffsettableList(14), game(game) { }
    
    size_t current() const { return game->pack->selected; }
    size_t count() const { return game->pack->count(); }
    void set(size_t i) { game->pack->selected = i; }
    const LevelSpec* get(size_t i) const { return game->pack->at(offset+i); }
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
