#ifndef ROOMGENBOX_H_INCLUDED
#define ROOMGENBOX_H_INCLUDED

#include "gamesystem.h"

#include <boost/random.hpp>

#include <Box2D/Box2D.h>

struct RoomGenBox
{
    /// Concerning the unique identifiers of the room object.
    int boxNumber; // Unique ID assigned to the box.

    /// Concerns the corresponding Box2D PHYSICS BODY assigned to the ROOM OBJECT.
    b2Body* correspondingB2Body;  // Pointer to the corresponding PHYSICS BODY.
    bool correspondingBodyAwake;  // True if the corresponding PHYSICS BODY is awake.


    /// Describes position and dimensions of the box.
    float x1, y1; // Top left corner
    float x2, y2; // Bottom right corner
    float x3, y3; // Midpoint

    int width, height; // Dimensions in pixels.
    int cellWidth, cellHeight; // Dimensions in cells (or pixels/MINI_TILESIZE).


    /// The ultimate fate of this roombox
    bool designatedMainRoom; // If true, this room's dimensions have met the required thresholds to be a main room and has been designated as such.
    bool designatedHallRoom;

    RoomGenBox(int bn, b2Body *cb2b, int w, int h);
    ~RoomGenBox();

    void UpdatePosition();   // Have the box take on the position of it's corresponding PHYSICS BODY.
    void SnapToGrid();       // Have the box's coordinates snap to the next lowest multiple of MINI_TILESIZE.
    bool BoundaryDeletionCheck(int strictness); // Boxes that end up outside the area boundaries are deleted - Return true if this deletion is necessary.
};

#endif // ROOMGENBOX_H_INCLUDED
