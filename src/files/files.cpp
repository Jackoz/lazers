//
//  lazers.cpp
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "files.h"

#include "core/level.h"

#ifdef _WIN32
#include "platforms/windows/dirent.h"
#else
#include <dirent.h>
#endif

#include <algorithm>
#include <cstring>

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
// type x y color direction roteable moveable

u32 Files::selectedPack = 0;

LevelSpec Files::loadLevel(const byte_t *ptr)
{
  static_assert(std::alignment_of<PieceInfo>::value == 1, "must be 1");
#ifndef _WIN32
  static_assert(sizeof(PieceInfo) == 5, "must be 5");
#endif
  
  size_t nameLength = ptr[0];
  size_t piecesCount = ptr[1];
  
  LevelSpec level = LevelSpec(string(reinterpret_cast<const char*>(ptr+2),nameLength));
  
  const PieceInfo* pptr = reinterpret_cast<const PieceInfo*>(ptr + 2 + nameLength);
  for (size_t i = 0; i < piecesCount; ++i)
  {
    level.add(*pptr);
    ++pptr;
  }
  
  return level;
}

void Files::saveLevel(const LevelSpec* level, byte_t **ptr, size_t *length)
{
  *length = 1 + 1 + level->name.length() + sizeof(PieceInfo)*level->count();
  *ptr = new u8[*length];
  
  u8 *optr = *ptr;
  
  optr[0] = static_cast<u8>(level->name.length());
  optr[1] = static_cast<u8>(level->count());

#ifdef _WIN32
  strncpy_s(reinterpret_cast<char*>(optr + 2), *length, level->name.c_str(), level->name.length());
#else
  strncpy(reinterpret_cast<char*>(optr + 2), level->name.c_str(), level->name.length());
#endif
  
  PieceInfo* poptr = reinterpret_cast<PieceInfo*>(optr + 2 + level->name.length());
  
  for (u32 i = 0; i < level->count(); ++i)
  {
    *poptr = level->at(i);
    ++poptr;
  }
}


#if defined(OPEN_DINGUX)
const string PATH_SAVE = "/home/SD/lazers/packs/";
const string PATH_PAK = "/home/SD/lazers/packs/";
#else
const string PATH_SAVE = "/Users/jack/Documents/Dev/c++/lazers/data/packs/";
const string PATH_PAK = "/Users/jack/Documents/Dev/c++/lazers/data/packs/";
#endif


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

std::vector<LevelPack> Files::loadPacks()
{
  std::vector<LevelPack> packs;
  std::vector<std::string> files = findFiles(PATH_PAK, ".pak");
  std::sort(files.begin(), files.end());
  
  std::transform(files.begin(), files.end(), std::back_inserter(packs),
                 [] (const std::string& filename) { return loadPack(filename); }
                 );
  
  return packs;
}

LevelPack Files::loadPack(const std::string& filename)
{
  ifstream in(PATH_PAK+filename);
  
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
  }
  else
    throw exception();
}

void Files::savePack(const LevelPack& pack)
{
  ofstream os(PATH_PAK+pack.path()+".pak");
  
  char *output;
  size_t outputLength;
  
  if (os)
  {
    stringstream ss;
    ss << (u8)pack.name().length() << pack.name() << pack.author();
    
    std::string header = ss.str();
    
    encode(reinterpret_cast<const u8*>(header.c_str()), header.length(), &output, &outputLength);
    os.write(output, outputLength) << endl;
    delete [] output;
    
    for (int i = 0; i < pack.count(); ++i)
    {
      u8 *levelOutput;
      size_t levelOutputLength;
      
      saveLevel(pack.at(i), &levelOutput, &levelOutputLength);
      encode(levelOutput, levelOutputLength, &output, &outputLength);
      os.write(output, outputLength) << endl;
      delete [] output;
      delete [] levelOutput;
    }
  }
}



void Files::loadSolvedStatus()
{
  ifstream in(PATH_SAVE+"save.dat");
  string line;
  
  if (in)
  {
    while (getline(in, line))
    {
      u8 *output;
      size_t outputLength;
      
      decode(line.c_str(), line.length(), &output, &outputLength);
      
      string packName = string(reinterpret_cast<const char *>(output+1), output[0]);
      
      //TODO: reimplement separated from LevelPack
      /*
      for (LevelPack &pack : packs)
      {
        if (pack.path() == packName)
        {
          u8 *status = output + 1 + output[0];
          
          for (int i = 0; i < pack.count(); ++i)
          {
            int k = i%8;
            int b = i/8;
            
            if (status[b] & (1 << k))
            {
              //TODO: move solved from pack.at(i)
              //pack.at(i)->solved = true;
              ++pack.solvedCount;
            }
          }

          break;
        }
      }
      */
      
      delete [] output;
    }
  }
}

void Files::saveSolvedStatus()
{  
  ofstream os(PATH_SAVE+"save.dat");
  
  //TODO: reimplement
  /*
  if (os)
  {
    for (LevelPack &pack : packs)
    {
      stringstream ss;
      
      ss << static_cast<char>(pack.path().length());
      ss << pack.path();
      
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
  }*/
}



