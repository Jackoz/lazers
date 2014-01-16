//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "aargon.h"

#include "files.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include <cassert>

#define ASSERT(condition, message) \
do { \
  if (! (condition)) { \
    std::cerr << "Assertion '" #condition "' failed in " << __FILE__ \
    << " line " << __LINE__ << ": " << message << std::endl; \
    std::exit(EXIT_FAILURE); \
    } \
    } while (false)

using namespace std;

static string NAME_PREFIX = string("MAPTEXT:[");
static string SCRIPT_PREFIX = string("SCRIPT:[");
static string COPYRIGHT_PREFIX = string("COPYRIGHT:[");

static const char PIECE_IGNORE = -20;
static const char PIECE_UNKNOWN = -1;

static const char DIRECTION_IGNORE = -1;

s8 pieceType(char c)
{
  switch (c) {
    case '=': return PIECE_IGNORE; // wall
    case '#': return PIECE_IGNORE; // wall for title
    case ' ': return PIECE_IGNORE; // empty
      
    case '<': return PIECE_SPLITTER;
    case '/': return PIECE_MIRROR;
    case 'O': return PIECE_STRICT_GOAL;
    case 'F': return PIECE_FILTER;
    case 'S': return PIECE_SOURCE;
    case 'X': return PIECE_TNT;
    case 's': return PIECE_SLIME;
    case '+': return PIECE_DOUBLE_PASS_MIRROR;
    case 'T': return PIECE_DSPLITTER;
    case 'J': return PIECE_COLOR_SHIFTER;
    case 'R': return PIECE_REFRACTOR;
    case 'P': return PIECE_PRISM;
    case '%': return PIECE_DOUBLE_MIRROR;
    case 'W': return PIECE_TUNNEL;
    case 'm': return PIECE_MINE;
    case ':': return PIECE_POLARIZER;
      
    case 'I': return PIECE_SELECTOR;
    case 'p': return PIECE_UFO;
    case 'c': return PIECE_CRYSTAL;
    case 'h': return PIECE_VOID_HOLE;
      
    default: return PIECE_UNKNOWN;
  }
}

s8 baseDirection(PieceType type)
{
  switch (type) {
    case PIECE_SPLITTER: return SOUTH;
    case PIECE_MIRROR: return SOUTH;
    case PIECE_SOURCE: return NORTH;
    case PIECE_DOUBLE_PASS_MIRROR: return SOUTH;
    case PIECE_DSPLITTER: return SOUTH;
    case PIECE_COLOR_SHIFTER: return EAST; // check
    case PIECE_REFRACTOR: return NORTH;
    case PIECE_PRISM: return NORTH;
    case PIECE_DOUBLE_MIRROR: return SOUTH;
    case PIECE_TUNNEL: return NORTH;
    case PIECE_POLARIZER: return NORTH;
    case PIECE_SELECTOR: return EAST; // check
      
    default: return DIRECTION_IGNORE;
  }
}

bool canHaveRotation(PieceType type)
{
  switch (type) {
    case PIECE_SPLITTER:
    case PIECE_MIRROR:
    case PIECE_SOURCE:
    case PIECE_DOUBLE_PASS_MIRROR:
    case PIECE_DSPLITTER:
    case PIECE_COLOR_SHIFTER:
    case PIECE_REFRACTOR:
    case PIECE_PRISM:
    case PIECE_DOUBLE_MIRROR:
    case PIECE_TUNNEL:
    case PIECE_POLARIZER:
    case PIECE_SELECTOR:
      return true;
      
    case PIECE_STRICT_GOAL:
    case PIECE_FILTER:
    case PIECE_TNT:
    case PIECE_SLIME:
    case PIECE_MINE:
    case PIECE_UFO:
    case PIECE_CRYSTAL:
    case PIECE_VOID_HOLE:
      return false;
      
    default: return false;
  }
}

LaserColor colorForChar(char c)
{
  switch (c) {
    case ' ': return COLOR_WHITE;
    case 'R': return COLOR_RED;
    case 'G': return COLOR_GREEN;
    case 'B': return COLOR_BLUE;
    case 'M': return COLOR_MAGENTA;
    case 'C': return COLOR_CYAN;
    case 'Y': return COLOR_YELLOW;
    case 'X': return COLOR_NONE;
    default:  ASSERT(false, "color is not a legal character (" << c << ")"); return COLOR_NONE;
  }
}

bool canHaveColor(PieceType type)
{
  switch (type) {
    case PIECE_SPLITTER:
    case PIECE_MIRROR:
    case PIECE_TNT:
    case PIECE_SLIME:
    case PIECE_DOUBLE_PASS_MIRROR:
    case PIECE_DSPLITTER:
    case PIECE_COLOR_SHIFTER:
    case PIECE_REFRACTOR:
    case PIECE_PRISM:
    case PIECE_DOUBLE_MIRROR:
    case PIECE_TUNNEL:
    case PIECE_MINE:
    case PIECE_UFO:
    case PIECE_CRYSTAL:
    case PIECE_VOID_HOLE:
      return false;
      
    case PIECE_STRICT_GOAL:
    case PIECE_FILTER:
    case PIECE_SOURCE:
    case PIECE_POLARIZER:
    case PIECE_SELECTOR:
      return true;
      
    default: return false;
  }
}

