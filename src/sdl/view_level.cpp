//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "view_level.h"

#include "ui.h"
#include "gfx.h"
#include "SDL.h"

#include "common/i18n.h"


static const u16 GFX_FIELD_POS_X = 0;
static const u16 GFX_FIELD_POS_Y = 15;

Position LevelView::coordToPosition(int x, int y)
{
  static const u16 GFX_INVENTORY_POS_X = ui::TILE_SIZE*field()->width() + 10;

  if (x >= GFX_FIELD_POS_X && x <= GFX_FIELD_POS_X + ui::TILE_SIZE*field()->width() && y >= GFX_FIELD_POS_Y && y < GFX_FIELD_POS_Y + ui::TILE_SIZE*field()->height())
  {
    auto tx = (x - GFX_FIELD_POS_X) / ui::TILE_SIZE, ty = (y - GFX_FIELD_POS_Y) / ui::TILE_SIZE;
    return Position(Position::Type::FIELD, tx, ty);
  }
  else if (x >= GFX_INVENTORY_POS_X && x < GFX_INVENTORY_POS_X + ui::TILE_SIZE*field()->invWidth() && y >= GFX_FIELD_POS_Y && y < GFX_FIELD_POS_Y + ui::TILE_SIZE*field()->invHeight())
  {
    auto tx = (x - GFX_INVENTORY_POS_X) / ui::TILE_SIZE, ty = (y - GFX_FIELD_POS_Y) / ui::TILE_SIZE;
    return Position(Position::Type::INVENTORY, tx, ty);
  }
  else
    return Position(Position::Type::INVALID);
}

u16 LevelView::coordX(const Position& p)
{
  static const u16 GFX_INVENTORY_POS_X = ui::TILE_SIZE*field()->width() + 10;

  if (p.type != Position::Type::INVENTORY)
    return ui::TILE_SIZE*p.x + GFX_FIELD_POS_X;
  else
    return ui::TILE_SIZE*p.x + GFX_INVENTORY_POS_X;
}

u16 LevelView::coordY(const Position& p) { return ui::TILE_SIZE*p.y + GFX_FIELD_POS_Y; }

void LevelView::activate()
{

}

SDL_Rect LevelView::rectForPiece(const Piece* piece)
{
  Position gfx = Position(0,0);
  int color = piece->color();
  
  
  switch (piece->type())
  {
    case PIECE_WALL: gfx = Position(13,7); break;
    case PIECE_GLASS: gfx = Position(11,7); break;
    
    case PIECE_SOURCE: gfx = Position(piece->rotation(), 1); break;

    case PIECE_MIRROR: gfx = Position(piece->rotation(), 0); break;
    case PIECE_SKEW_MIRROR: gfx = Position(piece->rotation(), 10); break;
    case PIECE_DOUBLE_MIRROR: gfx = Position(piece->rotation() % 4 + 4, 5); break;
    case PIECE_DOUBLE_SPLITTER_MIRROR: gfx = Position(piece->rotation() % 4 + 4, 11); break;
    case PIECE_DOUBLE_PASS_MIRROR: gfx = Position(piece->rotation() % 4, 12); break;
    case PIECE_DOUBLE_SKEW_MIRROR: gfx = Position(piece->rotation() % 4 + 4, 12); break;
    case PIECE_REFRACTOR: gfx = Position(piece->rotation() % 4, 5); break;

    case PIECE_SPLITTER: gfx = Position(piece->rotation(), 2); break;
    case PIECE_ANGLED_SPLITTER: gfx = Position(piece->rotation(), 3); break;
    case PIECE_PRISM: gfx = Position(piece->rotation(), 4); break;

    case PIECE_FILTER: gfx = Position(color + 8, 8); break;
    case PIECE_ROUND_FILTER: gfx = Position(piece->rotation() % 4, 9); break;
    case PIECE_POLARIZER: gfx = Position(color + 8, piece->rotation() % 4 + 9); break;

    case PIECE_RIGHT_BENDER: gfx = Position(14, 7); break;
    case PIECE_RIGHT_TWISTER: gfx = Position(12, 7); break;
    case PIECE_LEFT_BENDER: gfx = Position(10, 7); break;
    case PIECE_LEFT_TWISTER: gfx = Position(9, 7); break;

    case PIECE_SELECTOR: gfx = Position(piece->rotation(), 12 + color); break;




    case PIECE_THREE_WAY_SPLITTER: gfx = Position(piece->rotation() + 8, 17); break;
    case PIECE_STAR_SPLITTER: gfx = Position(10, 7); break;
    case PIECE_FLIPPED_PRISM: gfx = Position(piece->rotation() + 8, 17); break;
    case PIECE_SPLICER: gfx = Position(piece->rotation(), 12 + 7 + color); break;
    

    case PIECE_TUNNEL: gfx = Position(piece->rotation(), 6); break;
    case PIECE_COLOR_SHIFTER: gfx = Position(piece->rotation(), 8); break;
    case PIECE_COLOR_INVERTER: gfx = Position(piece->rotation(), 7); break;
    case PIECE_CROSS_COLOR_INVERTER: gfx = Position(piece->rotation() % 2 + 4, 9); break;
    case PIECE_TELEPORTER: gfx = Position(9, 7); break;
    case PIECE_TNT: gfx = Position(15, 7); break;
    case PIECE_SLIME: gfx = Position(8, 7); break;
    case PIECE_MINE: gfx = Position(8, 8); break;
    case PIECE_STRICT_GOAL: gfx = Position(color + 8, static_cast<const Goal*>(piece)->isSatisfied() ? 14 : 13); break;
      
    default:
      assert(false);
  }
  
  return { gfx.x*ui::PIECE_SIZE, gfx.y*ui::PIECE_SIZE, ui::PIECE_SIZE, ui::PIECE_SIZE };
}

