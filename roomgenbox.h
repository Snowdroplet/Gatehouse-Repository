#ifndef ROOMGENBOX_H_INCLUDED
#define ROOMGENBOX_H_INCLUDED

#include "gamesystem.h"

#include <vector>
#include <ctime>
#include <boost/random.hpp>


struct RoomGenBox
{
    /// Concerning the unique identifiers of the room object.
    int boxNumber; // Unique ID assigned to the box.

    /// Describes position and dimensions of the box.
    float x1, y1; // Top left corner
    float x2, y2; // Bottom right corner
    float x3, y3; // Midpoint

    int width, height; // Dimensions in pixels.
    int cellWidth, cellHeight; // Dimensions in cells (or pixels/MINI_TILESIZE).


    /// Data for use in procedural generation
    boost::random::mt19937 mtRng;
    std::vector<RoomGenBox*>overlaps;
    int xVelocity, yVelocity;



    /// Misc information of this roombox
    bool designatedMainRoom; // If true, this room's dimensions have met the required thresholds to be a main room and has been designated as such.
    bool designatedHallRoom;

    RoomGenBox(int id, int x, int y, int w, int h);
    ~RoomGenBox();

    bool CheckOverlap(RoomGenBox* other);
    void AddOverlap(RoomGenBox* other);
    void RepulseOverlaps();
    void ClearOverlaps();


    void Move();
    void ClearVelocity();

    void UpdateDimensions();
    void UpdateMidpoint();
    void SnapToGrid();       // Have the box's coordinates snap to the next lowest multiple of MINI_TILESIZE.
    bool BoundaryDeletionCheck(int strictness); // Boxes that end up outside the area boundaries are deleted - Return true if this deletion is necessary.
};

#endif // ROOMGENBOX_H_INCLUDED
