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


static const int GFX_FIELD_POS_X = 0;
static const int GFX_FIELD_POS_Y = 15;

Position LevelView::coordToPosition(int x, int y)
{
  const int GFX_INVENTORY_POS_X = ui::TILE_SIZE*field()->width() + 10;

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
    return Position::invalid();
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

struct PieceGfx
{
  enum class Mode
  {
    NEVER_ROTATES, HAS_HALF_ROTATION
  };
  
  SDL_Rect rect;
  int rotation;

  PieceGfx(const Piece* piece, Mode mode, int x, int y) : rect({ x * ui::PIECE_SIZE, y * ui::PIECE_SIZE, ui::PIECE_SIZE, ui::PIECE_SIZE })
  {
    constexpr bool forceRotations = true;
    
    switch (mode) {
      case Mode::NEVER_ROTATES: rotation = 0; break;
      case Mode::HAS_HALF_ROTATION:
      {
        if (forceRotations)
          rotation = piece->orientation();
        else
        {
          if (!isOrtho(piece->orientation())) rect.x += ui::PIECE_SIZE;
          rotation = (piece->orientation() / 2) * 2;
        }

        break;
      }
    }
  }


  PieceGfx(int x, int y, int angle) : rect({ x * ui::PIECE_SIZE, y * ui::PIECE_SIZE, ui::PIECE_SIZE, ui::PIECE_SIZE }), rotation(angle) { }
  PieceGfx(int x, int y) : PieceGfx(x, y, 0) { }
};

PieceGfx LevelView::gfxForPiece(const Piece* piece)
{
  Position gfx = Position(0,0);
  int angle = piece->orientation();

  int color = piece->color();
  int orientaton = piece->orientation();
  const Direction o = piece->orientation();
 
  switch (piece->type())
  {
  case PIECE_WALL: return PieceGfx(13, 7);
  case PIECE_GLASS: return PieceGfx(11, 7);
    
  case PIECE_SOURCE: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 0, 1);
  case PIECE_MIRROR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 0, 0);

    case PIECE_SKEW_MIRROR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 4, 2);
    case PIECE_DOUBLE_MIRROR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 2, 2);
    case PIECE_DOUBLE_SPLITTER_MIRROR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 4, 4);
    case PIECE_DOUBLE_PASS_MIRROR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 0, 2);
    case PIECE_DOUBLE_SKEW_MIRROR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 4, 3);
    case PIECE_REFRACTOR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 4, 1);

    case PIECE_SPLITTER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 2, 0);
    case PIECE_ANGLED_SPLITTER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 4, 0);
    case PIECE_THREE_WAY_SPLITTER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 2, 1);
    case PIECE_STAR_SPLITTER: gfx = Position(8, 9); break;
    case PIECE_PRISM: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 0, 3);
    case PIECE_FLIPPED_PRISM: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 2, 3);

    case PIECE_FILTER: return PieceGfx(0, 8 + color);
    case PIECE_ROUND_FILTER: gfx = Position(orientaton % 4, 9); break;
    case PIECE_POLARIZER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 1, 8 + color);
    case PIECE_TUNNEL: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 0, 7);

    case PIECE_RIGHT_BENDER: gfx = Position(14, 7); break;
    case PIECE_RIGHT_TWISTER: gfx = Position(12, 7); break;
    case PIECE_LEFT_BENDER: gfx = Position(10, 7); break;
    case PIECE_LEFT_TWISTER: gfx = Position(9, 7); break;

    case PIECE_SELECTOR: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 3, 8 + color);
    case PIECE_SPLICER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 5, 8 + color);
    case PIECE_COLOR_SHIFTER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 2, 6);
    case PIECE_COLOR_INVERTER: return PieceGfx(piece, PieceGfx::Mode::HAS_HALF_ROTATION, 0, 6);

    case PIECE_TNT: gfx = Position(15, 7); break;

    case PIECE_CROSS_COLOR_INVERTER: gfx = Position(orientaton % 2 + 4, 9); break;
    case PIECE_TELEPORTER: gfx = Position(9, 7); break;
    case PIECE_SLIME: gfx = Position(8, 7); break;
    case PIECE_MINE: gfx = Position(8, 8); break;
    case PIECE_STRICT_GOAL: gfx = Position(color + 8, static_cast<const Goal*>(piece)->isSatisfied() ? 14 : 13); break;
      
    default:
      assert(false);
  }
  
  return PieceGfx(gfx.x, gfx.y, piece->orientation());
}

void LevelView::drawPiece(const Piece* piece, int cx, int cy)
{
  PieceGfx src = gfxForPiece(piece);
  SDL_Rect dst = Gfx::ccr(cx + 1, cy + 1, ui::PIECE_SIZE, ui::PIECE_SIZE);

  //TODO: verify if this is fine and don't rotate for unrotable pieces
  SDL_RenderCopyEx(Gfx::renderer, Gfx::tiles, &src.rect, &dst, src.rotation * (360.0 / 8), nullptr, SDL_FLIP_NONE);
  //Gfx::blit(Gfx::tiles, src, dst);
}

