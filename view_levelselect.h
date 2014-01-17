//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _VIEW_LEVEL_SELECT_H_
#define _VIEW_LEVEL_SELECT_H_

#include "game.h"

class View;

template<class T>
class OffsettableList
{
  protected:
    u32 offset;
  
  public:
    OffsettableList(u32 listSize) : offset(0), LIST_SIZE(listSize) { }
  
    u32 getOffset() { return offset; }
    void changeOffset(int delta) { offset += delta; }
  
    u32 minOffset() { return offset; }
    u32 maxOffset() { return offset + LIST_SIZE - 1; }
    bool isSelected(u32 i) { return i + offset == current(); }
  
    virtual u32 current() = 0;
    virtual u32 count() = 0;
    virtual void set(u32 i) = 0;
    virtual T get(u32 i) = 0;
  
    bool down() {
      if (current() < count() - 1)
      {
        set(current()+1);
        
        if (current() > maxOffset())
          ++offset;
        
        return true;
      }      
      return false;
    }
  
    bool up() {
      if (current() > 0)
      {
        set(current()-1);
        
        if (current() < minOffset())
          --offset;
        
        return true;
      }
      return false;
    }
  
    bool nextPage() {
      if (current() < maxOffset())
      {
        set(maxOffset());
        return true;
      }
      else
      {
        if (offset < count()-1)
        {
          if (current()+LIST_SIZE <= count()-1)
          {
            offset += LIST_SIZE;
            set(current()+LIST_SIZE);
          }
          else
          {
            offset = count() - LIST_SIZE;
            set(count()-1);
          }
          return true;
        }
      }
      return false;
    }
    
    bool prevPage() {
      if (current() > minOffset())
      {
        set(minOffset());
        return true;
      }
      else
      {
        if (offset > 0)
        {
          u8 delta = LIST_SIZE <= offset ? LIST_SIZE : offset;
          offset -= delta;
          set(current()-delta);
          return true;
        }
      }
      return false;
    }
  
    const u32 LIST_SIZE;
};


class LevelList : public OffsettableList<LevelSpec*>
{
  private:
    Game *game;
    
  public:
    LevelList(Game *game) : OffsettableList(14), game(game) { }
    
    u32 current() { return game->pack->selected; }
    u32 count() { return game->pack->count(); }
    void set(u32 i) { game->pack->selected = i; }
    LevelSpec*get(u32 i) { return game->pack->at(offset+i); }
};

class LevelSelectView : public View
{
private:
  SDL_Surface *preview;
  SDL_Surface *scaledPreview;
  Field *field;
  
  LevelList levelList;
  
public:
  LevelSelectView(Game *game);
  void handleEvent(SDL_Event &event);
  void draw();
  void rebuildPreview();
  
  void activate();
};

#endif
