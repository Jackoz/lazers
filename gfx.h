//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _GFX_H_
#define _GFX_H_

#include "common.h"
#include "level.h"

#define OSX
#define SCALE (3)

#include <SDL/SDL.h>

#ifdef SCALE
#include <SDL_image/SDL_image.h>
#else
#include <SDL_image.h>
#endif


#define BACKGROUND_COLOR (Gfx::ccc(20, 20, 40))

typedef SDL_Color color;

class Gfx
{
  private:
    static SDL_PixelFormat *format;
    static const u16 WIDTH = 320;
    static const u16 HEIGHT = 240;
  
    static u8 charWidth(char c);
  

    
  public:
    static void line(u16 x1, u16 y1, u16 x2, u16 y2, u32 color);
    static void rect(u16 x1, u16 y1, u16 w, u16 h, u32 color);
    static void rectFill(s16 x1, s16 y1, u16 x2, u16 y2, u32 color);
    static void clear(u32 color);
    static inline void lock() { SDL_LockSurface(screen); }
    static inline void unlock() { SDL_UnlockSurface(screen); }
    static void blit(SDL_Surface *src, SDL_Surface *dst, u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h, u16 f);
    static void blit(SDL_Surface *src, u16 x, u16 y, u16 w, u16 h, u16 dx, u16 dy);
    static void drawString(int x, int y, const char *text, ...);
    
    static void setFormat(SDL_PixelFormat *format) { Gfx::format = format; }
    static void init();
    
    //static inline color makeColor(u8 r, u8 g, u8 b) { return (color){r,g,b,0}; }
    
    static inline SDL_Rect ccr(int x, int y, int w, int h)
    {
      return SDL_Rect{static_cast<s16>(x),static_cast<s16>(y),static_cast<u16>(w),static_cast<u16>(h)};
    }
    
    static inline u32 ccc(u8 r, u8 g, u8 b)
    {
      return SDL_MapRGB(format, r, g, b);
    }
    
    static inline u32 makeColor(color color)
    {
      return SDL_MapRGB(format, color.r, color.g, color.b);
      //return (r << format->Rshift) | (g << format->Gshift) | (b << format->Bshift);
    }
    
    static void postDraw();
    
    static SDL_Surface *tiles;
    static SDL_Surface *font;
    static SDL_Surface *ui;
  
  #ifdef SCALE
    static SDL_Surface *realScreen;
  #endif
    static SDL_Surface *screen;
  
};

#endif