void LevelView::drawField(const Field *field, int bx, int by)
{  
  for (u32 x = 0; x < field->width(); ++x)
    for (u32 y = 0; y < field->height(); ++y)
    {
      const Tile *tile = field->tileAt(Position(x, y));
      
      u32 cx = bx + x*ui::TILE_SIZE;
      u32 cy = by + y*ui::TILE_SIZE;
      
      if (tile->piece())
        drawPiece(tile->piece().get(), cx, cy);
      
      static SDL_Rect rect = { 224, 16, 4, 1 };
      static SDL_Rect dst = { 0, 0, 4, 8 };
      
      struct Spec
      {
        int dx, dy;
        double angle;
        int length;
      };
      
      static const Spec specs[] = {
        { 6, 0, 0, 8},
        { 11, -2, 45, 11},
        { 10, 4, 90, 8},
        { 10, 7, 135, 11},
        { 6, 8, 0, 8},
        { 3, 6, 45, 11},
        { 2, 4, 90, 8},
        { 3, 0, 135, 11},
      };

      SDL_SetTextureAlphaMod(Gfx::tiles, 160);
      SDL_SetTextureBlendMode(Gfx::tiles, SDL_BLENDMODE_BLEND);
      for (int i = 0; i < 8; ++i)
      {
        if (tile->colors[i] != LaserColor::NONE)
        {
          dst.x = (int)cx + specs[i].dx;
          dst.y = (int)cy + specs[i].dy;
          dst.h = specs[i].length;
          rect.y = 8 + 8*tile->colors[i];
          
          SDL_RenderCopyEx(Gfx::renderer, Gfx::tiles, &rect, &dst, specs[i].angle, nullptr, SDL_FLIP_NONE);

        }
      }
      
      SDL_SetTextureAlphaMod(Gfx::tiles, 0xFF);
      SDL_SetTextureBlendMode(Gfx::tiles, SDL_BLENDMODE_BLEND);
    }
}

void LevelView::drawInventory(const Field* field, int bx, int by)
{
  for (int x = 0; x < field->invWidth(); ++x)
    for (int y = 0; y < field->invHeight(); ++y)
    {
      const Tile* tile = field->tileAt(Position(Position::Type::INVENTORY, x, y));
      
      if (tile->piece())
      {
        PieceGfx src = gfxForPiece(tile->piece().get());
        SDL_Rect dst = Gfx::ccr(bx+ui::TILE_SIZE*x + 1, by + ui::TILE_SIZE*y + 1, ui::PIECE_SIZE, ui::PIECE_SIZE);
        Gfx::blit(Gfx::tiles, src.rect, dst);
      }
    }
}

void LevelView::drawGrid(int x, int y, int w, int h)
{
  for (int i = 0; i < w; ++i)
  {
    for (int j = 0; j < h; ++j)
    {
      SDL_Rect bgRect = Gfx::ccr(176 + 16 * 0, 266, 15, 15);
      SDL_Rect tileRect = Gfx::ccr(x + i * ui::TILE_SIZE, y + j * ui::TILE_SIZE, 15, 15);
      Gfx::blit(Gfx::tiles, bgRect, tileRect);
    }

    SDL_Rect bgRect = Gfx::ccr(176 + 16 * 0, 266 + 15, 15, 1);
    SDL_Rect tileRect = Gfx::ccr(x + i * ui::TILE_SIZE, y + h * ui::TILE_SIZE, 15, 1);
    Gfx::blit(Gfx::tiles, bgRect, tileRect);
  }

  for (int j = 0; j < h; ++j)
  {
    SDL_Rect bgRect = Gfx::ccr(176 + 16 * 0 + 15, 266, 1, 15);
    SDL_Rect tileRect = Gfx::ccr(x + w * ui::TILE_SIZE, y + j * ui::TILE_SIZE, 1, 15);
    Gfx::blit(Gfx::tiles, bgRect, tileRect);
  }
}

