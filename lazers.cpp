//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "lazers.h"

SDL_PixelFormat *Gfx::format = nullptr;

SDL_Surface *Gfx::screen = nullptr;
#ifdef SCALE
  SDL_Surface *Gfx::realScreen = nullptr;
#endif


SDL_Surface *Gfx::tiles = nullptr;

void Gfx::init()
{
  SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
  
	//SDL_ShowCursor(SDL_DISABLE);
#ifdef SCALE
	realScreen = SDL_SetVideoMode(WIDTH*SCALE, HEIGHT*SCALE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  screen = SDL_CreateRGBSurface(SDL_HWSURFACE, WIDTH, HEIGHT, 32, realScreen->format->Rmask, realScreen->format->Gmask, realScreen->format->Bmask, realScreen->format->Amask);
#else
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
#endif
  
	if (screen->format->BitsPerPixel != 32) {
    fprintf(stderr, "ERROR: Did not get 32 bpp, got %u bpp instead.\n",
            screen->format->BitsPerPixel);
    exit(1);
	}
	if (!(screen->flags & SDL_HWSURFACE)) {
		fprintf(stderr, "WARNING: Did not get a hardware surface.\n");
	}
	if (!(screen->flags & SDL_DOUBLEBUF)) {
		fprintf(stderr, "WARNING: Did not get double buffering.\n");
	}
  
  Gfx::setFormat(screen->format);
  
  //tiles = IMG_Load("/Users/jack/Documents/Dev/xcode/lazers/Lazers/Lazers/data/mirror.png");
  
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

void Gfx::blit(SDL_Surface *src, SDL_Surface *dst, u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h, u16 f)
{
  SDL_LockSurface(src);
  SDL_LockSurface(dst);
  
  u32 *sp = (u32*)src->pixels;
  u32 *dp = (u32*)dst->pixels;

  // for each pixel in source
  for (u16 x = sx; x < sx+w; ++x)
    for (u16 y = sy; y < sy+h; ++y)
    {
      u32 color = PIXEL(sp, src->w, x, y);
      // for each dest pixel according to scale factor
      for (u16 ix = 0; ix < f; ++ix)
        for (u16 iy = 0; iy < f; ++iy)
        {
          PIXEL(dp, dst->w, dx + x*f + ix, dy + y*f + iy) = color;
        } 
    }
  
  SDL_UnlockSurface(src);
  SDL_UnlockSurface(dst);
}


void Gfx::line(u16 x1, u16 y1, u16 x2, u16 y2, u32 color)
{
  bool hor = y1 == y2;
  u32* p = (u32*)screen->pixels;

  if (hor)
    for (u16 x = x1; x <= x2; ++x)
      p[y1*screen->w + x] = color;
  else
    for (u16 y = y1; y <= y2; ++y)
      p[y*screen->w + x1] = color;
}

void Gfx::rect(u16 x1, u16 y1, u16 x2, u16 y2, u32 color)
{
  line(x1, y1, x1, y2, color);
  line(x1, y1, x2, y1, color);
  line(x2, y1, x2, y2, color);
  line(x1, y2, x2, y2, color);
}

void Gfx::rectFill(s16 x1, s16 y1, u16 x2, u16 y2, u32 color)
{
  SDL_Rect rect = {x1,y1,static_cast<u16>(x2-x1+1),static_cast<u16>(y2-y1+1)};
  SDL_FillRect(screen, &rect, color);
  
  /*u32* p = (u32*)screen->pixels;
  
  for (int x = x1; x <= x2; ++x)
    for (int y = y1; y <= y2; ++y)
      p[y*screen->w + x] = color;*/
}

void Gfx::clear(u32 color)
{
  SDL_Rect frect = {0,0,WIDTH,HEIGHT};
  SDL_FillRect(screen, &frect, color);
}




void Gfx::render(Game *game)
{
  clear(ccc(220, 220, 220));
  
  //rect = {0,0,20,20};
  //SDL_BlitSurface(Gfx::tiles, &rect, screen, &rect);
  
  lock();
  
  u32 color = ccc(180, 0, 0);
  
  for (int i = 0; i < FIELD_WIDTH+1; ++i)
    line(i*TILE_SIZE, 0, i*TILE_SIZE, FIELD_HEIGHT*TILE_SIZE, color);
  
  for (int i = 0; i < FIELD_HEIGHT+1; ++i)
    line(0, i*TILE_SIZE, FIELD_WIDTH*TILE_SIZE, i*TILE_SIZE, color);
  
  rect(TILE_SIZE*game->position.x, TILE_SIZE*game->position.y, TILE_SIZE*(game->position.x+1), TILE_SIZE*(game->position.y+1), ccc(180, 0, 0));
  
  unlock();
  
  //drawField(game);
  
  #ifdef SCALE
    blit(screen, realScreen, 0, 0, 0, 0, WIDTH, HEIGHT, SCALE);
    SDL_Flip(realScreen);
  #else
    SDL_Flip(screen);
  #endif
}

void Gfx::drawField(Game *game)
{
  Field *field = game->field();
  
  for (int x = 0; x < FIELD_WIDTH; ++x)
    for (int y = 0; y < FIELD_HEIGHT; ++y)
    {
      Tile *tile = field->tileAt(x,y);
      
      if (tile->piece)
      {
        SDL_Rect src = {static_cast<s16>((PIECE_SIZE)*tile->piece->rotation()),0,PIECE_SIZE,PIECE_SIZE};
        SDL_Rect dst = {static_cast<s16>(x*TILE_SIZE+1),static_cast<s16>(y*TILE_SIZE+1),0,0};
        SDL_BlitSurface(tiles, &src, screen, &dst);
      }
    }
}




void Game::init()
{

  Gfx::init();
  running = true;
  position = {0,0};
}

void Game::handleEvents()
{
  SDL_Event event;
  
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT: running = false; break;
      case SDL_KEYDOWN:			// Button press
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE: running = false; break;
            
          case SDLK_LEFT:
          {
            if (position.x > 0) --position.x;
            break;
          }
          case SDLK_RIGHT:
          {
            if (position.x < FIELD_WIDTH - 1) ++position.x;
            break;
          }
          case SDLK_UP:
          {
            if (position.y > 0) --position.y;
            break;
          }
            
          case SDLK_DOWN:
          {
            if (position.y < FIELD_HEIGHT - 1) ++position.y;
            break;
          }
            
          case SDLK_LCTRL: // A
          {
            
            break;
          }
            
          case SDLK_LALT: // B
          {
            
            break;
          }
            
          case SDLK_LSHIFT: // X
          {
            
            break;
          }
            
          case SDLK_SPACE: // Y
          {
            
            break;
          }
            
          case SDLK_TAB: // L
          {
            Piece *piece = field_.tileAt(position.x, position.y)->piece;
            if (piece)
              piece->rotateLeft();
            
            
            break;
          }
            
          case SDLK_BACKSPACE: // R
          {
            Piece *piece = field_.tileAt(position.x, position.y)->piece;
            if (piece)
              piece->rotateRight();
            
            break;
          }
            
          default: break;
        }
        break;
      }
    }
  }
}



void Game::loop()
{
  while (running)
  {
    Gfx::render(this);
    handleEvents();
  }
}
