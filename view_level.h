//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _VIEW_LEVEL_H_
#define _VIEW_LEVEL_H_

#include "game.h"

class View;

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
  
public:
  LevelView(Game *game) : View(game), selectedTile(nullptr), fposition(Position(0,0)), iposition(Position(FIELD_WIDTH,0)), position(&fposition) { }
  void handleEvent(SDL_Event &event);
  void draw();
  
  static void drawField(Field &field, SDL_Surface *screen, u16 bx, u16 by);
  static void drawGrid(int x, int y, int w, int h, SDL_Surface *screen);
  void drawInventory();
};

#endif
