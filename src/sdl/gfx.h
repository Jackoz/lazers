//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _GFX_H_
#define _GFX_H_

#include "common/common.h"
#include "core/level.h"

#include <SDL.h>
#include <SDL_image.h>

#define BACKGROUND_COLOR (Gfx::ccc(20, 20, 40))

class Gfx
{
private:
  static SDL_PixelFormat *format;
  static const u16 WIDTH = 320;
  static const u16 HEIGHT = 240;

  static u32 charWidth(char c);
 
public:
  static u32 width() { return WIDTH; }
  static u32 height() { return HEIGHT; }
  
  static void line(u32 x1, u32 y1, u32 x2, u32 y2, SDL_Color color);
  static void rect(s32 x1, s32 y1, u32 w, u32 h, SDL_Color color);
  static void rectFill(s32 x1, s32 y1, u32 w, u32 h, SDL_Color color);

  static void clear(SDL_Color color);

  static SDL_Texture* generateSurface(u32 w, u32 h);

  static void scaleNN(SDL_Surface *src, SDL_Surface *dst, u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h, u16 f);

  static void blit(SDL_Texture* src, u16 x, u16 y, u16 w, u16 h, u16 dx, u16 dy);

  static void blit(SDL_Texture* src, const SDL_Rect* srcr, const SDL_Rect* dstr) { SDL_RenderCopy(renderer, src, srcr, dstr); }
  static void blit(SDL_Texture* src, const SDL_Rect& srcr, const SDL_Rect& dstr) { SDL_RenderCopy(renderer, src, &srcr, &dstr);  }

  static u32 stringHeight(const char* text);
  static u32 stringWidth(const char* text);
  static void drawString(int x, int y, bool centered, const char *text, ...);
  static void drawStringBounded(int x, int y, int w, const char *text, ...);
  
  static void setTarget(SDL_Texture* target) { SDL_SetRenderTarget(renderer, target);  }

  static void scaleBicubic(SDL_Surface *src, SDL_Surface *dst, u16 sw, u16 sh, u16 dw, u16 dh);

  static void setFormat(SDL_PixelFormat *format) { Gfx::format = format; }
  static void init();
  
  //static inline color makeColor(u8 r, u8 g, u8 b) { return (color){r,g,b,0}; }
  
  static inline SDL_Rect ccr(int x, int y, int w, int h) { return { x, y, w, h }; }
  static inline SDL_Color ccc(u8 r, u8 g, u8 b) { return { r, g, b, 0xFF }; }
  static inline SDL_Color ccc(u8 r, u8 g, u8 b, u8 a) { return { r, g, b, a }; }

  static SDL_Texture *tiles;
  static SDL_Texture *font;
  static SDL_Texture *ui;

  static SDL_Renderer* renderer;

private:
  static SDL_Window* window;
};



#endif
