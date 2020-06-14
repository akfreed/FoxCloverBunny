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

namespace fcb { namespace core {

//! A struct for holding globals.
struct Globals
{
    static bool     constexpr c_worldWrap = true;  // If true, going past the edge warps the object to the other side.
    static unsigned constexpr c_numHiddenNodes = 6;  // The number of nodes in the neural network's hidden layer.
    static unsigned constexpr c_cloverHp = 1;  // How many bites the clovers have in them before they are consumed.
    static unsigned constexpr c_secondsPerGeneration = 15;  // The runtime for one generation.

    static float constexpr c_worldLeftBound   = -1;
    static float constexpr c_worldRightBound  =  1;
    static float constexpr c_worldBottomBound = -1;
    static float constexpr c_worldTopBound    =  1;
};

} }