void LevelView::drawField(const Field *field, SDL_Surface *screen, u16 bx, u16 by)
{
  for (int x = 0; x < field->width(); ++x)
    for (int y = 0; y < field->height(); ++y)
    {
      const Tile *tile = field->tileAt(Position(x, y));
      
      u16 cx = bx + x*ui::TILE_SIZE;
      u16 cy = by + y*ui::TILE_SIZE;
      
      if (tile->piece())
      {
        SDL_Rect src = rectForPiece(tile->piece().get());
        SDL_Rect dst = Gfx::ccr(cx+1,cy+1,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      for (int i = 0; i < 8; ++i)
      {
        if (tile->colors[i] != LaserColor::NONE)
        {
          SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*8+(ui::PIECE_SIZE)*i, ui::PIECE_SIZE*(tile->colors[i]-1), ui::PIECE_SIZE, ui::PIECE_SIZE);
          SDL_Rect dst = Gfx::ccr(cx+1, cy+1, 0, 0);
          SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
        }
      }
      
      // diagonal middle filler '\'
      if (tile->colors[NORTH_WEST] != LaserColor::NONE && tile->colors[NORTH_WEST] == tile->colors[SOUTH_EAST])
      {
        SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*17, ui::PIECE_SIZE*(tile->colors[NORTH_WEST]-1) + 5, 4, 4);
        SDL_Rect dst = Gfx::ccr(cx + 1 + 5, cy + 1 + 5, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      // diagonal middle filler '/'
      if (tile->colors[SOUTH_WEST] != LaserColor::NONE && tile->colors[SOUTH_WEST] == tile->colors[NORTH_EAST])
      {
        SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*17, ui::PIECE_SIZE*(tile->colors[SOUTH_WEST]-1), 4, 4);
        SDL_Rect dst = Gfx::ccr(cx + 1 + 5, cy + 1 + 5, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }

      const Tile* upper = y > 0 ? field->tileAt(Position(x, y-1)) : nullptr;
      const Tile* left = x > 0 ? field->tileAt(Position(x-1, y)) : nullptr;
      const Tile* upperLeft = upper && left ? field->tileAt(Position(x-1, y-1)) : nullptr;
      const Tile* upperRight = upper && x < field->width() ? field->tileAt(Position(x+1, y-1)) : nullptr;
      
      // place horizontal fillers
      if (tile->colors[NORTH] != LaserColor::NONE && (upper && upper->colors[SOUTH] == tile->colors[NORTH]))
      {
        SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*16,ui::PIECE_SIZE*(tile->colors[NORTH]-1),ui::PIECE_SIZE,1);
        SDL_Rect dst = Gfx::ccr(cx + 1, cy,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      // place vertical fillers
      if (tile->colors[WEST] != LaserColor::NONE && (left && left->colors[EAST] == tile->colors[WEST]))
      {
        SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*16,ui::PIECE_SIZE*(tile->colors[WEST]-1),1,ui::PIECE_SIZE);
        SDL_Rect dst = Gfx::ccr(cx,cy + 1,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      
      // place cross filler upper left
      if (tile->colors[NORTH_WEST] != LaserColor::NONE && (upperLeft && upperLeft->colors[SOUTH_EAST] == tile->colors[NORTH_WEST]))
      {
        SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*16 + 9,ui::PIECE_SIZE*(tile->colors[NORTH_WEST]-1) + 9 , 5, 5);
        SDL_Rect dst = Gfx::ccr(cx - 2,cy - 2,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
      // place cross filler upper right
      if (tile->colors[NORTH_EAST] != LaserColor::NONE && (upperRight && upperRight->colors[SOUTH_WEST] == tile->colors[NORTH_EAST]))
      {
        SDL_Rect src = Gfx::ccr(ui::PIECE_SIZE*16 + 9,ui::PIECE_SIZE*(tile->colors[NORTH_EAST]-1) + 9 , 5, 5);
        SDL_Rect dst = Gfx::ccr(cx - 2, cy - 2,0,0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
    }
  

}

void LevelView::drawInventory(const Field* field, SDL_Surface *screen, u16 bx, u16 by)
{
  for (int x = 0; x < field->invWidth(); ++x)
    for (int y = 0; y < field->invHeight(); ++y)
    {
      const Tile* tile = field->tileAt(Position(Position::Type::INVENTORY, x, y));
      
      if (tile->piece())
      {
        SDL_Rect src = rectForPiece(tile->piece().get());
        SDL_Rect dst = Gfx::ccr(bx+ui::TILE_SIZE*x + 1, by + ui::TILE_SIZE*y + 1, 0, 0);
        SDL_BlitSurface(Gfx::tiles, &src, screen, &dst);
      }
    }
}

void LevelView::drawGrid(int x, int y, int w, int h, SDL_Surface *screen)
{
  for (int i = 0; i < w; ++i)
    for (int j = 0; j < h; ++j)
    {
      SDL_Rect bgRect = Gfx::ccr(176+16*0,266,16,16);
      SDL_Rect tileRect = Gfx::ccr(x+i*ui::TILE_SIZE, y+j*ui::TILE_SIZE, 15, 15);
      SDL_BlitSurface(Gfx::tiles, &bgRect, screen, &tileRect);
    }
}

void LevelView::drawTooltip(int x, int y, const char* text)
{
  static constexpr u32 padding = 2;
  static constexpr u32 margin = 2;

  auto width = Gfx::stringWidth(text) + padding * 2;
  auto height = Gfx::stringHeight(text) + padding * 2;

  x -= width / 2;

  if (x < 0) x = margin;
  else if (x + width > Gfx::width() - margin) x = Gfx::width() - margin - width;
  if (y < 0) y = margin;
  else if (y + height > Gfx::height() - margin) y = Gfx::height() - margin - height;

  Gfx::rectFill(x, y, x + width, y + height, Gfx::ccc(0, 45, 120));
  Gfx::rect(x, y, width, height, Gfx::ccc(30, 116, 255));

  Gfx::drawString(x + padding + 1, y + padding + 1, false, text);
}


void LevelView::draw()
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  x /= SCALE, y /= SCALE;
  
  auto field = this->field();
  
  Gfx::clear(BACKGROUND_COLOR);
  
  // draw field
  
  const auto inventoryBaseX = coordX(Position(Position::Type::INVENTORY, 0, 0));
  
  drawGrid(GFX_FIELD_POS_X, GFX_FIELD_POS_Y, field->width(), field->height(), Gfx::screen);
  drawGrid(inventoryBaseX, GFX_FIELD_POS_Y, field->invWidth(), field->invHeight(), Gfx::screen);

  Gfx::lock();
  
  Gfx::rect(coordX(*position), coordY(*position), ui::TILE_SIZE, ui::TILE_SIZE, Gfx::ccc(180, 0, 0));
  Gfx::rect(coordX(*position)+1, coordY(*position)+1, ui::TILE_SIZE-2, ui::TILE_SIZE-2, Gfx::ccc(180, 0, 0));
  
  if (selectedTile)
    Gfx::rect(coordX(Position(selectedTile->x, selectedTile->y)), coordY(Position(selectedTile->x, selectedTile->y)), ui::TILE_SIZE, ui::TILE_SIZE, Gfx::ccc(240, 240, 0));
  
  Gfx::unlock();
  
  drawField(field, Gfx::screen, GFX_FIELD_POS_X, GFX_FIELD_POS_Y);
  drawInventory(field, Gfx::screen, inventoryBaseX, GFX_FIELD_POS_Y);
  
  if (field->level())
    Gfx::drawString(GFX_FIELD_POS_X + field->width()*ui::TILE_SIZE/2, 5, true, "%s%s", field->level()->name.c_str(), field->level()->solved ? " \x1D" : "");

  // 245, 110
  
  const int BASE_X = 80;
  const int BASE_Y = GFX_FIELD_POS_X + field->height()*ui::TILE_SIZE + 20;
  const int STEP = 14;
  
  if (position == &iposition)
    Gfx::drawString(BASE_X, BASE_Y, true, "Y: to field");
  else
    Gfx::drawString(BASE_X, BASE_Y, true, "Y: to inventory");
  
  Tile *curTile = field->tileAt(*position);
  
  //if (!selectedTile && curTile->piece() && curTile->piece()->canBeMoved())
  //  Gfx::drawString(10, FIELD_HEIGHT*ui::TILE_SIZE+28, "B: pick up piece");

  if (!selectedTile && curTile->piece() && curTile->piece()->canBeMoved())
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: pick up");
  else if (selectedTile && curTile == selectedTile)
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: drop");
  else if (selectedTile && !curTile->piece())
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: drop");
  else if (selectedTile && curTile->piece())
    Gfx::drawString(BASE_X, BASE_Y + STEP*2, true, "B: swap");

  
  if (curTile->piece() && curTile->piece()->canBeRotated())
  {
    Gfx::drawString(BASE_X - 40, BASE_Y + STEP, true, "X: rotate left");
    Gfx::drawString(BASE_X + 40, BASE_Y + STEP, true, "A: rotate right");
  }
  
  if (heldPiece)
  {    
    SDL_Rect src = rectForPiece(heldPiece.get());
    SDL_Rect dst = Gfx::ccr(x - ui::PIECE_SIZE/2, y - ui::PIECE_SIZE/2,0,0);
    SDL_BlitSurface(Gfx::tiles, &src, Gfx::screen, &dst);
  }

  if (curTile->piece())
  {
    drawTooltip(x, y + 15, i18n::nameForPiece(curTile->piece()->type()));
  }
  
  if (field->isFailed())
  {
    Gfx::drawString(field->width()*ui::TILE_SIZE/2 + GFX_FIELD_POS_X, GFX_FIELD_POS_Y + field->height()*ui::TILE_SIZE/2, true, "FAILED!");
  }
  
  //Gfx::drawString(245, 110, "Y: switch zone\nX: rotate left\nA: rotate right\nB: select piece");
  
  Gfx::postDraw();
}



void LevelView::handleEvent(SDL_Event &event)
{
  auto field = this->field();
  
  switch(event.type)
  {
    case SDL_MOUSEMOTION:
    {
      auto x = event.motion.x / SCALE, y = event.motion.y / SCALE;
      Position hover = coordToPosition(x, y);
      
      if (field->isInside(hover))
        *position = hover;
      
      break;
    }
      
    case SDL_MOUSEBUTTONDOWN:
    {
      auto x = event.motion.x / SCALE, y = event.motion.y / SCALE;
      Position hover = coordToPosition(x, y);
      
      Tile* tile = field->tileAt(hover);
      const auto& piece = tile->piece();
      
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        if (!heldPiece && piece && piece->canBeMoved())
        {
          tile->swap(heldPiece);
          field->updateLasers();
        }
        else if (heldPiece && (!piece || piece->canBeMoved()))
        {
          tile->swap(heldPiece);
          field->updateLasers();
        }
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        if (piece && piece->canBeRotated())
        {
          piece->rotateRight();
          field->updateLasers();
        }
      }
      

      
      break;
    }
    
    case SDL_KEYDOWN:			// Button press
    {
      auto*& p = position;
      auto key = event.key.keysym.sym;
      
      switch(key)
      {
        case KEY_START: game->quit(); break;
        case KEY_SELECT: game->switchView(VIEW_LEVEL_SELECT); break;

        case SDLK_z:
        case SDLK_x:
        case SDLK_c:
        {
          const auto& piece = field->tileAt(*position)->piece();

          if (piece.get() && piece->canBeColored())
          {
            LaserColor channel = key == SDLK_z ? LaserColor::RED : (key == SDLK_x ? LaserColor::GREEN : LaserColor::BLUE);
            LaserColor color = piece->color();
            
            if ((color & channel) && (color != channel)) color = (LaserColor)((color & ~channel) & LaserColor::WHITE);
            else color = (LaserColor)(color | channel);

            piece->setColor(color);
            field->updateLasers();
          }

          break;
        }
          
        case SDLK_LEFT:
        {
          if (p->x > 0)
            --p->x;
          else if (p->x == 0 && p->isInventory())
          {
            fposition.x = field->width()-1;
            fposition.y = p->y;
            p = &fposition;
          }

          break;
        }
        case SDLK_RIGHT:
        {
          if (p->isInventory() && p->x < field->invWidth() - 1)
            ++p->x;
          else if (!p->isInventory())
          {
            if (p->x < field->width() - 1)
              ++p->x;
            else
            {
              iposition.x = 0;
              iposition.y = std::min((u32)p->y, field->invHeight()-1);
              p = &iposition;
            }
          }

          break;
        }
        case SDLK_UP:
        {
          if (p->y > 0)
            --p->y;
          break;
        }
        case SDLK_DOWN:
        {
          auto max = (p->isInventory() ? field->invHeight() : field->height()) - 1;
          if (p->y < max)
            ++p->y;
          break;
        }
          //   Y
          //  X A
          //   B
        case SDLK_LCTRL: // A
        {
          const auto& piece = field->tileAt(*position)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateRight();
            field->updateLasers();
          }
          
          break;
        }
        case SDLK_LALT: // B
        {
          Tile *curTile = field->tileAt(*position);
          
          if (!selectedTile && curTile->piece() && curTile->piece()->canBeMoved())
          {
            selectedTile = curTile;
          }
          else if (selectedTile == curTile)
            selectedTile = nullptr;
          else if (selectedTile)
          {
            const auto& oldPiece = selectedTile->piece();
            const auto& newPiece = curTile->piece();
            
            if (!newPiece || newPiece->canBeMoved())
            {
              selectedTile->swap(curTile);
              selectedTile = nullptr;              
              field->updateLasers();
            }
          }
          
          break;
        }
        case SDLK_LSHIFT: // X
        {
          const auto& piece = field->tileAt(*position)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateLeft();
            field->updateLasers();
          }
          
          break;
        }
        case SDLK_SPACE: // Y
        {
          position = position == &iposition ? &fposition : &iposition;
          break;
        }
        case SDLK_TAB: // L
        {
          break;
        }
        case SDLK_BACKSPACE: // R
        {
          break;
        }
        default: break;
      }
      break;
    }
  }
}
