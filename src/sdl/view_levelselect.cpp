//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "view_levelselect.h"

#include "view_level.h"
#include "gfx.h"
#include "ui.h"

#include "SDL.h"

LevelSelectView::LevelSelectView(Game *game) : View(game), preview(nullptr), field(game->field), levelList(LevelList(game))
{
  previewWidth = (field->width()+field->invWidth())*ui::TILE_SIZE+10;
  previewHeight = field->height()*ui::TILE_SIZE;
}

void LevelSelectView::activate()
{
  if (!preview) preview = Gfx::generateSurface(previewWidth, previewHeight); //TODO: height is max between field and inv
  
  levelList.set(0);
  levelList.reset();
  
  rebuildPreview();
}

void LevelSelectView::draw()
{
  Gfx::clear(BACKGROUND_COLOR);
  
  
  Gfx::drawString(ui::TITLE_X, ui::TITLE_Y, false, "- %s - by %s", game->pack->name().c_str(), game->pack->author().c_str());

  Gfx::drawString(20, 220, false, "B: start level    \x1F\x1E: choose level    A: back");

  for (int i = 0; levelList.hasNext(i) && i < ui::LIST_SIZE; ++i)
  {
    const LevelSpec* spec = levelList.get(i);
    
    Gfx::drawString(ui::LIST_X, ui::LIST_Y+ ui::LIST_DY*i, false, "%s%s", spec->name.c_str(), spec->solved ? " \x1D" : "");
    
    if (levelList.isSelected(i))
      Gfx::blit(Gfx::ui, 0, 0, 4, 7, ui::LIST_X-8, ui::LIST_Y+ ui::LIST_DY*i);
  }
  
  Gfx::drawString(ui::LIST_X+30, ui::LIST_Y+ ui::LIST_DY*levelList.LIST_SIZE+10, false, "%d of %d", levelList.current()+1, levelList.count());

  SDL_Rect dest = { 150, 30, previewWidth/2, previewHeight/2};
  Gfx::blit(preview, nullptr, &dest);
}

void LevelSelectView::handleMouseEvent(EventType type, int x, int y, int button)
{
  auto i = ui::coordToListEntry(x, y);
  
  if (type == EventType::MOUSE_MOTION)
  {
    if (i >= 0 && (i + levelList.getOffset()) != levelList.current() && levelList.isValidIndex(i))
    {
      levelList.set(levelList.getOffset() + i);
      rebuildPreview();
    }
  }
  else if (type == EventType::MOUSE_DOWN)
  {
    int i = ui::coordToListEntry(x, y);
    
    if (i >= 0 && levelList.get(i))
      game->switchView(VIEW_LEVEL);
  }
}


void LevelSelectView::handleEvent(SDL_Event &event)
{
  switch(event.type)
  {
    case SDL_MOUSEWHEEL:
    {
      ui::handleMouseWheelOnList(levelList, event.wheel.y);
      break;
    }
    
    case SDL_KEYDOWN:			// Button press
    {
      switch(event.key.keysym.sym)
      {
        case KEY_START:
        {
          game->quit();
          break;
        }
          
        case KEY_SELECT:
        {
          //game->pack = Files::packAt(0);
          game->switchView(VIEW_PACK_SELECT);
          break;
        }

        case KEY_DOWN:
        {
          if (levelList.down())
            rebuildPreview();
          break;
        }
        case KEY_UP:
        {
          if (levelList.up())
            rebuildPreview();
          break;
        }
          
        case KEY_L:
        {          
          if (levelList.prevPage())
            rebuildPreview();
          break;
        }
          
        case KEY_R:
        {
          if (levelList.nextPage())
            rebuildPreview();
          break;
        }
          
        case KEY_B:
        {
          game->switchView(VIEW_LEVEL);
        }
          
        default: break;
      }
    }
  }
}

void LevelSelectView::rebuildPreview()
{
  field->reset();
  field->load(game->pack->at(game->pack->selected));
  
  Gfx::setTarget(preview);
  Gfx::clear(BACKGROUND_COLOR);
  LevelView::drawGrid(0, 0, field->width(), field->height());
  LevelView::drawGrid(field->width()*ui::TILE_SIZE + 10, 0, field->invWidth(), field->invHeight());
  LevelView::drawField(field, 0, 0);
  LevelView::drawInventory(field, field->width()*ui::TILE_SIZE + 10, 0);
  Gfx::setTarget(nullptr);
  //Gfx::blit(preview, scaledPreview, 0, 0, 160, 150, 0, 0);
}
