#ifndef ROOMGENBOX_H_INCLUDED
#define ROOMGENBOX_H_INCLUDED

#include "gamesystem.h"

#include <vector>
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


    /// Concerning the room object's behavior during generation
    std::vector<RoomGenBox*>overlaps;


    /// Misc information of this roombox
    bool designatedMainRoom; // If true, this room's dimensions have met the required thresholds to be a main room and has been designated as such.
    bool designatedHallRoom;

    RoomGenBox(int id, int x, int y, int w, int h);
    ~RoomGenBox();

    void UpdateDimensions();
    void UpdateMidpoint();
    void SnapToGrid();       // Have the box's coordinates snap to the next lowest multiple of MINI_TILESIZE.
    bool BoundaryDeletionCheck(int strictness); // Boxes that end up outside the area boundaries are deleted - Return true if this deletion is necessary.
};

#endif // ROOMGENBOX_H_INCLUDED
