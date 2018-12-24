//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "files.h"

#include "level.h"
#include <dirent.h>

using namespace std;

const u8 base64map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

u8 value(u8 v)
{
  if (v >= 'A' && v <= 'Z')
    return v - 'A';
  else if (v >= 'a' && v <= 'z')
    return v - 'a' + 26;
  else if (v >= '0' && v <= '9')
    return v - '0' + 26 + 26;
  else if (v == '+')
    return 26+26+10;
  else
    return 26+26+10+1;
}

void Files::encode(const u8 *input, size_t length, char **outputPtr, size_t *outputLength)
{
  size_t steps = length/3;
  *outputLength = steps*4;
  size_t extraBytes = length%3;
  
  if (extraBytes != 0)
  {
    *outputLength += extraBytes + 1;
    ++steps;
  }
  
  char *output = new char [*outputLength];
  *outputPtr = output;
  
  for (size_t i = 0; i < steps; ++i)
  {
    const u8 *inputPiece;
    
    if (i == steps - 1 && extraBytes != 0)
    {
      if (extraBytes == 1)
      {
        const u8 tmp[] = {input[i*3], '\0', '\0'};
        inputPiece = tmp;
      }
      else
      {
        const u8 tmp[] = {input[i*3], input[i*3+1], '\0'};
        inputPiece = tmp;
      }
    }
    else
      inputPiece = input+(i*3);
    
    char *outputPiece = output+(i*4);

    outputPiece[0] = base64map[ (inputPiece[0] & 0xFC) >> 2 ];
    outputPiece[1] = base64map[ ((inputPiece[0] & 0x03) << 4) | ((inputPiece[1] & 0xF0) >> 4) ];
    
    if (i*4+2 < *outputLength)
      outputPiece[2] = base64map[ ((inputPiece[1] & 0x0F) << 2) | ((inputPiece[2] & 0xC0) >> 6) ];
    if (i*4+3 < *outputLength)
    outputPiece[3] = base64map[ (inputPiece[2] & 0x3F) ];
  }
}

void Files::decode(const char *input, size_t length, u8 **outputPtr, size_t *outputLength)
{
  size_t rlength = (length/4) * 3;
  size_t extraBytes = length%4;
  if (extraBytes) rlength += extraBytes - 1;
  *outputLength = rlength;
  
  *outputPtr = new u8[rlength];
  
  size_t j = 0;
  for (u32 i = 0; i < rlength; ++i)
  {
    u8 w = i%3;
    u8 v1 = value(input[j]);
    u8 v2 = value(input[j+1]);
    
    if (w == 0)
    {
      (*outputPtr)[i] = (v1 << 2) | (v2 & 0x30) >> 4;
      ++j;
    }
    else if (w == 1)
    {
      (*outputPtr)[i] = ((v1 & 0x0F) << 4) | ((v2 & 0x3C) >> 2);
      ++j;
    }
    else if (w == 2)
    {
      (*outputPtr)[i] = ((v1 & 0x03) << 6) | (v2 & 0x3F);
      j += 2;
    }
  }
}



PieceInfo::PieceInfo(PieceType type)
{
  spec = Files::specForPiece(type);
}

