#include <iostream>
#include "SDL.h"

#include "sdl/game.h"

using namespace std;

Game game;

int main(int argc, char * argv[])
{
  game.init();
  game.loop();

  return 0;
}