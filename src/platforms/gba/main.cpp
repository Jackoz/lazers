#include <array>

#include <cstdlib>
#include <cstring>

#include "common.h"
#include "math.h"
#include "gfx.h"
#include "bg.h"
#include "keypad.h"
#include "timer.h"
#include "bios.h"

#include "data.h"

#include "core/level.h"

alignas(4) color_t shipPalette[] = { color_t(7, 9, 10), color_t(10, 12, 13), color_t(12, 13, 14), color_t(13, 14, 14), color_t(13, 14, 15), color_t(19, 19, 20), color_t(20, 20, 21), color_t(20, 21, 21), color_t(21, 22, 23), color_t(22, 23, 23), color_t(22, 23, 23), color_t(23, 24, 24), color_t(23, 24, 24), color_t(24, 25, 25), color_t(24, 25, 25), color_t(0, 0, 0) };

alignas(4) color_t mirrorPalette[] = { color_t(0, 0, 0), color_t(5, 6, 6), color_t(8, 8, 8), color_t(10, 10, 11), color_t(13, 14, 14), color_t(13, 14, 15), color_t(15, 15, 15), color_t(16, 18, 19), color_t(20, 21, 22), color_t(20, 22, 22), color_t(20, 22, 23), color_t(22, 23, 24), color_t(24, 26, 27), color_t(25, 28, 29), color_t(28, 30, 31), color_t(30, 31, 31) };

// prismPalette color_t palette[] = { color_t(0, 0, 0), color_t(7, 5, 8), color_t(10, 7, 12), color_t(9, 11, 12), color_t(12, 8, 14), color_t(18, 15, 20), color_t(19, 20, 21), color_t(20, 22, 22), color_t(21, 23, 24), color_t(23, 25, 26), color_t(24, 25, 26), color_t(25, 26, 27), color_t(26, 28, 29), color_t(26, 28, 29), color_t(27, 28, 29), color_t(31, 31, 32) };
alignas(4) const color_t laserPalette[] = { color_t(0, 0, 0), color_t(26, 26, 26), color_t(30, 30, 30), color_t(29, 25, 0), color_t(31, 28, 0), color_t(0, 26, 0), color_t(7, 31, 7), color_t(8, 30, 31), color_t(0, 23, 26), color_t(1, 19, 29), color_t(8, 20, 31), color_t(25, 0, 23), color_t(31, 8, 26), color_t(26, 3, 7), color_t(31, 3, 6), color_t(0, 0, 0) };



color_t highlightColor = color_t(31, 5, 5);

Field field = Field(15, 10, 1, 1);
Gfx gfx;

class LevelView
{
public:
  enum
  {
    MIRROR_SPRITE = 12,
    PRISM_SPRITE = 28,
    SOURCE_SPRITE = 28 + 16,
    
    LASER_RED = SOURCE_SPRITE + 16
  };
  
public:
  void drawPiece(tile_entry* dest, u32 x, u32 y, Piece* piece)
  {
    dest = dest+(y*2)*32+(x*2);
    
    const auto o = piece->rotation();
    //bool flipHor = false, flipVer = false;
    bool rotated = false;
    u32 index = 0;
    
    switch (piece->type())
    {
      case PIECE_MIRROR:
      {
        rotated = o >= 4;
        index = MIRROR_SPRITE + (o%4)*4;
        break;
      }
        
      case PIECE_PRISM:
      {
        rotated = o >= 4;
        index = PRISM_SPRITE + (o%4)*4;
        break;
      }
        
      case PIECE_SOURCE:
      {
        rotated = o >= 4;
        index = SOURCE_SPRITE + (o%4)*4;
        break;
      }
    }
    
    if (!rotated)
    {
      dest->set(index, 2, false, false);
      (dest+1)->set(index+1, 2, false, false);
      (dest+32)->set(index+2, 2, false, false);
      (dest+32+1)->set(index+3, 2, false, false);
    }
    else if (rotated)
    {
      dest->set(index+3, 2, true, true);
      (dest+1)->set(index+2, 2, true, true);
      
      (dest+32)->set(index+1, 2, true, true);
      (dest+32+1)->set(index, 2, true, true);
    }
    /*else if (flipHor)
    {
      dest->set(index+3, 2, true, true);
      (dest+1)->set(index+2, 2, true, true);
      
      (dest+32)->set(index+1, 2, true, true);
      (dest+32+1)->set(index, 2, true, true);
    }*/
  }
  
  void refresh()
  {
    for (u32 y = 0; y < field.height(); ++y)
      for (u32 x = 0; x < field.width(); ++x)
        refreshPiece(x, y);
  }
  
  inline void refreshPiece(u32 x, u32 y)
  {
    const Tile* tile = field.tileAt(Pos(x, y));
    const auto& piece = tile->piece();
    
    if (piece)
      drawPiece(gfx.getBgTileMap(17), x, y, piece.get());
  }
  
  void dummyInit()
  {
    field.reset();
    
    for (int i = 0; i < 8; ++i)
    {
      field.place(Pos(2+i, 2), new Mirror((Direction)i));
      field.place(Pos(2+i, 3), new Prism((Direction)i));
      field.place(Pos(2+i, 6), new LaserSource((Direction)i, COLOR_RED));
    }
  }
};

LevelView levelView;

