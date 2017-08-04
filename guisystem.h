#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "gamesystem.h"
#include "control.h"
#include "resource.h"

#include "guidecorations.h"

enum enumTargetLockLevels
{
    TARGET_NONE = 0,
    TARGET_AUTO = 1,  // Soft-locked onto the closest being
    TARGET_CELL = 2,  // Hard-locked onto a visible cell
    TARGET_BEING = 3  // Hard-locked onto a visible being

};

class GuiSystem
{


    /// Frame

    std::vector<FrameRune*>frameRunes;

    bool frameActive;

    int frameHorizontalWidth;
    int frameHorizontalHeight;
    int numFrameHorizontalsAcross;

    int frameVerticalWidth;
    int frameVerticalHeight;
    int numFrameVerticalsDown;

    int frameCornerWidth;

    float frameRunicLead;
    float frameRunicScrollSpeed;

    int frameTotalRunicWidth;
    int frameIndividualRunicWidth;

    std::string frameRunicString;



    /// Frame buttons/menus
    int playerStatsBarButtonX;
    int playerStatsBarButtonY;

    int playerJournalBarButtonX;
    int playerJournalBarButtonY;

    /// Target context

    int targetXPosition;
    int targetYPosition;

    int targetLockLevel; // A heirarchy, from no target, to auto target, to hard targetted cells and beings


public:
    GuiSystem();
    ~GuiSystem();

    /// Logic
    void ProcessInput(int whatContext);

    void SetFrameRunicString(std::string update);
    void UpdateElements();

    /// Drawing

    void DrawFrame();
    void DrawTargetContext();



};

#endif // GUI_H_INCLUDED
