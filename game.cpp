//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "game.h"

#include "gfx.h"
#include <SDL/SDL.h>

void LevelView::drawField()
{
  for (int x = 0; x < FIELD_WIDTH; ++x)
    for (int y = 0; y < FIELD_HEIGHT; ++y)
    {
      Tile *tile = field.tileAt(x,y);
      
      if (tile->piece())
      {
        SDL_Rect src = tile->piece()->gfxRect();
        SDL_Rect dst = Gfx::ccr(coordX(x, false)+1,coordY(y)+1,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
      
      for (int i = 0; i < 8; ++i)
      {
        if (tile->colors[i])
        {
          SDL_Rect src = Gfx::ccr(PIECE_SIZE*8+(PIECE_SIZE)*i, PIECE_SIZE*(tile->colors[i]-1), PIECE_SIZE, PIECE_SIZE);
          SDL_Rect dst = Gfx::ccr(coordX(x, false)+1, coordY(y)+1, 0, 0);
          SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
        }
      }
      
      // diagonal middle filler '\'
      if (tile->colors[NORTH_WEST] && tile->colors[NORTH_WEST] == tile->colors[SOUTH_EAST])
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*17, PIECE_SIZE*(tile->colors[NORTH_WEST]-1) + 5, 4, 4);
        SDL_Rect dst = Gfx::ccr(coordX(x, false)+ 1 + 5, coordY(y)+ 1 + 5, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
      // diagonal middle filler '/'
      if (tile->colors[SOUTH_WEST] && tile->colors[SOUTH_WEST] == tile->colors[NORTH_EAST])
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*17, PIECE_SIZE*(tile->colors[SOUTH_WEST]-1), 4, 4);
        SDL_Rect dst = Gfx::ccr(coordX(x, false)+ 1 + 5, coordY(y)+ 1 + 5, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
      
      
      
      Tile *upper = y > 0 ? field.tileAt(x, y-1) : nullptr;
      Tile *left = x > 0 ? field.tileAt(x-1, y) : nullptr;
      Tile *upperLeft = upper && left ? field.tileAt(x-1, y-1) : nullptr;
      Tile *upperRight = upper && x < FIELD_WIDTH ? field.tileAt(x+1, y-1) : nullptr;
      
      // place horizontal fillers
      if (tile->colors[NORTH] && (upper && upper->colors[SOUTH] == tile->colors[NORTH]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16,PIECE_SIZE*(tile->colors[NORTH]-1),PIECE_SIZE,1);
        SDL_Rect dst = Gfx::ccr(coordX(x, false)+1,coordY(y),0,0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
      
      // place vertical fillers
      if (tile->colors[WEST] && (left && left->colors[EAST] == tile->colors[WEST]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16,PIECE_SIZE*(tile->colors[WEST]-1),1,PIECE_SIZE);
        SDL_Rect dst = Gfx::ccr(coordX(x, false),coordY(y)+1,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
      
      // place cross filler upper left
      if (tile->colors[NORTH_WEST] && (upperLeft && upperLeft->colors[SOUTH_EAST] == tile->colors[NORTH_WEST]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16 + 9,PIECE_SIZE*(tile->colors[NORTH_WEST]-1) + 9 , 5, 5);
        SDL_Rect dst = Gfx::ccr(coordX(x, false)-2,coordY(y)-2,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
      // place cross filler upper right
      if (tile->colors[NORTH_EAST] && (upperRight && upperRight->colors[SOUTH_WEST] == tile->colors[NORTH_EAST]))
      {
        SDL_Rect src = Gfx::ccr(PIECE_SIZE*16 + 9,PIECE_SIZE*(tile->colors[NORTH_EAST]-1) + 9 , 5, 5);
        SDL_Rect dst = Gfx::ccr(coordX(x+1, false)-2,coordY(y)-2,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
    }
  
  for (int x = 0; x < INVENTORY_WIDTH; ++x)
    for (int y = 0; y < INVENTORY_HEIGHT; ++y)
    {
      Tile *tile = field.tileAt(x+FIELD_WIDTH,y);
      
      if (tile->piece())
      {
        SDL_Rect src = tile->piece()->gfxRect();
        SDL_Rect dst = {static_cast<s16>(coordX(x+FIELD_WIDTH, true)+1),static_cast<s16>(coordY(y)+1),0,0};
        SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
      }
    }
}


void LevelView::draw()
{
  Gfx::clear(Gfx::ccc(20, 20, 40));
  
  // draw field
  SDL_Rect bgRect = {176,264,16,16};
  for (int i = 0; i < FIELD_WIDTH; ++i)
  {
    for (int j = 0; j < FIELD_HEIGHT; ++j)
    {
      SDL_Rect tileRect = Gfx::ccr(coordX(i, false), coordY(j), 15, 15);
      SDL_BlitSurface(Gfx::tiles, &bgRect, Gfx::screen, &tileRect);
      
      if (i < INVENTORY_WIDTH && j < INVENTORY_HEIGHT)
      {
        SDL_Rect tileRect = Gfx::ccr(coordX(i+FIELD_WIDTH, true), coordY(j), 15, 15);
        SDL_BlitSurface(Gfx::tiles, &bgRect, Gfx::screen, &tileRect);
      }
    }
  }
  
  Gfx::lock();
  
  Gfx::rect(coordX(position->x, true), coordY(position->y), TILE_SIZE, TILE_SIZE, Gfx::ccc(180, 0, 0));
  
  if (selectedTile)
    Gfx::rect(coordX(selectedTile->x, true), coordY(selectedTile->y), TILE_SIZE, TILE_SIZE, Gfx::ccc(240, 240, 0));
  
  Gfx::unlock();
  
  drawField();
  
  Gfx::drawString("Y: switch zone\nX: rotate left\nA: rotate right\nB: select piece", 245, 110);
  
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
        case SDLK_ESCAPE: game->quit(); break;
          
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
          Piece *piece = field.tileAt(position->x, position->y)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateRight();
            field.updateLasers();
          }
          
          break;
        }
        case SDLK_LALT: // B
        {
          Tile *curTile = field.tileAt(*position);
          
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
              
              field.updateLasers();
            }
          }
          
          break;
        }    
        case SDLK_LSHIFT: // X
        {
          Piece *piece = field.tileAt(position->x, position->y)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateLeft();
            field.updateLasers();
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

void Game::init()
{
  const char* test = "any carnal pleasure.";
  char *output = nullptr;
  size_t length;
  Files::encode(test, strlen(test), &output, &length);
  printf("\n%*s\n",static_cast<int>(length),output);
  
  Gfx::init();
  running = true;
}

void Game::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    switch (event.type) {
      case SDL_QUIT: quit(); break;
      default:
      {
        if (overView) overView->handleEvent(event);
        else view->handleEvent(event);
        break;
      }
    }
  }
}



void Game::loop()
{
  while (running)
  {
    view->draw();
    if (overView) overView->draw();
    
    handleEvents();
  }
}