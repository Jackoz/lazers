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
  Piece* heldPiece; //TODO: convert to std::unique_ptr
  Tile *selectedTile;
  Position fposition, iposition;
  Position *position;
  
  static u16 coordX(u16 x, bool isInventory);
  static u16 coordY(u16 y);
  
  
  
  static SDL_Rect rectForPiece(Piece* piece);
  
  Field* field() { return game->field; }
  
  Position coordToPosition(int x, int y);
  
public:
  LevelView(Game *game) : View(game), selectedTile(nullptr), fposition(Position(0,0)), iposition(Position(FIELD_WIDTH,0)), position(&fposition), heldPiece(nullptr) { }
  void handleEvent(SDL_Event &event);
  void draw();
  void activate();
  
  static void drawField(Field *field, SDL_Surface *screen, u16 bx, u16 by);
  static void drawGrid(Field *field, int x, int y, int w, int h, SDL_Surface *screen);
  static void drawInventory(Field *field, SDL_Surface *screen, u16 bx, u16 by);
};

#endif
