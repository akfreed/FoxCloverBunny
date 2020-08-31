// ==================================================================
// Copyright 2020 Alexander K. Freed
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ==================================================================

// Language: ISO C++17

#pragma once

#include "util/Rng.h"

#include <Eigen/Dense>

// Global macros.
#define UNUSED(x) ((void)x);

namespace fcb { namespace util {


//! Eigen Maps cannot be swapped using operator=, as it overwrites the pointed-to array.
//! @param[in/out] left  The map to swap with right.
//! @param[in/out] right The map to swap with left.
template <typename T>
void SwapMap(Eigen::Map<T>& left, Eigen::Map<T>& right)
{
    Eigen::Map<T> temp = left;
    new (&left)  Eigen::Map<T>(&right(0, 0), right.rows(), right.cols());
    new (&right) Eigen::Map<T>(&temp(0, 0),  temp.rows(),  temp.cols());
}


} }
