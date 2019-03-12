#include "voxelcoords.h"

namespace ou {

VoxelCoords VoxelCoords::operator-(const VoxelCoords& other) const
{
    auto diffLow = glm::u64vec3(pos) - glm::u64vec3(other.pos);
    auto diffHigh = glm::u64vec3(voxel) - glm::u64vec3(other.voxel);

    // TODO: take care of underflows

    return { diffHigh, diffLow };
}

VoxelCoords VoxelCoords::operator+(const VoxelCoords &other) const
{
    return { voxel + other.voxel, pos + other.pos };
}
}
