//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "gfx.h"

#include "game.h"
#include <cmath>

#define PIXEL(s, w, x, y) s[(y)*(w) + x]
SDL_PixelFormat *Gfx::format = nullptr;

SDL_Surface *Gfx::screen = nullptr;
#ifdef SCALE
SDL_Surface *Gfx::realScreen = nullptr;
#endif

SDL_Surface *Gfx::tiles = nullptr;
SDL_Surface *Gfx::font = nullptr;
SDL_Surface *Gfx::ui = nullptr;

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
    //"/Users/jack/Documents/Dev/xcode/Lazers/Lazers/lazers""/Users/jack/Desktop/lazers/cpp-lazers/data/
    tiles = IMG_Load("/Users/jack/Documents/Dev/xcode/Lazers/Lazers/lazers/data/tiles.png");
    font = IMG_Load("/Users/jack/Documents/Dev/xcode/Lazers/Lazers/lazers/data/font.png");
    ui = IMG_Load("/Users/jack/Documents/Dev/xcode/Lazers/Lazers/lazers/data/ui.png");


  #else
    font = IMG_Load("./font.png");
    tiles = IMG_Load("./tiles.png");
    ui = IMG_Load("./ui.png");
  #endif
  
  SDL_EnableKeyRepeat(300/*SDL_DEFAULT_REPEAT_DELAY*/, 80/*SDL_DEFAULT_REPEAT_INTERVAL*/);
}

SDL_Surface *Gfx::generateSurface(u16 w, u16 h)
{
  return SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, realScreen->format->Rmask, realScreen->format->Gmask, realScreen->format->Bmask, realScreen->format->Amask);
}

void Gfx::scaleNN(SDL_Surface *src, SDL_Surface *dst, u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h, u16 f)
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

float cubicInterpolate (float p[4], float x) {
	return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

void Gfx::scaleBicubic(SDL_Surface *src, SDL_Surface *dst, u16 sw, u16 sh, u16 dw, u16 dh)
{
  SDL_LockSurface(src);
  SDL_LockSurface(dst);
  
  u32 *sp = (u32*)src->pixels;
  u32 *dp = (u32*)dst->pixels;
  
  // calculate offset for each pixel
  float dx = static_cast<float>(sw)/dw;
  float dy = static_cast<float>(sh)/dh;
  u32 pgrid[4][4];
  float grid[3][4][4];
  
  // for each pixel in destination width
  for (u16 x = 0; x < dw; ++x)
  {
    // for each pixel in destination height
    for (u16 y = 0; y < dh; ++y)
    {
      // compute coordinates in source image for the 4x4 grid
      float sxf[4] = {(dx)*(x-1), dx*x, dx*(x+1), dx*(x+2)};
      float syf[4] = {(dy)*(y-1), dy*y, dy*(y+1), dy*(y+2)};

      // clamp coordinates to min and max width/height
      for (u16 t = 0; t < 4; ++t)
      {
        if (sxf[t] < 0) sxf[t] = 0;
        else if (sxf[t] >= sw) sxf[t] = sw-1;
        
        if (syf[t] < 0) syf[t] = 0;
        else if (syf[t] >= sh) syf[t] = sh-1;
        
        sxf[t] = roundf(sxf[t]);
        syf[t] = roundf(syf[t]);
      }
      
      // compute actual grid
      for (int t = 0; t < 4; ++t)
        for (int u = 0; u < 4; ++u)
          pgrid[t][u] = PIXEL(sp, src->w, static_cast<int>(sxf[t]), static_cast<int>(syf[u]));
      
      // compute float grid for each channel
      for (int t = 0; t < 4; ++t)
      {
        for (int u = 0; u < 4; ++u)
        {
          grid[0][t][u] = ((pgrid[t][u] & src->format->Rmask) >> src->format->Rshift) / 256.0f;
          grid[1][t][u] = ((pgrid[t][u] & src->format->Gmask) >> src->format->Gshift) / 256.0f;
          grid[2][t][u] = ((pgrid[t][u] & src->format->Bmask) >> src->format->Bshift) / 256.0f;
        }
      }
      
      float columns[3][4];
      
      for (int u = 0; u < 3; ++u)
        for (int t = 0; t < 4; ++t)
          columns[u][t] = cubicInterpolate(grid[u][t], static_cast<float>(y)/dh);
      
      float finalColor[3];
      u8 ifinalColor[3];
      
      for (int u = 0; u < 3; ++u)
      {
        finalColor[u] = cubicInterpolate(columns[u], static_cast<float>(x)/dw)*256.0f;
        if (finalColor[u] < 0.0f) ifinalColor[u] = 0;
        else if (finalColor[u] > 255.0f) ifinalColor[u] = 255;
        else ifinalColor[u] = static_cast<u8>(roundf(finalColor[u]));
      }
      
      
      u32 color = SDL_MapRGB(dst->format, ifinalColor[0], ifinalColor[1], ifinalColor[2]);
      PIXEL(dp, dst->w, x, y) = color;
    }
  }
  
  
  SDL_UnlockSurface(src);
  SDL_UnlockSurface(dst);
}

void Gfx::blit(SDL_Surface *srcs, SDL_Surface *dest, u16 x, u16 y, u16 w, u16 h, u16 dx, u16 dy)
{
  SDL_Rect src = ccr(x, y, w, h);
  SDL_Rect dst = ccr(dx, dy, 0, 0);
  SDL_BlitSurface(srcs, &src, dest, &dst);
}

void Gfx::blit(SDL_Surface *srcs, u16 x, u16 y, u16 w, u16 h, u16 dx, u16 dy)
{
  blit(srcs, screen, x, y, w, h, dx, dy);
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

void Gfx::clear(SDL_Surface *surf, u32 color)
{
  SDL_Rect frect = {0,0,WIDTH,HEIGHT};
  SDL_FillRect(surf, &frect, color);
}

void Gfx::clear(u32 color)
{
  clear(screen, color);
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
    //case 'p':
    //case 'q':
    case 'r':
    case 't':
      return 4;
      
      
    default: return 5;
  }
}

void Gfx::drawString(int x, int y, const char *text, ...)
{
  char buffer[64];
  va_list args;
  va_start (args, text);
  vsnprintf (buffer, 64, text, args);
  va_end(args);
  
  
  u16 len = strlen(buffer);
  u16 dy = 9;
  u16 space = 3;
  
  SDL_Rect rect;
  SDL_Rect out = ccr(x, y, 0, 0);
  
  for (u16 i = 0; i < len; ++i)
  {
    char c = buffer[i];
    
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
      rect = ccr(6 * (c%32), 9 * (c/32), w, 9);
      SDL_BlitSurface(font, &rect, screen, &out);
      out.x += w+1;
    }
  }
}

void Gfx::postDraw()
{
  #ifdef SCALE
    scaleNN(screen, realScreen, 0, 0, 0, 0, WIDTH, HEIGHT, SCALE);
    SDL_Flip(realScreen);
  #else
    SDL_Flip(screen);
  #endif
}

