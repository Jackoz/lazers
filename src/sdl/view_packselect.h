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

class PackList : public OffsettableList<const LevelPack*>
{
private:
  Game *game;
  
public:
  PackList(Game *game) : OffsettableList(14), game(game) { }
  
  size_t current() const { return Files::selectedPack; }
  size_t count() const { return game->packs.packCount(); }
  void set(size_t i) { Files::selectedPack = i; }
  const LevelPack* get(size_t i) const { return &game->packs[i]; }
};

class PackSelectList : public View
{
private:
  PackList levelList;
  
public:
  PackSelectList(Game *game);
  
  void handleMouseEvent(EventType type, int x, int y, int button) override;
  
  void handleEvent(SDL_Event &event);
  void draw();
  
  void activate();
};

#endif
