#include "view_help.h"

#include "gfx.h"
#include "ui.h"

#include "view_level.h"

#include "files/level_encoder.h"

class HelpEntry
{
public:
  std::string title;
  std::string text;
  std::string level;
 
  HelpEntry(std::string&& title, std::string&& text, std::string&& level) : title(title), text(text), level(level) { }

  bool hasExampleLevel() const { return !level.empty(); }
  void buildField(Field* field) const
  {
    assert(level.length() % 5 == 0);
    
    LevelEncoder encoder;
    for (size_t i = 0; i < level.length(); i += 5)
    {
      PieceInfo info = encoder.decodePieceFromString(&level[i], false);
      Piece* piece = field->generatePiece(info);
      field->place(Position(info.x, info.y), piece);
      field->updateLasers();
    }
  }
};

static const HelpEntry help[] = {
  HelpEntry("Source", "A source produces a beam of a specific color. Some can be rotated.", "S112RS122GS132BS350YS450MS550C"),
  HelpEntry( "Mirror", "The mirror deflects laser beams by 90 degrees.", "M221 S02erM260 S303yM632 M454 "),
  HelpEntry( "Double Mirror", "A double mirror works as a normal mirror but on both sides", "S113RS557BD332 "),
};

class HelpEntryList : public OffsettableList<const HelpEntry*>
{
private:
  size_t selected;

public:
  HelpEntryList() : OffsettableList<const HelpEntry*>(ui::LIST_SIZE), selected(0) { }

  size_t current() const override { return selected; }
  size_t count() const override { return sizeof(help) / sizeof(help[0]); }
  void set(size_t i) override { selected = i; }
  const HelpEntry* get(size_t i) const override { return &help[i]; }
};

static HelpEntryList list;

void HelpView::draw()
{
  Gfx::clear(BACKGROUND_COLOR);

  Gfx::drawString(ui::TITLE_X, ui::TITLE_Y, false, "HELP");
  
  for (int i = 0; list.hasNext(i) && i < list.LIST_SIZE; ++i)
  {
    const HelpEntry* entry = list.get(i);

    Gfx::drawString(ui::LIST_X, ui::LIST_Y + ui::LIST_DY*i, false, "%s", entry->title.c_str());

    if (list.isSelected(i))
      Gfx::blit(Gfx::ui, 0, 0, 4, 7, ui::LIST_X - 8, ui::LIST_Y + ui::LIST_DY*i);
  }

  if (list.hasSelection())
  {
    const HelpEntry& entry = help[list.current()];

    const int x = Gfx::width() - ui::TILE_SIZE*field->width() - 10;

    if (!entry.level.empty())
    {
      LevelView::drawGrid(x, 10, field->width(), field->height(), Gfx::screen);
      LevelView::drawField(field, Gfx::screen, Gfx::width() - ui::TILE_SIZE*field->width() - 10, 10);
    }

    Gfx::drawStringBounded(x, 20 + field->height()*ui::TILE_SIZE, 100, entry.text.data());
  }



  Gfx::postDraw();
}

void HelpView::handleEvent(SDL_Event& event)
{
  switch (event.type)
  {
    case SDL_MOUSEMOTION:
    {
      auto x = event.motion.x / SCALE, y = event.motion.y / SCALE;
      int i = ui::coordToListEntry(x, y);

      if (i >= 0 && (i + list.getOffset()) != list.current() && list.isValidIndex(i))
      {
        list.set(list.getOffset() + i);

        const HelpEntry& entry = help[list.current()];
        
        if (entry.hasExampleLevel())
        {
          field->reset();
          entry.buildField(field);
        }
      }

      break;
    }

    case SDL_MOUSEBUTTONDOWN:
    {
      auto x = event.motion.x / SCALE, y = event.motion.y / SCALE;
      int i = ui::coordToListEntry(x, y);

      if (i >= 0 && list.get(i))
        ;

      break;
    }

    case SDL_MOUSEWHEEL:
    {
      ui::handleMouseWheelOnList(list, event.wheel.y);
      break;
    }
  }
}

void HelpView::activate()
{
  field = new Field(7, 7, 0, 0);
}

void HelpView::deactivate()
{
  delete field;
}