LevelSpec Aargon::parseLevel(string filename)
{
  ifstream in = ifstream(filename);
  
  cout << "parsing " << filename << endl;
  
  string line;
  vector<string> lines;
  
  while (getline(in, line))
    lines.push_back(line.substr(0, line.length()-1));
  
  assert(lines.size() == 16);
  
  for (int i = 0; i < 13; ++i)
    assert(lines[i].length() == 20*4);
  
  assert(lines[13].compare(0, NAME_PREFIX.length(), NAME_PREFIX) == 0);
  assert(lines[14].compare(0, SCRIPT_PREFIX.length(), SCRIPT_PREFIX) == 0);
  assert(lines[15].compare(0, COPYRIGHT_PREFIX.length(), COPYRIGHT_PREFIX) == 0);
  
  string tmpName = string(lines[13].substr(NAME_PREFIX.length(), lines[13].length() - NAME_PREFIX.length() - 1));
  stringstream nameBuffer;
  
  for (int i = 0; i < tmpName.length(); ++i)
  {
    if (i == 0 || tmpName[i-1] == ' ' || tmpName[i] == ' ')
      nameBuffer << tmpName[i];
    else if (tmpName[i] >= 'A' && tmpName[i] <= 'Z')
      nameBuffer << static_cast<char>(tmpName[i] - 'A' + 'a');
  }
  
  LevelSpec spec(nameBuffer.str());
  
  int INVENTORY_ROWS = 1;
  if (lines[1][12] == '=')
    INVENTORY_ROWS = 2;
  
  for (int i = 0; i < 13; ++i)
  {
    const char *line = lines[i].c_str();
    char *data;
    
    for (int j = 0; j < 20; ++j)
    {
      data = const_cast<char*>(line + 4*j);
      
      s8 type = pieceType(data[0]);
                  
      if (type == PIECE_UNKNOWN)
      {
        ASSERT(false, "unknown piece in " << filename << ": " << data[0] << " (" << (int)data[0] << ")");
      }
      else if (type != PIECE_IGNORE)
      {
        ASSERT(data[3] == '.' || data[3] == 'n' || data[3] == 'r' || data[3] == 'm', "movable is not a legal character (" << data[3] << ")");
        
        ASSERT(data[1] == ' ' || (data[1] >= '1' && data[1] <= '7'), "direction is not a legal character (" << data[1] << ")");
        
        s8 direction = data[1] == ' ' ? 0 : (data[1] - '1' + 1);
        LaserColor color = colorForChar(data[2]);
        
        PieceType pieceType = static_cast<PieceType>(type);
        
        PieceInfo info = PieceInfo(pieceType);
        
        info.inventory = j <= INVENTORY_ROWS;
        
        if (!info.inventory)
        {
          info.x = j - (INVENTORY_ROWS + 2);
          info.y = i - 1;
        }
        
        //ASSERT(canHaveRotation(pieceType) || data[1] == ' ', "direction is specified for piece " << data[0] << ": " << data[1] << "  " << i << "," << (j*4));
        
        if (canHaveRotation(pieceType))
          info.direction = static_cast<Direction>(baseDirection(pieceType) + direction);
        else
          info.direction = NORTH;
        
        ASSERT(!canHaveColor(pieceType) || color != COLOR_NONE || pieceType == PIECE_STRICT_GOAL, "X color specifier found for not a goal");
        ASSERT(canHaveColor(pieceType) || data[2] == ' ', "color is specified for piece " << data[0]);
        
        if (canHaveColor(pieceType))
          info.color = color;
        else
          info.color = COLOR_NONE;
      
        switch (data[3])
        {
          case '.': info.moveable = true; info.roteable = true; break;
          case 'n': info.moveable = false; info.roteable = false; break;
          case 'r': info.moveable = false; info.roteable = true; break;
          case 'm': info.moveable = true; info.roteable = false; break;
        }
        
        spec.add(info);
      }
    }
  }
  
  return spec;
}

void Aargon::parseLevels()
{
  /*string base = "/Users/jack/Desktop/Twilight/Aargon Deluxe/Level Packs/";
  string packs[] = {"Tutorial", "Deluxe", "Classic", "Smooth Sailing", "Demo Level Set", "Level Pack 1"};
  int amount[] = {2, 4, 4, 4, 3, 1};
  
  for (int p = 0; p < sizeof(packs)/sizeof(packs[0]); ++p)
  {
    for (int s = 0; s < amount[p]; ++s)
    {
      for (int i = 1; i <= 30; ++i)
      {
        stringstream ss;
        ss << base;
        ss << packs[p] << "/Levels/SKILL";
        ss << (1+s) << "/";
        ss << "Level_0";
        if (i < 10) ss << '0';
        ss << i << ".map";
        parseLevel(ss.str());
      }
    }
  }*/
  
  for (int i = 1; i <= 30; ++i)
  {
    string base = "/Users/jack/Desktop/lazers/SKILL1/";
    stringstream ss;
    ss << base;
    ss << "Level_0";
    if (i < 10) ss << '0';
    ss << i << ".map";
    Files::packAt(0)->addLevel(parseLevel(ss.str()));
  }
}