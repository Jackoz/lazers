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

struct SDL_Rect;
struct SDL_Texture;

class LevelView : public View
{
private:
  std::unique_ptr<Piece> heldPiece;
  Tile *selectedTile;
  Position fposition, iposition;
  Position *position;
  
  u16 coordX(const Position& p);
  u16 coordY(const Position& p);
  static SDL_Rect rectForPiece(const Piece* piece);
  
  Field* field() { return game->field; }
  
  Position coordToPosition(int x, int y);
  
public:
  LevelView(Game *game) : View(game), selectedTile(nullptr), fposition(Position(0,0)), iposition(Position(Position::Type::INVENTORY,0,0)), position(&fposition), heldPiece(nullptr) { }
  void handleEvent(SDL_Event &event);
  void draw();
  void activate();
  
  static void drawField(const Field *field, int bx, int by);
  static void drawGrid(int x, int y, int w, int h);
  static void drawInventory(const Field *field, int bx, int by);

  static void drawTooltip(int x, int y, const char* text);
};

#endif
