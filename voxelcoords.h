#ifndef VOXELCOORDS_H
#define VOXELCOORDS_H

#include <glm/glm.hpp>

namespace ou {

// This coordinate system is 3.6*10^19 light years across
// The actual observable universe is 9.3*10^10 light years across
// So it is enough.
struct VoxelCoords
{
    // approx. 1.94 light years across
    glm::i64vec3 voxel;

    // in millimeters
    glm::i64vec3 pos;

    VoxelCoords operator-(VoxelCoords const& other) const;
    VoxelCoords operator+(VoxelCoords const& other) const;
};
}

#endif // VOXELCOORDS_H
