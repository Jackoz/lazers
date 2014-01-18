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
  Tile *selectedTile;
  Position fposition, iposition;
  Position *position;
  Field *field;
  
  static u16 coordX(u16 x, bool isInventory) {
    if (!isInventory || x < FIELD_WIDTH)
      return TILE_SIZE*x + GFX_FIELD_POS_X;
    else
      return TILE_SIZE*(x-FIELD_WIDTH) + GFX_INVENTORY_POS_X;
  }
  static u16 coordY(u16 y) { return TILE_SIZE*y + GFX_FIELD_POS_Y; }
  
public:
  LevelView(Game *game) : View(game), selectedTile(nullptr), fposition(Position(0,0)), iposition(Position(FIELD_WIDTH,0)), position(&fposition), field(game->field) { }
  void handleEvent(SDL_Event &event);
  void draw();
  void activate();
  
  static void drawField(Field *field, SDL_Surface *screen, u16 bx, u16 by);
  static void drawGrid(Field *field, int x, int y, int w, int h, SDL_Surface *screen);
  static void drawInventory(Field *field, SDL_Surface *screen, u16 bx, u16 by);
};

#endif
