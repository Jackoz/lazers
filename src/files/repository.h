#pragma once

#include <vector>
#include <string>

#include "common/common.h"

struct PieceInfoSpec
{
  PieceType type;
  u8 mapping;
  bool canBeColored;
  bool canBeRotated;
};

struct PieceInfo
{
  bool inventory : 1;
  PieceType type : 7;
  s8 x, y;
  LaserColor color : 3;
  Direction direction : 4;
  bool moveable : 1;
  bool roteable : 1;
  
  PieceInfo(PieceType type) : type(type) { }
} __attribute__((packed));

struct LevelSpec
{
private:
  std::vector<PieceInfo> pieces;
  
public:
  LevelSpec(const std::string& name) : name(name), solved(false) { }
  
  void add(PieceInfo piece) { pieces.push_back(piece); }
  size_t count() const { return pieces.size(); }
  const PieceInfo& at(size_t index) const { return pieces[index]; }
  
  std::string name;
  bool solved;
};

class LevelPack
{
private:
  std::vector<LevelSpec> levels;
  
  std::string _name;
  std::string _author;
  std::string _path;
  
public:
  LevelPack(const std::string& name, const std::string& author, const std::string& path) : _name(name), _author(author), _path(path), selected(0), solvedCount(0) { }
  void addLevel(LevelSpec level) { levels.push_back(level); }
  
  size_t count() const { return levels.size(); }
  const LevelSpec *at(u32 index) const { return &levels[index]; }
  
  const std::string& author() const { return _author; }
  const std::string& name() const { return _name; }
  const std::string& path() const { return _path; }
  
  mutable u32 selected;
  u32 solvedCount;
};

class Repository
{
private:
  std::vector<LevelPack> packs;
  
public:
  size_t packCount() { return static_cast<size_t>(packs.size()); }
  void move(std::vector<LevelPack>& elems) { packs.insert(packs.end(), std::make_move_iterator(elems.begin()), std::make_move_iterator(elems.end())); }
  void add(const LevelPack& pack) { packs.push_back(pack); }
  const LevelPack& operator[](size_t index) { return packs[index]; }
  
  auto begin() const { return packs.begin(); }
  auto end() const { return packs.end(); }
};
