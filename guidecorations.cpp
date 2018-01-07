#include "guidecorations.h"

FrameRune::FrameRune()
{
    xPosition = 0;
    yPosition = 0;
}

FrameRune::~FrameRune()
{

}

void FrameRune::Set(char whichRune,
                    float iXPosition, float iYPosition,
                    float iScroll, float iTermination,
                    float iXVeloc, float iYVeloc)
{
    rune = whichRune;

    xPosition = startXPosition = iXPosition;
    yPosition = startYPosition = iYPosition;

    scrollDistance = iScroll;
    terminationDistance = iTermination;

    xVelocity = iXVeloc;
    yVelocity = iYVeloc;

}

void FrameRune::Scroll()
{
    xPosition += xVelocity;
    yPosition += yVelocity;
    scrollDistance += (xVelocity+yVelocity)/2;

    if(scrollDistance >= terminationDistance)
    {
        xPosition = startXPosition;
        yPosition = startYPosition;
    }
}