// TODO: mancano pezzi nuovi
PieceInfoSpec specs[PIECES_COUNT] =
{
  {PIECE_WALL, 'W', false, false},
  {PIECE_GLASS, 'y', false, false},
  {PIECE_SOURCE, 'S', true, true},
  {PIECE_MIRROR, 'M', false, true},
  {PIECE_SKEW_MIRROR, 'K', false, true},
  {PIECE_DOUBLE_MIRROR, 'D', false, true},
  {PIECE_DOUBLE_SKEW_MIRROR, 'J', false, true},
  {PIECE_DOUBLE_PASS_MIRROR, 'P', false, true},
  {PIECE_DOUBLE_SPLITTER_MIRROR, 'd', false, true},
  {PIECE_SPLITTER, 'T', false, true},
  {PIECE_DSPLITTER, 'U', false, true},
  {PIECE_REFRACTOR, 'r', false, true},
  {PIECE_PRISM, 'p', false, true},
  {PIECE_FLIPPED_PRISM, 'p', false, true},
  {PIECE_SELECTOR, 's', true, true},
  {PIECE_SPLICER, ' ', true, true},
  {PIECE_BENDER, 'B', false, false},
  {PIECE_TWISTER, 'b', false, false},
  {PIECE_FILTER, 'F', true, false},
  {PIECE_ROUND_FILTER, 'f', false, true},
  {PIECE_POLARIZER, 'Z', true, true},
  {PIECE_TUNNEL, 'u', false, true},
  {PIECE_COLOR_SHIFTER, 'H', false, true},
  {PIECE_COLOR_INVERTER, 'I', false, true},
  {PIECE_CROSS_COLOR_INVERTER, 'X', false, true},
  {PIECE_STAR_SPLITTER, 'x', false, false},
  {PIECE_GATE, 'G', false, true},
  {PIECE_PRIMARY_GATE, 'g', false, true},
  {PIECE_QUANTUM_SPLITTER, 'Q', false, true},
  {PIECE_TELEPORTER, 'E', false, false},
  {PIECE_COMPLEX_PRISM, 'i', false, true},
  {PIECE_THREE_WAY_SPLITTER, 'r', false, true},
  {PIECE_TNT, 'N', false, false},
  {PIECE_SLIME, 'm', false, false},
  {PIECE_MINE, 'e', false, false},
  {PIECE_STRICT_GOAL, 'V', true, false},
  {PIECE_LOOSE_GOAL, 'v', true, false},
  
  {PIECE_UFO, 'f', false, false},
  {PIECE_CRYSTAL, 'Y', false, false},
  {PIECE_VOID_HOLE, 'a', false, false}
};

/*
 case 'p': return PIECE_UFO;
 case 'c': return PIECE_CRYSTAL;
 case 'h': return PIECE_VOID_HOLE;
 */

u8 Files::charForPiece(PieceType type)
{
  for (int i = 0; i < sizeof(specs)/sizeof(specs[0]); ++i)
    if (specs[i].type == type)
      return specs[i].mapping;
  
  return 0;
}

PieceType Files::pieceForChar(u8 type)
{
  for (int i = 0; i < sizeof(specs)/sizeof(specs[0]); ++i)
    if (specs[i].mapping == type)
      return specs[i].type;
  
  return PIECE_WALL;
}

PieceInfoSpec* Files::specForPiece(PieceType type)
{
  for (int i = 0; i < sizeof(specs)/sizeof(specs[0]); ++i)
    if (specs[i].type == type)
      return &specs[i];
  
  return nullptr;
}

LaserColor Files::colorForChar(u8 color)
{
  switch (color) {
    case 'n': case 'N': return COLOR_NONE;
    case 'r': case 'R': return COLOR_RED;
    case 'g': case 'G': return COLOR_GREEN;
    case 'b': case 'B': return COLOR_BLUE;
    case 'c': case 'C': return COLOR_CYAN;
    case 'm': case 'M': return COLOR_MAGENTA;
    case 'y': case 'Y': return COLOR_YELLOW;
    case 'w': case 'W': return COLOR_WHITE;
    default: return COLOR_WHITE;
  }
}

u8 Files::charForColor(LaserColor color)
{
  switch (color) {
    case COLOR_NONE: return 'n';
    case COLOR_RED: return 'r';
    case COLOR_GREEN: return 'g';
    case COLOR_BLUE: return 'b';
    case COLOR_CYAN: return 'c';
    case COLOR_MAGENTA: return 'm';
    case COLOR_YELLOW: return 'y';
    case COLOR_WHITE: return 'w';
  }
}

Direction Files::directionForChar(u8 dir)
{
  switch (dir) {
    case 'N': case 'n': case '0': return NORTH;
    case 'O': case 'o': case '1': return NORTH_EAST;
    case 'E': case 'e': case '2': return EAST;
    case 'A': case 'a': case '3': return SOUTH_EAST;
    case 'S': case 's': case '4': return SOUTH;
    case 'U': case 'u': case '5': return SOUTH_WEST;
    case 'W': case 'w': case '6': return WEST;
    case 'R': case 'r': case '7': return NORTH_WEST;
    default: return NORTH;
  }
}

