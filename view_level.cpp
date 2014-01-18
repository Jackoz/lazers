//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "view_level.h"

#include "gfx.h"
#include <SDL/SDL.h>

void LevelView::activate()
{

}

void LevelView::drawField(Field *field, SDL_Surface *screen, u16 bx, u16 by)
{
  for (int x = 0; x < FIELD_WIDTH; ++x)
    for (int y = 0; y < FIELD_HEIGHT; ++y)
    {
      Tile *tile = field->tileAt(x,y);
      
      u16 cx = bx + x*TILE_SIZE;
      u16 cy = by + y*TILE_SIZE;
      
      if (tile->piece())
      {
        SDL_Rect src = tile->piece()->gfxRect();
        SDL_Rect dst = Gfx::ccr(cx+1,cy+1,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      for (int i = 0; i < 8; ++i)
      {
        if (tile->colors[i])
        {
          SDL_Rect src = Gfx::ccr(PIECE_SIZE*8+(PIECE_SIZE)*i, PIECE_SIZE*(tile->colors[i]-1), PIECE_SIZE, PIECE_SIZE);
          SDL_Rect dst = Gfx::ccr(cx+1, cy+1, 0, 0);
          SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
        }
      }
      
      // diagonal middle filler '\'
      if (tile->colors[NORTH_WEST] && tile->colors[NORTH_WEST] == tile->colors[SOUTH_EAST])
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*17, PIECE_SIZE*(tile->colors[NORTH_WEST]-1) + 5, 4, 4);
        SDL_Rect dst = Gfx::ccr(cx + 1 + 5, cy + 1 + 5, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      // diagonal middle filler '/'
      if (tile->colors[SOUTH_WEST] && tile->colors[SOUTH_WEST] == tile->colors[NORTH_EAST])
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*17, PIECE_SIZE*(tile->colors[SOUTH_WEST]-1), 4, 4);
        SDL_Rect dst = Gfx::ccr(cx + 1 + 5, cy + 1 + 5, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      
      
      Tile *upper = y > 0 ? field->tileAt(x, y-1) : nullptr;
      Tile *left = x > 0 ? field->tileAt(x-1, y) : nullptr;
      Tile *upperLeft = upper && left ? field->tileAt(x-1, y-1) : nullptr;
      Tile *upperRight = upper && x < FIELD_WIDTH ? field->tileAt(x+1, y-1) : nullptr;
      
      // place horizontal fillers
      if (tile->colors[NORTH] && (upper && upper->colors[SOUTH] == tile->colors[NORTH]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16,PIECE_SIZE*(tile->colors[NORTH]-1),PIECE_SIZE,1);
        SDL_Rect dst = Gfx::ccr(cx + 1, cy,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      // place vertical fillers
      if (tile->colors[WEST] && (left && left->colors[EAST] == tile->colors[WEST]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16,PIECE_SIZE*(tile->colors[WEST]-1),1,PIECE_SIZE);
        SDL_Rect dst = Gfx::ccr(cx,cy + 1,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      // place cross filler upper left
      if (tile->colors[NORTH_WEST] && (upperLeft && upperLeft->colors[SOUTH_EAST] == tile->colors[NORTH_WEST]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16 + 9,PIECE_SIZE*(tile->colors[NORTH_WEST]-1) + 9 , 5, 5);
        SDL_Rect dst = Gfx::ccr(cx - 2,cy - 2,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      // place cross filler upper right
      if (tile->colors[NORTH_EAST] && (upperRight && upperRight->colors[SOUTH_WEST] == tile->colors[NORTH_EAST]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16 + 9,PIECE_SIZE*(tile->colors[NORTH_EAST]-1) + 9 , 5, 5);
        SDL_Rect dst = Gfx::ccr(cx - 2, cy - 2,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
    }
  

}

void LevelView::drawInventory(Field* field, SDL_Surface *screen, u16 bx, u16 by)
{
  for (int x = 0; x < INVENTORY_WIDTH; ++x)
    for (int y = 0; y < INVENTORY_HEIGHT; ++y)
    {
      Tile *tile = field->tileAt(x+FIELD_WIDTH,y);
      
      if (tile->piece())
      {
        SDL_Rect src = tile->piece()->gfxRect();
        SDL_Rect dst = Gfx::ccr(bx+TILE_SIZE*x + 1, by + TILE_SIZE*y + 1, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
    }
}

void LevelView::drawGrid(Field *field, int x, int y, int w, int h, SDL_Surface *screen)
{
  for (int i = 0; i < w; ++i)
    for (int j = 0; j < h; ++j)
    {
      SDL_Rect bgRect = Gfx::ccr(176+16*0,266,16,16);
      SDL_Rect tileRect = Gfx::ccr(x+i*TILE_SIZE, y+j*TILE_SIZE, 15, 15);
      SDL_BlitSurface(Gfx::tiles, &bgRect, screen, &tileRect);
    }
}


void LevelView::draw()
{
  Gfx::clear(BACKGROUND_COLOR);
  
  // draw field
  
  drawGrid(field, GFX_FIELD_POS_X, GFX_FIELD_POS_Y, FIELD_WIDTH, FIELD_HEIGHT, Gfx::screen);
  drawGrid(field, GFX_INVENTORY_POS_X, GFX_FIELD_POS_Y, INVENTORY_WIDTH, INVENTORY_HEIGHT, Gfx::screen);

  Gfx::lock();
  
  Gfx::rect(coordX(position->x, true), coordY(position->y), TILE_SIZE, TILE_SIZE, Gfx::ccc(180, 0, 0));
  Gfx::rect(coordX(position->x, true)+1, coordY(position->y)+1, TILE_SIZE-2, TILE_SIZE-2, Gfx::ccc(180, 0, 0));
  
  if (selectedTile)
    Gfx::rect(coordX(selectedTile->x, true), coordY(selectedTile->y), TILE_SIZE, TILE_SIZE, Gfx::ccc(240, 240, 0));
  
  Gfx::unlock();
  
  drawField(field, Gfx::screen, GFX_FIELD_POS_X, GFX_FIELD_POS_Y);
  drawInventory(field, Gfx::screen, GFX_INVENTORY_POS_X, GFX_FIELD_POS_Y);
  
  
  Gfx::drawString(GFX_FIELD_POS_X + FIELD_WIDTH*TILE_SIZE/2, 5, true, "%s%s", field->level->name.c_str(), field->level->solved ? " \x1D" : "");

  // 245, 110
  
  const int BASE_X = 80;
  const int BASE_Y = GFX_FIELD_POS_X + FIELD_HEIGHT*TILE_SIZE + 20;
  const int STEP = 14;
  
  if (position == &iposition)
    Gfx::drawString(BASE_X, BASE_Y, true, "Y: to field");
  else
    Gfx::drawString(BASE_X, BASE_Y, true, "Y: to inventory");
  
  Tile *curTile = field->tileAt(*position);
  
  //if (!selectedTile && curTile->piece() && curTile->piece()->canBeMoved())
  //  Gfx::drawString(10, FIELD_HEIGHT*TILE_SIZE+28, "B: pick up piece");

  if (!selectedTile && curTile->piece() && curTile->piece()->canBeMoved())
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: pick up");
  else if (selectedTile && curTile == selectedTile)
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: drop");
  else if (selectedTile && !curTile->piece())
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: drop");
  else if (selectedTile && curTile->piece())
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: swap");

  
  if (curTile->piece() && curTile->piece()->canBeRotated())
  {
    Gfx::drawString(BASE_X - 40, BASE_Y + STEP, true, "X: rotate left");
    Gfx::drawString(BASE_X + 40, BASE_Y + STEP, true, "A: rotate right");
  }
  
  //Gfx::drawString(245, 110, "Y: switch zone\nX: rotate left\nA: rotate right\nB: select piece");
  
  Gfx::postDraw();
}



void LevelView::handleEvent(SDL_Event &event)
{
  switch(event.type)
  {
    case SDL_KEYDOWN:			// Button press
    {
      switch(event.key.keysym.sym)
      {
        case KEY_START: game->quit(); break;
        case KEY_SELECT: game->switchView(VIEW_LEVEL_SELECT); break;
          
        case SDLK_LEFT:
        {
          if ((position == &fposition && position->x > 0) || (position == &iposition && position->x > FIELD_WIDTH))
            --position->x;
          
          break;
        }
        case SDLK_RIGHT:
        {
          if ((position == &fposition && position->x < FIELD_WIDTH-1) || (position == &iposition && position->x < FIELD_WIDTH+INVENTORY_WIDTH-1))
            ++position->x;
          break;
        }
        case SDLK_UP:
        {
          if (position->y > 0)
            --position->y;
          break;
        }
        case SDLK_DOWN:
        {
          if ((position == &fposition && position->y < FIELD_HEIGHT-1) || (position == &iposition && position->y < INVENTORY_HEIGHT-1))
            ++position->y;
          break;
        }
          //   Y
          //  X A
          //   B
        case SDLK_LCTRL: // A
        {
          Piece *piece = field->tileAt(position->x, position->y)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateRight();
            field->updateLasers();
          }
          
          break;
        }
        case SDLK_LALT: // B
        {
          Tile *curTile = field->tileAt(*position);
          
          if (!selectedTile && curTile->piece() && curTile->piece()->canBeMoved())
          {
            selectedTile = curTile;
          }
          else if (selectedTile == curTile)
            selectedTile = nullptr;
          else if (selectedTile)
          {
            Piece *oldPiece = selectedTile->piece();
            Piece *newPiece = curTile->piece();
            
            if (!newPiece || newPiece->canBeMoved())
            {
              selectedTile->place(newPiece);
              curTile->place(oldPiece);
              //std::swap(selectedTile_->piece, curTile->piece);
              
              selectedTile = nullptr;
              
              field->updateLasers();
            }
          }
          
          break;
        }
        case SDLK_LSHIFT: // X
        {
          Piece *piece = field->tileAt(position->x, position->y)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateLeft();
            field->updateLasers();
          }
          
          break;
        }
        case SDLK_SPACE: // Y
        {
          position = position == &iposition ? &fposition : &iposition;
          break;
        }
        case SDLK_TAB: // L
        {
          break;
        }
        case SDLK_BACKSPACE: // R
        {
          break;
        }
        default: break;
      }
      break;
    }
  }
}
