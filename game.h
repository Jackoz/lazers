//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _GAME_H_
#define _GAME_H_

#include "common.h"
#include "level.h"

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
  
  bool hasNext(u32 i) { return i < LIST_SIZE && i + offset < count(); }
  
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


enum ViewType
{
  VIEW_LEVEL = 0,
  VIEW_LEVEL_SELECT = 1,
  VIEW_PACK_SELECT = 2,
  
  VIEWS_COUNT
};

class View
{
  protected:
    Game *game;
  
  public:
    View(Game *game) : game(game) { }
    virtual void handleEvent(SDL_Event &event) = 0;
    virtual void draw() = 0;
  
    virtual void activate() { };
    virtual void deactivate() { };
};




class Game
{
  public:
    Field *field;

  private:
    bool running;
    View* views[VIEWS_COUNT];
    View* view;
    View* overView;
  
    void handleEvents();

  public:
    Game();
    void init();
    void loop();
    void quit() { Files::saveSolvedStatus(); running = false; }
  
    void switchView(ViewType type);
  
    LevelPack *pack;
    
};

#endif
