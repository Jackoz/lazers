//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _AARGON_H_
#define _AARGON_H_

#include "common.h"

#include <string>

struct LevelSpec;

class Aargon
{
  public:
    static LevelSpec parseLevel(std::string name);
    static void parseLevels();
};

#endif
