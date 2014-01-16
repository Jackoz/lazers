//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _GAME_H_
#define _GAME_H_

#include "common.h"
#include "level.h"

enum ViewType
{
  VIEW_LEVEL = 0,
  VIEW_LEVEL_SELECT = 1,
  
  VIEWS_COUNT
};

class View
{
  protected:
    Game *game;
  
  public:
    View(Game *game) : game(game) { }
    virtual void handleEvent(SDL_Event &event) = 0;
    virtual void draw() = 0;
  
    virtual void activate() { };
    virtual void deactivate() { };
};




class Game
{
  public:
    Field *field;

  private:
    bool running;
    View* views[VIEWS_COUNT];
    View* view;
    View* overView;
  
    void handleEvents();

  public:
    Game();
    void init();
    void loop();
    void quit() { running = false; }
  
    void switchView(ViewType type);
  
    LevelPack *pack;
    
};

#endif