u8 Files::charForDirection(Direction dir)
{
  switch (dir) {
    case NORTH: return 'n';
    case NORTH_EAST: return 'o';
    case EAST: return 'e';
    case SOUTH_EAST: return 'a';
    case SOUTH: return 's';
    case SOUTH_WEST: return 'u';
    case WEST: return 'w';
    case NORTH_WEST: return 'r';
  }
}

// type x y color direction roteable moveable

vector<LevelPack> Files::packs;
u32 Files::selectedPack = 0;

PieceInfo Files::loadPiece(const u8 *ptr)
{
  //char buffer[PIECE_INFO_SIZE];
  //is.read(buffer, PIECE_INFO_SIZE);
  
  PieceType type = pieceForChar(ptr[0] & 0x7F);
  PieceInfo info(type);
  
  if (info.spec)
  {
    info.x = (ptr[1] & 0xF0) >> 4;
    info.y = ptr[1] & 0x0F;
    info.color = static_cast<LaserColor>((ptr[2] >> 3) & 0x07);
    info.direction = static_cast<Direction>(ptr[2] & 0x07);
    info.roteable = (ptr[2] & 0x80) != 0;
    info.moveable = (ptr[2] & 0x40) != 0;
    info.inventory = ptr[0] & 0x80;
  }
  else
    info.spec = nullptr;
  
  return info;
}

PieceSaveInfo Files::savePiece(Piece *piece)
{
  PieceSaveInfo info;
  
  info.data[0] = charForPiece(piece->type());
  // TODO: inventory
  Tile *tile = piece->getTile();
  info.data[1] = (tile->x << 4) | tile->y;
  info.data[2] = 0;
  info.data[2] |= piece->rotation();
  info.data[2] |= (piece->color() << 3);
  if (piece->canBeRotated())
    info.data[2] |= 0x80;
  if (piece->canBeMoved())
    info.data[2] |= 0x40;
  return info;
}

PieceSaveInfo Files::savePiece(const PieceInfo* piece)
{
  PieceSaveInfo info;
  
  info.data[0] = piece->spec->mapping;
  if (piece->inventory)
    info.data[0] |= 0x80;
  info.data[1] = (piece->x << 4) | piece->y;
  info.data[2] = 0;
  info.data[2] |= piece->direction;
  info.data[2] |= (piece->color << 3);
  if (piece->roteable)
    info.data[2] |= 0x80;
  if (piece->moveable)
    info.data[2] |= 0x40;
  
  return info;
}



LevelSpec Files::loadLevel(const u8 *ptr)
{
  u8 nameLength = ptr[0];
  u8 piecesCount = ptr[1];
  
  LevelSpec level = LevelSpec(string(reinterpret_cast<const char*>(ptr+2),nameLength));
  
  ptr += 2 + nameLength;
  for (u32 i = 0; i < piecesCount; ++i)
  {
    level.add(loadPiece(ptr));
    ptr += PIECE_INFO_SIZE;
  }
  
  return level;
}

void Files::saveLevel(const LevelSpec* level, u8 **ptr, size_t *length)
{
  *length = 1 + 1 + level->name.length() + PIECE_INFO_SIZE*level->count();
  *ptr = new u8[*length];
  
  u8 *optr = *ptr;
  
  optr[0] = static_cast<u8>(level->name.length());
  optr[1] = static_cast<u8>(level->count());
  strncpy(reinterpret_cast<char*>(optr+2), level->name.c_str(), level->name.length());
  
  optr += 1 + 1 + level->name.length();
  
  for (u32 i = 0; i < level->count(); ++i)
  {
    PieceSaveInfo info = savePiece(level->at(i));
    memcpy(optr, info.data, PIECE_INFO_SIZE);
    optr += 3;
  }
}


const string PATH_SAVE = "/Users/jack/Documents/Dev/xcode/lazers/Lazers/";

const string PATH_PAK = "/Users/jack/Documents/Dev/xcode/lazers/Lazers/";