int main(void)
{
  static_assert(sizeof(port_disp_cnt) == sizeof(u16), "Must be 2 bytes");
  static_assert(sizeof(oam_entry) == sizeof(u16)*4, "Must be 8 bytes");
  static_assert(sizeof(oam_affine) == sizeof(oam_entry)*4, "Must be correct size");
  static_assert(sizeof(palette_t) == sizeof(color_t)*16, "Must be 32 bytes");
  static_assert(std::alignment_of<oam_entry>::value == 4, "");
  
  KeypadManager keypad;
  
  math::init();
  
  levelView.dummyInit();
  
  port_disp_cnt* dispCnt = gfx.dispCnt();
  dispCnt->set(0);
  dispCnt->setMode(video_mode::MODE0);
  dispCnt->enableBGs();
  dispCnt->enableOBJ();
  dispCnt->setObjMapping1D();
  
  port_bg_cnt* bg0cnt =  gfx.bg0Cnt();
  bg0cnt->set(0);
  bg0cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg0cnt->setTileDataBlock(0);
  bg0cnt->setTileMapBlock(16);
  bg0cnt->setPriority(3);
  
  port_bg_cnt* bg1cnt = gfx.bg1Cnt();
  bg1cnt->set(0);
  bg1cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg1cnt->setTileDataBlock(0);
  bg1cnt->setTileMapBlock(17);
  bg1cnt->setPriority(2);
  
  port_bg_cnt* bg2cnt = gfx.bg2Cnt();
  bg2cnt->set(0);
  bg2cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg2cnt->setTileDataBlock(0);
  bg2cnt->setTileMapBlock(18);
  bg2cnt->setPriority(1);
  
  port_bg_cnt* bg3cnt = gfx.bg3Cnt();
  bg3cnt->set(0);
  bg3cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg3cnt->setTileDataBlock(0);
  bg3cnt->setTileMapBlock(19);
  bg3cnt->setPriority(0);
  
  gfx.bg0().setOffset(0, 0);
  gfx.bg1().setOffset(0, 0);
  gfx.bg2().setOffset(-4, 0);
  gfx.bg3().setOffset(0, -4);
  
  memcpy(&gfx.getBgPalette(0), shipPalette, PALETTE_SMALL_SIZE * sizeof(color_t));
  memcpy(&gfx.getBgPalette(1), mirrorPalette, PALETTE_SMALL_SIZE * sizeof(color_t));
  memcpy(&gfx.getBgPalette(2), laserPalette, PALETTE_SMALL_SIZE * sizeof(color_t));

  
  memset(&gfx.getObjPalette(0), 0, PALETTE_SMALL_SIZE * sizeof(color_t));
  gfx.getObjPalette(0)[1] = color_t(5, 31, 5);
  
  memset(gfx.getBgTileData(0, 0), 0, BG_TILE_SIZE_4BPP*4);
  
  u32 ti = 4;
  for (u32 i = 0; i < SPRITE_IDX_COUNT; ++i)
  {
    auto* data = Data::getSpriteData(SPRITE_IDX_BACKGROUND + i);
    memcpy(gfx.getBgTileData(0, ti), data->data, data->sizeAs4BPP());
    ti += data->sizeInTiles();
  }
  
  memcpy(gfx.getObjTileData(0), Data::getSpriteData(SPRITE_IDX_CELL_HIGHLIGHT)->data, OBJ_TILE_SIZE_4BPP*4);

  auto* tileMap = gfx.getBgTileMap(16);

  for (int x = 0; x < 32; ++x)
    for (int y = 0; y < 32; ++y)
    {
      tileMap[y*32 + x].set(4 + (x & 0x01) | ((y & 0x01) << 1));
    }
  
  auto* laserMap = gfx.getBgTileMap(18);
  laserMap[0].set(LevelView::LASER_RED, 4, false, false);
  laserMap[32].set(LevelView::LASER_RED, 4, false, false);
  
  auto* laserMap2 = gfx.getBgTileMap(19);
  laserMap2[0].set(LevelView::LASER_RED+3 + 2, 4, false, false);
  laserMap2[1].set(LevelView::LASER_RED+3 + 2, 4, false, false);

  levelView.refresh();
  
  oam_entry object;
  object.reset();
  object.setAffineMode(oam_affine_mode::HIDDEN);

  for (u32 i = 0; i < OAM_COUNT; ++i)
    gfx.getOAM(i)->set(&object);

  object.setPalette(0);
  object.setTileIndex(0);
  object.setShape(oam_shape::SQUARE);
  object.setSize(oam_size::SIZE16x16);
  
  object.setAffineMode(oam_affine_mode::DISABLED);
  object.setAffineIndex(0);

  Pos hover = Pos(0, 0);

  object.setX(hover.x * 16);
  object.setY(hover.y * 16);
  
  gfx.getOAM(0)->set(&object);
  
  while (1)
  {        
    keypad.update();
    
    if (keypad.isPressed(Key::KEY_RIGHT))
    {
      if (hover.x < 14)
        ++hover.x;
    }
    else if (keypad.isPressed(Key::KEY_LEFT))
    {
      if (hover.x > 0)
        --hover.x;
    }
    
    if (keypad.isPressed(Key::KEY_UP))
    {
      if (hover.y > 0)
        --hover.y;
    }
    else if (keypad.isPressed(Key::KEY_DOWN))
    {
      if (hover.y < 9)
        ++hover.y;
    }
    else if (keypad.isPressed(Key::KEY_L))
    {
      const auto& piece = field.tileAt(hover)->piece();
      if (piece && piece->canBeRotated())
      {
        piece->rotateLeft();
        levelView.refreshPiece(hover.x, hover.y);
      }
    }
    else if (keypad.isPressed(Key::KEY_R))
    {
      const auto& piece = field.tileAt(hover)->piece();
      if (piece && piece->canBeRotated())
      {
        piece->rotateRight();
        levelView.refreshPiece(hover.x, hover.y);
      }
    }
    
    object.setX(hover.x * 16);
    object.setY(hover.y * 16);
    gfx.getOAM(0)->set(&object);
  }
  
  return 0;
}