void LevelView::drawTooltip(int x, int y, const std::string& text)
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

  Gfx::rectFill(x, y, width, height, Gfx::ccc(35, 35, 45));
  Gfx::rect(x, y, width, height, Gfx::ccc(4, 4, 8));

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
  
  drawGrid(GFX_FIELD_POS_X, GFX_FIELD_POS_Y, field->width(), field->height());
  drawGrid(inventoryBaseX, GFX_FIELD_POS_Y, field->invWidth(), field->invHeight());
  
  if (position->isValid())
  {
    Gfx::rect(coordX(*position), coordY(*position), ui::TILE_SIZE, ui::TILE_SIZE, Gfx::ccc(180, 0, 0));
    Gfx::rect(coordX(*position) + 1, coordY(*position) + 1, ui::TILE_SIZE - 2, ui::TILE_SIZE - 2, Gfx::ccc(180, 0, 0));
  }
  
  if (selectedTile)
    Gfx::rect(coordX(Position(selectedTile->x, selectedTile->y)), coordY(Position(selectedTile->x, selectedTile->y)), ui::TILE_SIZE, ui::TILE_SIZE, Gfx::ccc(240, 240, 0));
    
  drawField(field, GFX_FIELD_POS_X, GFX_FIELD_POS_Y);
  drawInventory(field, inventoryBaseX, GFX_FIELD_POS_Y);
  
  if (field->level())
    Gfx::drawString(GFX_FIELD_POS_X + field->width()*ui::TILE_SIZE/2, 5, true, field->level()->name + (field->level()->solved ? " \x1D" : ""));

  // 245, 110
  
  constexpr int BASE_X = 80;
  const int BASE_Y = GFX_FIELD_POS_X + field->height()*ui::TILE_SIZE + 20;
  constexpr int STEP = 14;
  
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
    drawPiece(heldPiece.get(), x - ui::PIECE_SIZE / 2, y - ui::PIECE_SIZE / 2);

  if (curTile->piece())
  {
    const auto& piece = curTile->piece();
    
    if (piece->color() != LaserColor::NONE)
      drawTooltip(x, y + 15, (std::string(i18n::nameForPiece(piece->type())) + " (" + ui::textColorForLaser(piece->color()) + i18n::nameForColor(piece->color()) + "^^)").c_str());
    else
      drawTooltip(x, y + 15, i18n::nameForPiece(piece->type()));
  }
  
  if (field->isFailed())
  {
    Gfx::rectFill(GFX_FIELD_POS_X, GFX_FIELD_POS_Y, field->width() * ui::TILE_SIZE, field->height() * ui::TILE_SIZE, Gfx::ccc(180, 0, 0, 128));
    Gfx::drawString(field->width()*ui::TILE_SIZE/2 + GFX_FIELD_POS_X, GFX_FIELD_POS_Y + field->height()*ui::TILE_SIZE/2, true, "FAILED!");
  }
  else if (field->isWon())
  {
    Gfx::rectFill(GFX_FIELD_POS_X, GFX_FIELD_POS_Y, field->width() * ui::TILE_SIZE, field->height() * ui::TILE_SIZE, Gfx::ccc(0, 180, 0, 128));
    Gfx::drawString(field->width()*ui::TILE_SIZE / 2 + GFX_FIELD_POS_X, GFX_FIELD_POS_Y + field->height()*ui::TILE_SIZE / 2, true, "WON!");
  }

  //Gfx::blit(Gfx::tiles, &src, &dst);
  
  //Gfx::drawString(245, 110, "Y: switch zone\nX: rotate left\nA: rotate right\nB: select piece");
}

void LevelView::levelChanged()
{
  Field* field = this->field();
  field->updateLasers();
  field->checkForWin();

  if (field->isWon())
  {
    //TODO: finish
    //_level->solved = true;
    //++Files::packAt(Files::selectedPack)->solvedCount;
  }
}

void LevelView::handleMouseEvent(EventType type, int x, int y, int button)
{
  auto field = this->field();

  if (type == EventType::MOUSE_MOTION)
  {
    Position hover = coordToPosition(x, y);
    
    if (hover.isValid() && field->isInside(hover))
      *position = hover;
    else
      *position = Pos::invalid();
  }
  else if (type == EventType::MOUSE_DOWN)
  {
    Position hover = coordToPosition(x, y);
    
    Tile* tile = field->tileAt(hover);
    
    if (hover.isValid() && tile)
    {
      const auto& piece = tile->piece();
      
      if (button == SDL_BUTTON_LEFT)
      {
        if (!heldPiece && piece && piece->canBeMoved())
        {
          if (piece->isInfinite())
          {
            Piece* dupe = piece->dupe();
            dupe->setInfinite(false);
            heldPiece.reset(dupe);
          }
          else
            tile->swap(heldPiece);

          levelChanged();
        }
        else if (heldPiece && (!piece || piece->canBeMoved()))
        {
          tile->swap(heldPiece);
          levelChanged();
        }
      }
      else if (button == SDL_BUTTON_RIGHT)
      {
        if (piece && piece->canBeRotated())
        {
          piece->rotateRight();
          levelChanged();
        }
      }
    }
  }
}


void LevelView::handleEvent(SDL_Event &event)
{
  auto field = this->field();
  
  switch(event.type)
  {
    case SDL_KEYDOWN:			// Button press
    {
      Pos*& p = position;
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
            levelChanged();
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
        /*case SDLK_LCTRL: // A
        {
          const auto& piece = field->tileAt(*position)->piece();
          if (piece && piece->canBeRotated())
          {
            piece->rotateRight();
            levelChanged();
          }
          
          break;
        }*/
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
              levelChanged();
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
            levelChanged();
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
