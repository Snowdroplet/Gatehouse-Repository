#include "allegrocustom.h"


ALLEGRO_COLOR NEUTRAL_WHITE;
ALLEGRO_COLOR NEUTRAL_GRAY;
ALLEGRO_COLOR NEUTRAL_BLACK;

ALLEGRO_COLOR FIRE_ORANGE;
ALLEGRO_COLOR LIGHTNING_YELLOW;
ALLEGRO_COLOR COLD_BLUE;

ALLEGRO_COLOR HOLY_INDIGO;
ALLEGRO_COLOR DARK_VIOLET;

ALLEGRO_COLOR POISON_GREEN;
ALLEGRO_COLOR BLOOD_RED;

void AllegroCustomInit()
{
    NEUTRAL_WHITE = al_map_rgb(255,255,255);
    NEUTRAL_GRAY = al_map_rgb(180,180,180);
    NEUTRAL_BLACK = al_map_rgb(0,0,0);

    FIRE_ORANGE = al_map_rgb(255,106,0);
    LIGHTNING_YELLOW = al_map_rgb(255,216,0);
    COLD_BLUE = al_map_rgb(0,148,255);

    HOLY_INDIGO = al_map_rgb(220,220,255);
    DARK_VIOLET = al_map_rgb(178,0,255);

    POISON_GREEN = al_map_rgb(0,127,14);
    BLOOD_RED = al_map_rgb(127,0,0);
}

void c_al_draw_centered_bitmap(ALLEGRO_BITMAP *bitmap, float dx, float dy, int flags)
{
    al_draw_bitmap(bitmap,
                   dx-al_get_bitmap_width(bitmap)/2, dy-al_get_bitmap_height(bitmap)/2,
                   flags);
}

void c_al_draw_rotated_centered_bitmap(ALLEGRO_BITMAP *bitmap, float cx, float cy, float dx, float dy, float angle, int flags)
{
    al_draw_rotated_bitmap(bitmap,
                           cx, cy,
                           dx-al_get_bitmap_width(bitmap)/2, dy-al_get_bitmap_height(bitmap)/2,
                           angle, flags);
}

int s_al_get_text_width(const ALLEGRO_FONT *f, std::string str)
{
    const char *c = str.c_str();
    return al_get_text_width(f, c);
}

void s_al_draw_text(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, std::string text)
{
    const char *c = text.c_str();
    al_draw_text(font, color, x, y, flags, c);
}

int s_al_show_native_message_box(ALLEGRO_DISPLAY *display,
                                 std::string title, std::string heading, std::string text,
                                 char const *buttons, int flags)
{
    const char *ctitle = title.c_str();
    const char *cheading = heading.c_str();
    const char *ctext = text.c_str();

    return al_show_native_message_box(display, ctitle, cheading, ctext, buttons, flags);
}
