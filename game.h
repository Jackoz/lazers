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
};

class LevelView : public View
{
  private:
    Field field;
    Tile *selectedTile;
    Position fposition, iposition;
    Position *position;
  
    static u16 coordX(u16 x, bool isInventory) {
      if (!isInventory || x < FIELD_WIDTH)
        return TILE_SIZE*x + GFX_FIELD_POS_X;
      else
        return TILE_SIZE*(x-FIELD_WIDTH) + GFX_INVENTORY_POS_X;
    }
    static u16 coordY(u16 y) { return TILE_SIZE*y + GFX_FIELD_POS_Y; }
  
    void drawField();
  
  public:
    LevelView(Game *game) : View(game), selectedTile(nullptr), fposition(Position(0,0)), iposition(Position(FIELD_WIDTH,0)), position(&fposition) { }
    void handleEvent(SDL_Event &event);
    void draw();
};


class Game
{
  private:
    bool running;
    View* views[VIEWS_COUNT];
    View* view;
    View* overView;
  
    void handleEvents();

  public:
    Game() : running(true), views{new LevelView(this)}, view(views[0]), overView(nullptr) { }
    void init();
    void loop();
    void quit() { running = false; }
};

#endif
