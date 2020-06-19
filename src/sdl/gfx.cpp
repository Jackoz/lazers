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

#ifdef _WIN32
  std::string prefix = "./../../";
#else
  std::string prefix = "";
#endif

  for (size_t i = 0; i < 3; ++i)
  {
    SDL_Surface* surface = IMG_Load((prefix + textures[i]).c_str());
    
    if (!surface)
    {
      const char* message = IMG_GetError();
      printf("IMG_Load error: %s\n", message);
      assert(false);
    }

    *dest[i] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
  }

  //SDL_EnableKeyRepeat(300/*SDL_DEFAULT_REPEAT_DELAY*/, 80/*SDL_DEFAULT_REPEAT_INTERVAL*/);
}

SDL_Texture* Gfx::generateSurface(u32 w, u32 h)
{
  return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);
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

u32 Gfx::stringHeight(const std::string& text)
{
  return 9;
}

u32 Gfx::stringWidth(const std::string& text)
{
  u32 strLen = 0;

  for (size_t i = 0; i < text.size(); ++i)
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

void Gfx::drawString(int x, int y, bool centered, const std::string& text)
{
  if (centered)
  {
    u16 length = stringWidth(text);
    x -= length/2;
  }
  
  const size_t len = text.size();
  
  SDL_Rect rect;
  SDL_Rect out = ccr(x, y, 0, 0);
  
  for (size_t i = 0; i < len; ++i)
  {
    char c = text[i];
    
    if (c == '\n')
    {
      out.y += dy;
      out.x = x;
    }
    else if (c == '^')
    {
      if (text[i + 1] == '^')
      {
        SDL_SetTextureColorMod(font, 0xFF, 0xFF, 0xFF);
        ++i;
      }
      else
      {
        int rgb[] = { text[i + 1], text[i + 2], text[i + 3] };
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

void Gfx::drawStringBounded(int x, int y, int w, std::string text)
{
  lwstring string = text;
  std::vector<lwstring> words;
  size_t c = 0;
  size_t len = string.size();

  while (c < len)
  {
    size_t s = string.find_first_of(" ", c);

    if (c != s)
      words.emplace_back(string.substr(c, s - c));

    if (s == lwstring::npos)
      break;
    else
      text[s] = '\0';

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

