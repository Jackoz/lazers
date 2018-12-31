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
    /*std::exit(EXIT_FAILURE);*/ assert(false); \
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
    case '#': return PIECE_WALL; // wall for title
    case 'w': return PIECE_WALL;
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
    case 'M': return PIECE_FLIPPED_PRISM;
    case '%': return PIECE_DOUBLE_MIRROR;
    case 'W': return PIECE_TUNNEL;
    case 'm': return PIECE_MINE;
    case ':': return PIECE_POLARIZER;
    case 'y': return PIECE_THREE_WAY_SPLITTER;
    case 'I': return PIECE_SELECTOR;
    case 'L': return PIECE_SPLICER;
    case '@': return PIECE_BENDER;
      
    case 'p': return PIECE_UFO;
    case 'c': return PIECE_CRYSTAL;
    case 'h': return PIECE_VOID_HOLE;
      
    default:
      assert(false);
      return PIECE_UNKNOWN;
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
    case PIECE_FLIPPED_PRISM: return NORTH;
    case PIECE_PRISM: return NORTH;
    case PIECE_DOUBLE_MIRROR: return SOUTH;
    case PIECE_TUNNEL: return NORTH;
    case PIECE_POLARIZER: return NORTH;
    case PIECE_SELECTOR: return EAST;
    case PIECE_SPLICER: return EAST;

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
    case PIECE_FLIPPED_PRISM:
    case PIECE_DOUBLE_MIRROR:
    case PIECE_TUNNEL:
    case PIECE_POLARIZER:
    case PIECE_SELECTOR:
    case PIECE_SPLICER:
    case PIECE_THREE_WAY_SPLITTER:
      return true;
      
    case PIECE_STRICT_GOAL:
    case PIECE_FILTER:
    case PIECE_TNT:
    case PIECE_SLIME:
    case PIECE_MINE:
    case PIECE_UFO:
    case PIECE_CRYSTAL:
    case PIECE_VOID_HOLE:
    case PIECE_WALL:
    case PIECE_BENDER:
      return false;
      
    default: return false;
  }
}

LaserColor colorForChar(char c)
{
  switch (c) {
    case ' ': return LaserColor::WHITE;
    case 'R': return LaserColor::RED;
    case 'G': return LaserColor::GREEN;
    case 'B': return LaserColor::BLUE;
    case 'M': return LaserColor::MAGENTA;
    case 'C': return LaserColor::CYAN;
    case 'Y': return LaserColor::YELLOW;
    case 'X': return LaserColor::NONE;
    default:  ASSERT(false, "color is not a legal character (" << c << ")"); return LaserColor::NONE;
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
    case PIECE_WALL:
    case PIECE_THREE_WAY_SPLITTER:
    case PIECE_BENDER:
    case PIECE_FLIPPED_PRISM:
      return false;
      
    case PIECE_STRICT_GOAL:
    case PIECE_FILTER:
    case PIECE_SOURCE:
    case PIECE_POLARIZER:
    case PIECE_SELECTOR:
    case PIECE_SPLICER:
      return true;
      
    default: return false;
  }
}

