#ifndef GUIDECORATIONS_H_INCLUDED
#define GUIDECORATIONS_H_INCLUDED

class FrameRune
{
    char rune;

    float xPosition, yPosition;
    float startXPosition, startYPosition;
    float scrollDistance;
    float terminationDistance;
    float xVelocity, yVelocity;


public:
    FrameRune();
    ~FrameRune();
    void Set(char whichRune,
             float iXPosition, float iYPosition,
             float iScroll, float iTermination, // Repeats at initial position when distance scrolled meets or exceeds termination distance.
             float iXVeloc, float iYVeloc);
    void Scroll();
};

#endif // GUIDECORATIONS_H_INCLUDED
