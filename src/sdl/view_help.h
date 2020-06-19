#include "game.h"

class HelpView : public View
{
private:
  Field* field;

public:
  HelpView(Game* game) : View(game), field(nullptr) { }

  void activate() override;
  void deactivate() override;

  void draw() override;
  void handleEvent(SDL_Event& event) override;
  
  void handleMouseEvent(EventType type, int x, int y, int button) override;
};