std::vector<std::string> Files::findFiles(std::string path, const char *ext)
{
  vector<string> packs;
  
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (path.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name + strlen(ent->d_name) - 4, ext, 4) == 0)
      {
        packs.push_back(ent->d_name);
//        printf ("%s\n", ent->d_name);

      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    //perror ("");
  }
  
  return packs;
}

LevelPack Files::loadPack(std::string filename)
{
  ifstream in = ifstream(PATH_PAK+filename);
  
  u8 *output;
  size_t outputLength;
  string line;
  
  if (in)
  {
    getline(in, line);
    decode(line.c_str(), line.length(), &output, &outputLength);
    u8 nameLength = static_cast<u8>(output[0]);
    
    string name = string(reinterpret_cast<const char *>(&output[1]),nameLength);
    string author = string(reinterpret_cast<const char *>(&output[1+nameLength]),outputLength - 1 - nameLength);
    
    delete [] output;
    
    LevelPack pack = LevelPack(name, author, filename);
    
    while (getline(in, line))
    {
      decode(line.c_str(), line.length(), &output, &outputLength);

      pack.addLevel(loadLevel(static_cast<const u8 *>(output)));
      
      delete [] output;
    }
    
    return pack;
    //while (getline(in, line))
  }
  else
    throw exception();
}

void Files::loadPacks()
{
  vector<string> packs = findFiles(PATH_PAK, ".pak");
  std::sort(packs.begin(), packs.end());
  
  for (string filename : packs)
  {
    LevelPack pack = loadPack(filename);
    Files::packs.push_back(pack);
  }
}

void Files::savePack(LevelPack *pack)
{
  ofstream os = ofstream(PATH_PAK+pack->filename+".pak");
  
  char *output;
  size_t outputLength;
  
  if (os)
  {
    stringstream ss;
    ss << (char)pack->name.length() << pack->name << pack->author;
    
    std::string header = ss.str();
    
    encode(reinterpret_cast<const u8*>(header.c_str()), header.length(), &output, &outputLength);
    os.write(output, outputLength) << endl;
    delete [] output;
    
    for (int i = 0; i < pack->count(); ++i)
    {
      u8 *levelOutput;
      size_t levelOutputLength;
      
      saveLevel(pack->at(i), &levelOutput, &levelOutputLength);
      encode(levelOutput, levelOutputLength, &output, &outputLength);
      os.write(output, outputLength) << endl;
      delete [] output;
      delete [] levelOutput;
    }
  }
}



void Files::loadSolvedStatus()
{
  ifstream in = ifstream(PATH_SAVE+"save.dat");
  string line;
  
  if (in)
  {
    while (getline(in, line))
    {
      u8 *output;
      size_t outputLength;
      
      decode(line.c_str(), line.length(), &output, &outputLength);
      
      string packName = string(reinterpret_cast<const char *>(output+1), output[0]);
      
      for (LevelPack &pack : packs)
      {
        if (pack.filename == packName)
        {
          u8 *status = output + 1 + output[0];
          
          for (int i = 0; i < pack.count(); ++i)
          {
            int k = i%8;
            int b = i/8;
            
            if (status[b] & (1 << k))
            {
              pack.at(i)->solved = true;
              ++pack.solvedCount;
            }
          }

          break;
        }
      }
      
      delete [] output;
    }
  }
}

void Files::saveSolvedStatus()
{  
  ofstream os = ofstream(PATH_SAVE+"save.dat");
  
  if (os)
  {
    for (LevelPack &pack : packs)
    {
      stringstream ss;
      
      ss << static_cast<char>(pack.filename.length());
      ss << pack.filename;
      
      u32 steps = pack.count() / 8 + (pack.count() % 8 != 0 ? 1 : 0);
      
      for (u32 i = 0; i < steps; ++i)
      {
        u8 byte = 0;
        
        for (u32 k = 0; k < 8 && i*8 + k < pack.count(); ++k)
          if (pack.at(i*8 + k)->solved)
          {
            byte |= 1 << k;
          }
            
        ss << static_cast<char>(byte);
      }
      
      string result = ss.str();
      
      char *output;
      size_t outputLength;
      encode(reinterpret_cast<const u8*>(result.c_str()), result.length(), &output, &outputLength);
      os << output;
      os << endl;
      
      delete [] output;
    }
  }
}



