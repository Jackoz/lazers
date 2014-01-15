//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "gfx.h"

#include "game.h"

#define PIXEL(s, w, x, y) s[(y)*(w) + x]
SDL_PixelFormat *Gfx::format = nullptr;

SDL_Surface *Gfx::screen = nullptr;
#ifdef SCALE
SDL_Surface *Gfx::realScreen = nullptr;
#endif

SDL_Surface *Gfx::tiles = nullptr;
SDL_Surface *Gfx::font = nullptr;

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
  
  #ifdef SCALE
    tiles = IMG_Load("/Users/jack/Documents/Dev/xcode/Lazers/Lazers/lazers/data/tiles.png");
    font = IMG_Load("/Users/jack/Documents/Dev/xcode/Lazers/Lazers/lazers/data/font.png");

  #else
    font = IMG_Load("./font.png");
    tiles = IMG_Load("./tiles.png");
  #endif
  
  SDL_EnableKeyRepeat(300/*SDL_DEFAULT_REPEAT_DELAY*/, 80/*SDL_DEFAULT_REPEAT_INTERVAL*/);
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

void Gfx::rect(u16 x, u16 y, u16 w, u16 h, u32 color)
{
  line(x, y, x, y+h, color);
  line(x, y, x+w, y, color);
  line(x+w, y, x+w, y+h, color);
  line(x, y+h, x+w, y+h, color);
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

u8 Gfx::charWidth(char c)
{
  switch (c) {
    case '!':
    case '\'':
    case '.':
    case ':':
    case '|':
      return 1;
    case '(':
    case ')':
    case ',':
    case ';':
    case '`':
      return 2;
    case ' ':
    case '\"':
    case '<':
    case '=':
    case '>':
    case '[':
    case ']':
    case 'i':
    case 'j':
    case 'l':
    case '{':
    case '}':
      return 3;
    case 'f':
    case 'p':
    case 'q':
    case 'r':
    case 't':
      return 4;
      
      
    default: return 5;
  }
}

void Gfx::drawString(const char *text, int x, int y)
{
  u16 len = strlen(text);
  u16 dy = 8;
  u16 space = 3;
  
  SDL_Rect rect;
  SDL_Rect out = ccr(x, y, 0, 0);
  
  for (u16 i = 0; i < len; ++i)
  {
    char c = text[i];
    
    if (c == '\n')
    {
      out.y += dy;
      out.x = x;
    }
    else if (c == ' ')
    {
      out.x += space;
    }
    else
    {    
      u8 w = charWidth(c);
      rect = ccr(6 * (c%32), 8 * (c/32), w, 7);
      SDL_BlitSurface(font, &rect, screen, &out);
      out.x += w+1;
    }
  }
}

void Gfx::postDraw()
{
  #ifdef SCALE
    blit(screen, realScreen, 0, 0, 0, 0, WIDTH, HEIGHT, SCALE);
    SDL_Flip(realScreen);
  #else
    SDL_Flip(screen);
  #endif
}

