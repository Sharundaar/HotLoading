#pragma once

#include "mathlib.h"

struct Transform
{
    Vector3 position = {};
    Quaternion rotation = {};
};

struct Entity
{
    Transform transform;
    
};