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
#include <SDL/SDL.h>

const u16 LIST_X = 20;
const u16 LIST_Y = 30;
const u16 LIST_DY = 10;
const u16 LIST_SIZE = 8;

LevelSelectView::LevelSelectView(Game *game) : View(game), offset(0), preview(nullptr), scaledPreview(nullptr), field(game->field)
{

}

void LevelSelectView::activate()
{
  if (!preview) preview = Gfx::generateSurface((FIELD_WIDTH+INVENTORY_WIDTH)*TILE_SIZE+10, FIELD_HEIGHT*TILE_SIZE);
  if (!scaledPreview) scaledPreview = Gfx::generateSurface((FIELD_WIDTH+INVENTORY_WIDTH)*7+5, FIELD_HEIGHT*7);
  rebuildPreview();
}

void LevelSelectView::draw()
{
  Gfx::clear(BACKGROUND_COLOR);
  
  Gfx::drawString(20, 10, "Pack name: %s (%s)", game->pack->name.c_str(), game->pack->author.c_str());

  Gfx::drawString(20, 220, "B: start level    \x1F\x1E: choose level    A: back", game->pack->name.c_str(), game->pack->author.c_str());

  
  
  for (int i = 0; i < LIST_SIZE && i + offset < game->pack->count(); ++i)
  {
    LevelSpec *spec = game->pack->at(offset+i);
    
    Gfx::drawString(LIST_X, LIST_Y+LIST_DY*i, "%s%s", spec->name.c_str(), " \x1D");
    
    if (i+offset == game->pack->selected)
      Gfx::blit(Gfx::ui, 0, 0, 4, 7, LIST_X-8, LIST_Y+LIST_DY*i);
  }
  

  Gfx::blit(scaledPreview, 0, 0, 160, 150, 170, 30);
  
  /*Gfx::lock();
  Gfx::rect(150, 30, 160, 150, Gfx::ccc(180, 0, 0));
  Gfx::unlock();*/
  
  Gfx::postDraw();
}

void LevelSelectView::handleEvent(SDL_Event &event)
{
  u32 minOffset = offset;
  u32 maxOffset = offset + LIST_SIZE - 1;
  
  switch(event.type)
  {
    case SDL_KEYDOWN:			// Button press
    {
      switch(event.key.keysym.sym)
      {
        case KEY_START: game->quit(); break;
        
        case KEY_DOWN:
        {
          if (game->pack->selected < game->pack->count() - 1)
          {
            ++game->pack->selected;
            rebuildPreview();
            
            if (game->pack->selected > maxOffset)
              ++offset;
          }
          break;
        }
        case KEY_UP:
        {
          if (game->pack->selected > 0)
          {
            --game->pack->selected;
            rebuildPreview();
            
            if (game->pack->selected < minOffset)
              --offset;
          }
          break;
        }
          
        case KEY_L:
        {          
          if (game->pack->selected > minOffset)
          {
            game->pack->selected = minOffset;
            rebuildPreview();
          }
          else
          {
            if (offset > 0)
            {
              u8 delta = LIST_SIZE <= offset ? LIST_SIZE : offset;
              offset -= delta;
              game->pack->selected -= delta;
              rebuildPreview();
            }
          }
          break;
        }
          
        case KEY_R:
        {
          if (game->pack->selected < maxOffset)
          {
            game->pack->selected = maxOffset;
            rebuildPreview();
          }
          else
          {
            if (offset < game->pack->count()-1)
            {
              if (game->pack->selected+LIST_SIZE <= game->pack->count()-1)
              {
                offset += LIST_SIZE;
                game->pack->selected += LIST_SIZE;
                rebuildPreview();
              }
              else
              {
                offset = game->pack->count() - LIST_SIZE;
                game->pack->selected = game->pack->count()-1;
                rebuildPreview();
              }
            }
          }
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
  
  
  Gfx::clear(preview, BACKGROUND_COLOR);
  LevelView::drawGrid(0, 0, FIELD_WIDTH, FIELD_HEIGHT, preview);
  LevelView::drawGrid(FIELD_WIDTH*TILE_SIZE + 10, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, preview);
  LevelView::drawField(field, preview, 0, 0);
  LevelView::drawInventory(field, preview, FIELD_WIDTH*TILE_SIZE + 10, 0);
  Gfx::scaleBicubic(preview, scaledPreview, preview->w, preview->h, scaledPreview->w, scaledPreview->h);
  //Gfx::blit(preview, scaledPreview, 0, 0, 160, 150, 0, 0);
}