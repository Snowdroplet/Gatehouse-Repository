#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED

#include <iostream>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "gamesystem.h"
#include "allegrocustom.h"
#include "resource.h"

extern const int TERMINAL_NUM_LINES;
extern const int EXCERPT_NUM_LINES;

extern const int TERMINAL_MAX_WIDTH;
extern const int TERMINAL_EXCERPT_LINES;

extern std::string terminalLine[]; //The entire terminal's text, chopped up into neat lines
extern std::string terminalExcerpt[]; //Shown when extended terminal is not open

extern int lineToAppend; // The line currently being written to. New turn or text wrap will increase
extern int lineRemainingWidth;

extern bool extendedLogOpen;

extern int TERMINAL_TEXT_OPEN_X;
extern int TERMINAL_TEXT_OPEN_Y;

void WriteToTerminal(std::string toAppend, ALLEGRO_COLOR colourToAppend);
void ShiftTerminal();
void UpdateTerminalExcerpt();




#endif // TERMINAL_H_INCLUDED
