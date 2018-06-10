#pragma once

#ifdef MATHLIB_IMPLEMENTATION

#ifndef ML_ASSERT
#include <assert.h>
#define ML_ASSERT(EXPR, MSG) assert(EXPR)
#endif

#endif

#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

#include "size.h"
#include "color.h"