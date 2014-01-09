//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "lazers.h"

#include <list>

using namespace std;

SDL_PixelFormat *Gfx::format = nullptr;

SDL_Surface *Gfx::screen = nullptr;
#ifdef SCALE
  SDL_Surface *Gfx::realScreen = nullptr;
#endif


SDL_Surface *Gfx::tiles = nullptr;

const s16 Gfx::GFX_LASER_ROWS[] = {-1, 2, 3, 4, 5, 6, 7, 8};
const s16 Gfx::GFX_PIECE_ROWS[] = {-1, 1, -1, 0};
const u16 Gfx::GFX_FIELD_POS_X = 0;
const u16 Gfx::GFX_FIELD_POS_Y = 10;
const u16 Gfx::GFX_INVENTORY_POS_X = TILE_SIZE*FIELD_WIDTH + 5;

void Gfx::init()
{
  SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
  
	//SDL_ShowCursor(SDL_DISABLE);
#ifdef SCALE
	realScreen = SDL_SetVideoMode(WIDTH*SCALE, HEIGHT*SCALE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  screen = SDL_CreateRGBSurface(SDL_HWSURFACE, WIDTH, HEIGHT, 32, realScreen->format->Rmask, realScreen->format->Gmask, realScreen->format->Bmask, realScreen->format->Amask);
#else
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
#endif
  
	if (screen->format->BitsPerPixel != 32) {
    fprintf(stderr, "ERROR: Did not get 32 bpp, got %u bpp instead.\n",
            screen->format->BitsPerPixel);
    exit(1);
	}
	if (!(screen->flags & SDL_HWSURFACE)) {
		fprintf(stderr, "WARNING: Did not get a hardware surface.\n");
	}
	if (!(screen->flags & SDL_DOUBLEBUF)) {
		fprintf(stderr, "WARNING: Did not get double buffering.\n");
	}
  
  Gfx::setFormat(screen->format);
  
  #ifdef SCALE
    tiles = IMG_Load("/Users/jack/Documents/Dev/xcode/lazers/Lazers/Lazers/data/mirror.png");
  #else
    tiles = IMG_Load("mirror.png");
  #endif
  
  SDL_EnableKeyRepeat(300/*SDL_DEFAULT_REPEAT_DELAY*/, 80/*SDL_DEFAULT_REPEAT_INTERVAL*/);
}

void Gfx::blit(SDL_Surface *src, SDL_Surface *dst, u16 sx, u16 sy, u16 dx, u16 dy, u16 w, u16 h, u16 f)
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


void Gfx::line(u16 x1, u16 y1, u16 x2, u16 y2, u32 color)
{
  bool hor = y1 == y2;
  u32* p = (u32*)screen->pixels;

  if (hor)
    for (u16 x = x1; x <= x2; ++x)
      p[y1*screen->w + x] = color;
  else
    for (u16 y = y1; y <= y2; ++y)
      p[y*screen->w + x1] = color;
}

void Gfx::rect(u16 x, u16 y, u16 w, u16 h, u32 color)
{
  line(x, y, x, y+h, color);
  line(x, y, x+w, y, color);
  line(x+w, y, x+w, y+h, color);
  line(x, y+h, x+w, y+h, color);
}

void Gfx::rectFill(s16 x1, s16 y1, u16 x2, u16 y2, u32 color)
{
  SDL_Rect rect = {x1,y1,static_cast<u16>(x2-x1+1),static_cast<u16>(y2-y1+1)};
  SDL_FillRect(screen, &rect, color);
  
  /*u32* p = (u32*)screen->pixels;
  
  for (int x = x1; x <= x2; ++x)
    for (int y = y1; y <= y2; ++y)
      p[y*screen->w + x] = color;*/
}

void Gfx::clear(u32 color)
{
  SDL_Rect frect = {0,0,WIDTH,HEIGHT};
  SDL_FillRect(screen, &frect, color);
}




void Gfx::render(Game *game)
{
  clear(ccc(220, 220, 220));
  
  //rect = {0,0,20,20};
  //SDL_BlitSurface(Gfx::tiles, &rect, screen, &rect);
  
  lock();
  
  u32 color = ccc(180, 180, 180);
  
  for (int i = 0; i < FIELD_WIDTH+1; ++i)
    line(coordX(i, false), coordY(0), coordX(i, false), coordY(FIELD_HEIGHT), color);
  
  for (int i = 0; i < FIELD_HEIGHT+1; ++i)
    line(coordX(0, false), coordY(i), coordX(FIELD_WIDTH, false), coordY(i), color);
  
  for (int i = 0; i < INVENTORY_WIDTH+1; ++i)
    line(coordX(i+FIELD_WIDTH, true), coordY(0), coordX(i+FIELD_WIDTH, true), coordY(INVENTORY_HEIGHT), color);
  
  for (int i = 0; i < INVENTORY_HEIGHT+1; ++i)
    line(coordX(FIELD_WIDTH, true), coordY(i), coordX(FIELD_WIDTH+INVENTORY_WIDTH, true), coordY(i), color);
  
  rect(coordX(game->position->x, true), coordY(game->position->y), TILE_SIZE, TILE_SIZE, ccc(180, 0, 0));
  
  if (game->selectedTile())
  {
    rect(coordX(game->selectedTile()->x, true), coordY(game->selectedTile()->y), TILE_SIZE, TILE_SIZE, ccc(240, 240, 0));
  }
  
  unlock();
  
  drawField(game);
  
  #ifdef SCALE
    blit(screen, realScreen, 0, 0, 0, 0, WIDTH, HEIGHT, SCALE);
    SDL_Flip(realScreen);
  #else
    SDL_Flip(screen);
  #endif
}

void Gfx::drawField(Game *game)
{
  Field *field = game->field();
  
  for (int x = 0; x < FIELD_WIDTH; ++x)
    for (int y = 0; y < FIELD_HEIGHT; ++y)
    {
      Tile *tile = field->tileAt(x,y);
      
      if (tile->piece)
      {
        SDL_Rect src = {static_cast<s16>((PIECE_SIZE)*tile->piece->rotation()),static_cast<s16>(PIECE_SIZE*GFX_PIECE_ROWS[tile->piece->type()]),PIECE_SIZE,PIECE_SIZE};
        SDL_Rect dst = {static_cast<s16>(coordX(x, false)+1),static_cast<s16>(coordY(y)+1),0,0};
        SDL_BlitSurface(tiles, &src, screen, &dst);
      }
      
      for (int i = 0; i < 8; ++i)
      {
        if (tile->colors[i])
        {
          SDL_Rect src = {static_cast<s16>((PIECE_SIZE)*i),static_cast<s16>(PIECE_SIZE*GFX_LASER_ROWS[tile->colors[i]]),PIECE_SIZE,PIECE_SIZE};
          SDL_Rect dst = {static_cast<s16>(coordX(x, false)+1),static_cast<s16>(coordY(y)+1),0,0};
          SDL_BlitSurface(tiles, &src, screen, &dst);
        }
      }
    }
  
  for (int x = 0; x < INVENTORY_WIDTH; ++x)
    for (int y = 0; y < INVENTORY_HEIGHT; ++y)
    {
      Tile *tile = field->tileAt(x+FIELD_WIDTH,y);
      
      if (tile->piece)
      {
        SDL_Rect src = {static_cast<s16>((PIECE_SIZE)*tile->piece->rotation()),static_cast<s16>(PIECE_SIZE*GFX_PIECE_ROWS[tile->piece->type()]),PIECE_SIZE,PIECE_SIZE};
        SDL_Rect dst = {static_cast<s16>(coordX(x+FIELD_WIDTH, true)+1),static_cast<s16>(coordY(y)+1),0,0};
        SDL_BlitSurface(tiles, &src, screen, &dst);
      }
    }
}




void Field::updateLasers()
{
  list<Laser> lasers;
  
  for (int i = 0; i < FIELD_WIDTH; ++i)
    for (int j = 0; j < FIELD_HEIGHT; ++j)
    {
      Tile *tile = tileAt(i,j);
      tile->resetLasers();
      Piece *piece = tile->piece;
      
      if (piece && piece->type() == PIECE_SOURCE)
      {
        Laser laser = Laser(Position(i+directions[piece->rotation()][0],j+directions[piece->rotation()][1]), piece->rotation(), piece->color());
        lasers.push_back(laser);
      }
    }
  
  list<Laser>::iterator it = lasers.begin();
  while (!lasers.empty())
  {
    bool toRemove = true;

    Laser& laser = (*it);
    
    while (laser.position.isValid())
    {
      Tile *tile = tileAt(laser.position);
      
      tile->colors[laser.direction] |= laser.color;
      tile->colors[(laser.direction+4)%8] |= laser.color;
      
      laser.position.x += directions[laser.direction][0];
      laser.position.y += directions[laser.direction][1];
    }
    
    
    
    if (toRemove)
      it = lasers.erase(it);
    else
      ++it;
  }
}



void Game::init()
{

  Gfx::init();
  running = true;
}

void Game::handleEvents()
{
  SDL_Event event;
  
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT: running = false; break;
      case SDL_KEYDOWN:			// Button press
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE: running = false; break;
            
          case SDLK_LEFT:
          {
            if ((position == &fposition && position->x > 0) || position->x > FIELD_WIDTH)
              --position->x;

            break;
          }
          case SDLK_RIGHT:
          {
            if ((position == &fposition && position->x < FIELD_WIDTH-1) || position->x < FIELD_WIDTH+INVENTORY_WIDTH-1)
              ++position->x;
            break;
          }
          case SDLK_UP:
          {
            if (position->y > 0)
              --position->y;
            break;
          }
            
          case SDLK_DOWN:
          {
            if ((position == &fposition && position->y < FIELD_HEIGHT-1) || position->y < FIELD_HEIGHT-1)
              ++position->y;
            break;
          }
            
          //   Y
          //  X A
          //   B
            
          case SDLK_LCTRL: // A
          {
            position = position == &iposition ? &fposition : &iposition;
            break;
          }
            
          case SDLK_LALT: // B
          {
            Tile *curTile = field_.tileAt(*position);
            
            if (!selectedTile_ && curTile->piece)
            {
              selectedTile_ = curTile;
            }
            else if (selectedTile_ == curTile)
              selectedTile_ = nullptr;
            else if (selectedTile_)
            {
              std::swap(selectedTile_->piece, curTile->piece);
              selectedTile_ = nullptr;
              field_.updateLasers();
            }
            
            break;
          }
            
          case SDLK_LSHIFT: // X
          {
            
            break;
          }
            
          case SDLK_SPACE: // Y
          {
            
            break;
          }
            
          case SDLK_TAB: // L
          {
            Piece *piece = field_.tileAt(position->x, position->y)->piece;
            if (piece)
            {
              piece->rotateLeft();
              field_.updateLasers();
            }
            
            
            break;
          }
            
          case SDLK_BACKSPACE: // R
          {
            Piece *piece = field_.tileAt(position->x, position->y)->piece;
            if (piece)
            {
              piece->rotateRight();
              field_.updateLasers();
            }
            
            break;
          }
            
          default: break;
        }
        break;
      }
    }
  }
}



void Game::loop()
{
  while (running)
  {
    Gfx::render(this);
    handleEvents();
  }
}
