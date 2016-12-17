#include "GameSaves.h"

Point GetSpawnPosition(uint32 levelNumber, uint32 checkpointNumber)
{
    switch (levelNumber)
    {
        case 1:
            if (checkpointNumber == 0) { return Point(689, 4723); }
            else if (checkpointNumber == 1) { return Point(6873, 4970); }
            else if (checkpointNumber == 2) { return Point(14227, 2281); }

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        default:
            assert(false && "Not implemented yet");
    }

    assert(false && "Unknown level / checkpoint number.");
    return Point();
}