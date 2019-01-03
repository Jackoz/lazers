//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "gfx.h"

#include "ui.h"
#include "game.h"
#include <cmath>
#include <cassert>

#define PIXEL(s, w, x, y) s[(y)*(w) + x]
SDL_PixelFormat *Gfx::format = nullptr;

SDL_Window* Gfx::window = nullptr;
SDL_Renderer* Gfx::renderer = nullptr;

SDL_Texture* Gfx::tiles = nullptr;
SDL_Texture* Gfx::font = nullptr;
SDL_Texture* Gfx::ui = nullptr;

void Gfx::init()
{
  SDL_Init(SDL_INIT_EVERYTHING);
	atexit(SDL_Quit);
  
  window = SDL_CreateWindow("Lazers",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  SDL_RenderSetScale(renderer, SCALE, SCALE);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  const char* textures[] = { "data/tiles.png", "data/font.png", "data/ui.png" };
  SDL_Texture** dest[] = { &tiles, &font, &ui };

  for (size_t i = 0; i < 3; ++i)
  {
    SDL_Surface* surface = IMG_Load(textures[i]);
    assert(surface);
    *dest[i] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
  }

  //SDL_EnableKeyRepeat(300/*SDL_DEFAULT_REPEAT_DELAY*/, 80/*SDL_DEFAULT_REPEAT_INTERVAL*/);
}

SDL_Texture* Gfx::generateSurface(u32 w, u32 h)
{
  return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);
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
	return p[1] + 0.5f * x*(p[2] - p[0] + x*(2.0f*p[0] - 5.0f*p[1] + 4.0f*p[2] - p[3] + x*(3.0f*(p[1] - p[2]) + p[3] - p[0])));
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
  for (u32 x = 0; x < dw; ++x)
  {
    // for each pixel in destination height
    for (u32 y = 0; y < dh; ++y)
    {
      // compute coordinates in source image for the 4x4 grid
      float sxf[4] = {(dx)*(x-1), dx*x, dx*(x+1), dx*(x+2)};
      float syf[4] = {(dy)*(y-1), dy*y, dy*(y+1), dy*(y+2)};

      // clamp coordinates to min and max width/height
      for (u32 t = 0; t < 4; ++t)
      {
        if (sxf[t] < 0) sxf[t] = 0;
        else if (sxf[t] >= sw) sxf[t] = (float)(sw-1);
        
        if (syf[t] < 0) syf[t] = 0;
        else if (syf[t] >= sh) syf[t] = (float)(sh-1);
        
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

void Gfx::blit(SDL_Texture *src, u16 x, u16 y, u16 w, u16 h, u16 dx, u16 dy)
{
  SDL_Rect srcr = ccr(x, y, w, h);
  SDL_Rect dstr = ccr(dx, dy, w, h);
  SDL_RenderCopy(renderer, src, &srcr, &dstr);
}

void Gfx::line(u32 x1, u32 y1, u32 x2, u32 y2, SDL_Color color)
{
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Gfx::rect(int x, int y, int w, int h, SDL_Color color)
{
  SDL_Rect rect = { x, y, w, h };
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); 
  SDL_RenderDrawRect(renderer, &rect);
}

void Gfx::rectFill(int x, int y, int w, int h, SDL_Color color)
{
  SDL_Rect rect = { x, y, w, h };
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
}

void Gfx::clear(SDL_Color color)
{
  SDL_Rect frect = {0,0,WIDTH,HEIGHT};
  rectFill(0, 0, WIDTH, HEIGHT, color);
}

u32 Gfx::charWidth(char c)
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

u32 Gfx::stringHeight(const char* text)
{
  return 9;
}

u32 Gfx::stringWidth(const char *text)
{
  size_t len = strlen(text);
  u32 strLen = 0;

  for (size_t i = 0; i < len; ++i)
  {
    char c = text[i];

    if (c == '^')
      i += text[i + 1] == '^' ? 1 : 3;
    else
      strLen += charWidth(c) + 1;
  }
  
  return strLen;
}

constexpr u32 dy = 9;

void Gfx::drawString(int x, int y, bool centered, const char *text, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, text);
  vsnprintf(buffer, 256, text, args);
  va_end(args);
  
  if (centered)
  {
    u16 length = stringWidth(buffer);
    x -= length/2;
  }
  
  size_t len = strlen(buffer);
  
  SDL_Rect rect;
  SDL_Rect out = ccr(x, y, 0, 0);
  
  for (size_t i = 0; i < len; ++i)
  {
    char c = buffer[i];
    
    if (c == '\n')
    {
      out.y += dy;
      out.x = x;
    }
    else if (c == '^')
    {
      if (buffer[i + 1] == '^')
      {
        SDL_SetTextureColorMod(font, 0xFF, 0xFF, 0xFF);
        ++i;
      }
      else
      {
        int rgb[] = { buffer[i + 1], buffer[i + 2], buffer[i + 3] };
        std::for_each(rgb, rgb + 3, [](int& v) {
          if (v >= 'A' && v <= 'F') v += - 'A' + 0x0a;
          else if (v >= 'a' && v <= 'f') v += - 'a' + 0xa;
          else if (v >= '0' && v <= '9') v -= '0';
          else assert(false);
        });

        SDL_SetTextureColorMod(font, rgb[0] * 17, rgb[1] * 17, rgb[2] * 17);
        i += 3;
      }

    }
    else
    {    
      u32 w = charWidth(c) + 1;
      rect = ccr(6 * (c%32), 9 * (c/32), w, 9);
      out.w = w;
      out.h = 9;

      SDL_RenderCopy(renderer, font, &rect, &out);
      out.x += w;
    }
  }

  SDL_SetTextureColorMod(font, 0xFF, 0xFF, 0xFF);
}

void Gfx::drawStringBounded(int x, int y, int w, const char *text, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, text);
  vsnprintf(buffer, 256, text, args);
  va_end(args);

  std::string_view string = buffer;
  std::vector<std::string_view> words;
  size_t c = 0;
  size_t len = string.size();

  while (c < len)
  {
    size_t s = string.find_first_of(" ", c);

    if (c != s)
      words.emplace_back(string.substr(c, s - c));

    if (s == std::string_view::npos)
      break;
    else
      buffer[s] = '\0';

    c = s + 1;
  }

  int cx = 0;
  for (size_t i = 0; i < words.size(); ++i)
  {
    const auto& word = words[i];
    u32 wlen = stringWidth(word.data());

    if (cx + wlen > w)
    {
      cx = 0;
      y += 9;
      --i;
    }
    else
    {
      if (cx != 0)
        cx += stringWidth(" ");

      for (const char c : word)
      {
        u32 w = charWidth(c);

        SDL_Rect rect = ccr(6 * (c % 32), 9 * (c / 32), w, 9);
        SDL_Rect out = ccr(cx + x, y, w, 9);

        SDL_RenderCopy(renderer, font, &rect, &out);
        cx += w + 1;
      }
    }
    
  }
}

