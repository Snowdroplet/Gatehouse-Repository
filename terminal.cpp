#include "terminal.h"

const int TERMINAL_NUM_LINES = 30;
const int EXCERPT_NUM_LINES = 4;

const int TERMINAL_MAX_WIDTH = 550; // 600 for example

/// Later create an "offset by X lines" value so that drawing happens on the topmost row until the terminal is filled
/// In main, maybe the (draw Y position of each line - text height * offset)

int lineRemainingWidth;

bool extendedLogOpen = false;

int TERMINAL_TEXT_OPEN_X = 215;
int TERMINAL_TEXT_OPEN_Y = SCREEN_H - 50;

std::string terminalLine[TERMINAL_NUM_LINES];
std::string terminalExcerpt[EXCERPT_NUM_LINES];

void WriteToTerminal(std::string toAppende, ALLEGRO_COLOR colourToAppend)
{
    std::string toAppend;
    toAppend.assign(toAppend);

    //This loop ends when toAppend is completely emptied.
    for(std::string::iterator it = toAppend.begin(); toAppend.begin() != toAppend.end(); ++it)
    {
        //Calculate remaining space
        lineRemainingWidth = TERMINAL_MAX_WIDTH - s_al_get_text_width(pirulenFont,terminalLine[0]);

        if(*it == ' ' || *it == '\0')
        {
            //Check width of text to append against remaining space
            //If sufficient, append to 0th row. If insufficient, shift all rows up by 1 first.
            if(s_al_get_text_width(pirulenFont,toAppend) < lineRemainingWidth)
            {
                terminalLine[0].append(toAppend.begin(),it);
            }
            else
            {
                ShiftTerminal();
                terminalLine[0].append(toAppend.begin(),it);
            }

            //The part of toAppend that has been copied over is no longer necessary, and is erased.
            //The now invalidated iterator is reset to the beginning.
            toAppend.erase(toAppend.begin(),it);
            it = toAppend.begin();
        }
    }
    //std::cout << lineRemainingWidth << " px remaining in line 0" << std::endl;

    UpdateTerminalExcerpt();

    /* Old testing code
    std::cout << "Terminal (turn " << turn << "):" << std::endl;
    for(int i = 10; i >= 0; i--)
    {
        std::cout << "ln" << i << ":" << terminalLine[i] << std::endl;
    }
    std::cout << std::endl;
    */
}

void ShiftTerminal()
{
    //Shift all lines up by one, copying the contents of the previous line
    for(int i = TERMINAL_NUM_LINES-1; i > 0; --i)
        terminalLine[i].assign(terminalLine[i-1]);

    terminalLine[0].erase();
    lineRemainingWidth = TERMINAL_MAX_WIDTH;
}

void UpdateTerminalExcerpt()
{
    for(int i = 0; i < EXCERPT_NUM_LINES; i++)
    {
        terminalExcerpt[i].assign(terminalLine[i]);
    }
}
