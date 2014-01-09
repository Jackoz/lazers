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
SDL_Surface *Gfx::tiles = nullptr;

void Gfx::init()
{
  SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
  
	//SDL_ShowCursor(SDL_DISABLE);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
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

void Gfx::rectFill(u16 x1, u16 y1, u16 x2, u16 y2, u32 color)
{
  u32* p = (u32*)screen->pixels;
  
  for (int x = x1; x <= x2; ++x)
    for (int y = y1; y <= y2; ++y)
      p[y*screen->w + x] = color;
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
  
  u32 color = ccc(80, 80, 80);
  
  for (int i = 0; i < Game::FIELD_WIDTH+1; ++i)
    line(i*TILE_SIZE, 0, i*TILE_SIZE, Game::FIELD_HEIGHT*TILE_SIZE, color);
  
  for (int i = 0; i < Game::FIELD_HEIGHT+1; ++i)
    line(0, i*TILE_SIZE, Game::FIELD_WIDTH*TILE_SIZE, i*TILE_SIZE, color);
  
  rectFill(TILE_SIZE*game->position.x, TILE_SIZE*game->position.y, TILE_SIZE*(game->position.x+1), TILE_SIZE*(game->position.y+1), ccc(180, 0, 0));
  
  unlock();
  
  SDL_Flip(screen);
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