LevelSpec Aargon::parseLevel(const string& filename)
{
  ifstream in = ifstream(filename);
  
  //cout << "parsing " << filename << endl;

  assert(!in.fail());
  
  string line;
  vector<string> lines;
  
  const std::string content = std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
  const size_t length = content.length();

  for (size_t i = 0; i < length; ++i)
  {
    const char c = content[i];

    if (c == '\r' || c == '\n')
    {
      if (c == '\r')
      {
        assert(i < length - 1 && content[i + 1] == '\n');
        ++i;
      }

      if (!line.empty())
        lines.push_back(line);
      line.clear();
    }
    else
      line += c;
  }

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
    if (tmpName[i] >= 'A' && tmpName[i] <= 'Z' && i > 0 && tmpName[i-1] != ' ')
      nameBuffer << static_cast<char>(tmpName[i] - 'A' + 'a');
    else
      nameBuffer << tmpName[i];
  }
  
  LevelSpec spec(nameBuffer.str());
  
  int INVENTORY_ROWS;
  if (lines[1][12] == '=')
    INVENTORY_ROWS = 2;
  else if (lines[1][8] == '=')
    INVENTORY_ROWS = 1;
  else
    INVENTORY_ROWS = 0;
  
  for (int i = 1; i < 12; ++i)
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
                
        info.inventory = INVENTORY_ROWS > 0 && j <= INVENTORY_ROWS;
        
        if (!info.inventory)
        {
          info.x = INVENTORY_ROWS == 0 ? (j - (INVENTORY_ROWS + 3)) : (j - (INVENTORY_ROWS + 2));
          info.y = i - 1;
        }
        
        //ASSERT(info.inventory || (info.x >= 0 && info.y >= 0), "piece coordinate must be greater than 0");
        
        //ASSERT(canHaveRotation(pieceType) || data[1] == ' ', "direction is specified for piece " << data[0] << ": " << data[1] << "  " << i << "," << (j*4));
        
        if (canHaveRotation(pieceType))
          info.direction = static_cast<Direction>((baseDirection(pieceType) + direction)%8);
        else
          info.direction = NORTH;
        
        ASSERT(!canHaveColor(pieceType) || color != LaserColor::NONE || pieceType == PIECE_STRICT_GOAL || pieceType == PIECE_FILTER, "X color specifier found for not a goal");
        //ASSERT(canHaveColor(pieceType) || data[2] == ' ' || pieceType == PIECE_WALL, "color is specified for piece " << data[0]);
        
        if (canHaveColor(pieceType))
          info.color = color;
        else
          info.color = LaserColor::NONE;
      
        switch (data[3])
        {
          case '.': info.moveable = true; info.roteable = true; break;
          case 'n': info.moveable = false; info.roteable = false; break;
          case 'r': info.moveable = false; info.roteable = true; break;
          case 'm': info.moveable = true; info.roteable = false; break;
        }
        
        if (info.type == PIECE_FILTER && info.color == LaserColor::WHITE)
        {
          info.type = PIECE_GLASS;
          info.color = LaserColor::NONE;
        }
        
        spec.add(info);
      }
    }
  }
  
  return spec;
}

struct AargonPack
{
  string name;
  string author;
  string filename;
  
  string folderName;
  int skillNumber;
};

std::vector<LevelPack> Aargon::parseLevels()
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
  
  // Classic 4 - DONE
  // Creepy 3 - DONE
  // Deluxe 4 - DONE
  // Demo Level Set - SKIP?
  // Hospital 4 - DONE
  // Level Pack 1 4 - DONE
  // Smooth Sailing - DONE
  // Tutorial - SKIP?
  
  vector<AargonPack> apacks;
  apacks.push_back({"Aargon Deluxe (Easy)", "Twilight Games", "aargon-deluxe-00", "Deluxe", 1});
  apacks.push_back({"Aargon Deluxe (Medium)", "Twilight Games", "aargon-deluxe-01", "Deluxe", 2});
  apacks.push_back({"Aargon Deluxe (Hard)", "Twilight Games", "aargon-deluxe-02", "Deluxe", 3});
  apacks.push_back({"Aargon Deluxe (Extreme)", "Twilight Games", "aargon-deluxe-03", "Deluxe", 4});

  apacks.push_back({ "Aargon Classic (Beginner)", "Twilight Games", "aargon-classic-00", "Classic", 1 });
  apacks.push_back({ "Aargon Classic (Easy)", "Twilight Games", "aargon-classic-01", "Classic", 2 });
  apacks.push_back({ "Aargon Classic (Hard)", "Twilight Games", "aargon-classic-02", "Classic", 3 });
  apacks.push_back({ "Aargon Classic (Expert)", "Twilight Games", "aargon-classic-03", "Classic", 4 });
  
  apacks.push_back({ "Aargon Space Station (Training)", "Twilight Games", "aargon-space-station-00", "Level Pack 1", 1 });
  apacks.push_back({ "Aargon Space Station (Recreation)", "Twilight Games", "aargon-space-station-01", "Level Pack 1", 2 });
  apacks.push_back({ "Aargon Space Station (Engineering)", "Twilight Games", "aargon-space-station-02", "Level Pack 1", 3 });
  apacks.push_back({ "Aargon Space Station (Intelligence)", "Twilight Games", "aargon-space-station-03", "Level Pack 1", 4 });
  
  std::vector<LevelPack> packs;
  
  for (const AargonPack &apack : apacks)
  {
    LevelPack npack = LevelPack(apack.name, apack.author, apack.filename);

    for (int i = 1; i <= 30; ++i)
    {
      string base = "aargon/";// "/Users/jack/Documents/dev/c++/lazers/data/packs/aargon/";
      stringstream ss;
      ss << base << apack.folderName << "/" << "Levels/SKILL";
      ss << apack.skillNumber << "/";
      ss << "Level_0";
      if (i < 10) ss << '0';
      ss << i << ".map";
      
      const std::string path = ss.str();
      
      npack.addLevel(parseLevel(path));
    }
    
    packs.push_back(npack);
  }
  
  return packs;
}
