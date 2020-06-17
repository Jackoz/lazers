//
//  lazers.h
//  TestSDL
//
//  Created by Jack on 1/9/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#pragma once

#include "common/common.h"

#include <string>
#include <vector>

struct LevelSpec;
class LevelPack;

class Aargon
{
public:
  static LevelSpec parseLevel(const std::string& name);
  static std::vector<LevelPack> parseLevels();
};