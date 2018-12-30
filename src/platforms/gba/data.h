#pragma once

#include <cstdint>

#include "gfx.h"
#include "common/common.h"

using sprite_data_t = uint32_t;

enum
{
  SPRITE_IDX_BACKGROUND = 0,
  
  SPRITE_IDX_CELL_HIGHLIGHT,
  
  SPRITE_IDX_MIRROR_NORTH,
  SPRITE_IDX_MIRROR_NORTH_EAST,
  SPRITE_IDX_MIRROR_EAST,
  SPRITE_IDX_MIRROR_SOUTH_EAST,
  
  SPRITE_IDX_PRISM_NORTH,
  SPRITE_IDX_PRISM_NORTH_EAST,
  SPRITE_IDX_PRISM_EAST,
  SPRITE_IDX_PRISM_SOUTH_EAST,
  
  
  SPRITE_IDX_SOURCE_NORTH,
  SPRITE_IDX_SOURCE_NORTH_EAST,
  SPRITE_IDX_SOURCE_EAST,
  SPRITE_IDX_SOURCE_SOUTH_EAST,
  
  L1, L2, L3, L4, L5, L6, L7,
  
  
  SPRITE_IDX_COUNT
};

class Data
{
public:
  static const sprite_info_t* getSpriteData(u32 index);
};
