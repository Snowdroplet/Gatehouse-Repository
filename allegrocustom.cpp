#include "allegrocustom.h"

ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_TIMER *FPStimer;
ALLEGRO_EVENT ev;
ALLEGRO_MOUSE_STATE mouseState;

ALLEGRO_COLOR NEUTRAL_WHITE;
ALLEGRO_COLOR DIM_NEUTRAL_WHITE;
ALLEGRO_COLOR NEUTRAL_GRAY;
ALLEGRO_COLOR DIM_NEUTRAL_GRAY;
ALLEGRO_COLOR NEUTRAL_BLACK;

ALLEGRO_COLOR FIRE_ORANGE;

ALLEGRO_COLOR LIGHTNING_YELLOW;
ALLEGRO_COLOR PALE_YELLOW;
ALLEGRO_COLOR DARK_YELLOW;

ALLEGRO_COLOR COLD_BLUE;

ALLEGRO_COLOR HOLY_INDIGO;
ALLEGRO_COLOR DARK_VIOLET;

ALLEGRO_COLOR POISON_GREEN;
ALLEGRO_COLOR BRIGHT_GREEN;
ALLEGRO_COLOR BLOOD_RED;

ALLEGRO_COLOR PEN_INK;

void AllegroCustomColours()
{
    NEUTRAL_WHITE = al_map_rgb(255,255,255);
    DIM_NEUTRAL_WHITE = al_map_rgb(200,200,200);
    NEUTRAL_GRAY = al_map_rgb(180,180,180);
    DIM_NEUTRAL_GRAY = al_map_rgb(128,128,128);
    NEUTRAL_BLACK = al_map_rgb(0,0,0);

    FIRE_ORANGE = al_map_rgb(255,106,0);

    LIGHTNING_YELLOW = al_map_rgb(255,216,0);
    PALE_YELLOW = al_map_rgb(255,255,102);
    DARK_YELLOW = al_map_rgb(102, 102, 0);

    COLD_BLUE = al_map_rgb(0,148,255);

    HOLY_INDIGO = al_map_rgb(220,220,255);
    DARK_VIOLET = al_map_rgb(178,0,255);

    POISON_GREEN = al_map_rgb(0,127,14);
    BRIGHT_GREEN = al_map_rgb(55,255,55);
    BLOOD_RED = al_map_rgb(127,0,0);

    PEN_INK = al_map_rgb(0,15,84);

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

void al_draw_centered_text(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, char const* text)
{
    // Note that this function only centers on the y dimension, neglecting the x, because too convoluted.
    al_draw_text(font, color, x, y-al_get_font_line_height(font)/2, flags, text);
}

void s_al_draw_centered_text(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, std::string text)
{
    // Note that this function only centers on the y dimension, neglecting the x, because too convoluted.
    const char *c = text.c_str();
    al_draw_text(font, color, x, y-al_get_font_line_height(font)/2, flags, c);
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
