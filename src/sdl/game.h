//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _GAME_H_
#define _GAME_H_

#include "common/common.h"
#include "core/level.h"

template<class T>
class OffsettableList
{
protected:
  size_t offset;
  
public:
  OffsettableList(size_t listSize) : offset(0), LIST_SIZE(listSize) { }
  
  size_t getOffset() { return offset; }
  void changeOffset(int delta) { offset += delta; }
  
  size_t minOffset() { return offset; }
  size_t maxOffset() { return offset + LIST_SIZE - 1; }
  bool isSelected(size_t i) { return i + offset == current(); }
  void reset() { offset = 0; }
  
  bool isValidIndex(size_t i) const { return offset + i < count(); }
  
  virtual size_t current() const = 0;
  virtual size_t count() const = 0;
  virtual void set(size_t i) = 0;
  virtual T get(size_t i) const  = 0;
  
  bool hasNext(size_t i) { return i < LIST_SIZE && i + offset < count(); }
  
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
  
  const size_t LIST_SIZE;
};


enum ViewType
{
  VIEW_LEVEL = 0,
  VIEW_LEVEL_SELECT = 1,
  VIEW_PACK_SELECT = 2,
  
  VIEWS_COUNT
};

union SDL_Event;

class View
{
protected:
  Game *game;
  
public:
  View(Game *game) : game(game) { }
  virtual void handleEvent(SDL_Event& event) = 0;
  virtual void draw() = 0;
  
  virtual void activate() { };
  virtual void deactivate() { };
};




class Game
{
public:
  Field *field;
  Repository packs;
  
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
  
  const LevelPack *pack;
};

#endif
