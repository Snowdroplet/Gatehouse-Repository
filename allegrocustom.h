#ifndef ALLEGROCUSTOM_H_INCLUDED
#define ALLEGROCUSTOM_H_INCLUDED

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/color.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>


void c_al_draw_centered_bitmap(ALLEGRO_BITMAP *bitmap, float dx, float dy, int flags);
void c_al_draw_rotated_centered_bitmap(ALLEGRO_BITMAP *bitmap, float cx, float cy, float dx, float dy, float angle, int flags);

int s_al_get_text_width(const ALLEGRO_FONT *f, std::string str);
void s_al_draw_text(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, std::string text);

int s_al_show_native_message_box(ALLEGRO_DISPLAY *display,
                                 std::string title, std::string heading, std::string text,
                                 char const *buttons, int flags);

extern ALLEGRO_COLOR NEUTRAL_WHITE;
extern ALLEGRO_COLOR DIM_NEUTRAL_WHITE;
extern ALLEGRO_COLOR NEUTRAL_GRAY;
extern ALLEGRO_COLOR DIM_NEUTRAL_GRAY;
extern ALLEGRO_COLOR NEUTRAL_BLACK;

extern ALLEGRO_COLOR FIRE_ORANGE;

extern ALLEGRO_COLOR LIGHTNING_YELLOW;
extern ALLEGRO_COLOR PALE_YELLOW;
extern ALLEGRO_COLOR DARK_YELLOW;

extern ALLEGRO_COLOR COLD_BLUE;

extern ALLEGRO_COLOR HOLY_INDIGO;
extern ALLEGRO_COLOR DARK_VIOLET;

extern ALLEGRO_COLOR POISON_GREEN;
extern ALLEGRO_COLOR BRIGHT_GREEN;
extern ALLEGRO_COLOR BLOOD_RED;

void AllegroCustomInit();

#endif // ALLEGROCUSTOM_H_INCLUDED
