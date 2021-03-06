#pragma once

#include "common/common.h"

#include "SDL.h"
#include "gfx.h"

#if _WIN32 || __APPLE__
#include <string_view>
using lwstring = std::string_view;
#else
using lwstring = std::string;
#endif

#define KEY_LEFT (SDLK_LEFT)
#define KEY_RIGHT (SDLK_RIGHT)
#define KEY_UP (SDLK_UP)
#define KEY_DOWN (SDLK_DOWN)
#define KEY_X (SDLK_LSHIFT)
#define KEY_Y (SDLK_SPACE)
#define KEY_A (SDLK_LCTRL)
#define KEY_B (SDLK_LALT)
#define KEY_L (SDLK_TAB)
#define KEY_R (SDLK_BACKSPACE)
#define KEY_SELECT (SDLK_ESCAPE)
#define KEY_START (SDLK_RETURN)

class ui
{
public:
  static constexpr int TILE_SIZE = 15;
  static constexpr int PIECE_SIZE = 14;
  
  static constexpr int LIST_X = 20;
  static constexpr int LIST_Y = 30;
  static constexpr int LIST_DY = 10;
  static constexpr int LIST_SIZE = 14;
  static constexpr int LIST_WIDTH = 150;

  static constexpr int TITLE_X = 20;
  static constexpr int TITLE_Y = 10;

  static int coordToListEntry(int x, int y)
  {
    if (x >= LIST_X && x < LIST_X + LIST_WIDTH && y >= LIST_Y && y < LIST_Y + LIST_DY * LIST_SIZE)
      return (y - LIST_Y) / LIST_DY;
    else
      return -1;

  }

  template<typename T> static bool handleMouseWheelOnList(T& list, int wd)
  {
    int x, y;
    SDL_GetMouseState(&x, &y);
    x /= SCALE; y /= SCALE;
    int i = ui::coordToListEntry(x, y);

    if (i >= 0)
    {
      if (wd < 0 && list.getOffset() < list.count() - LIST_SIZE)
        list.changeOffset(+1);
      else if (wd > 0 && list.getOffset() > 0)
        list.changeOffset(-1);

      return true;
    }
    else
      return false;
  }

  static const char* textColorForLaser(LaserColor color)
  {
    switch (color)
    {
      case LaserColor::RED: return "^d00";
      case LaserColor::GREEN: return "^3f7";
      case LaserColor::BLUE: return "^4ae";

      case LaserColor::YELLOW: return "^ed0";
      case LaserColor::MAGENTA: return "^f4d";
      case LaserColor::CYAN: return "^4ee";

      case LaserColor::WHITE: return "^fff";     

      default:
        assert(false);
        return "";
    }
  }
};
