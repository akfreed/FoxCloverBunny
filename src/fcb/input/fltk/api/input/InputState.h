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

namespace fcb { namespace input {

//! An interface for sharing user input state between the main program and the GUI.
struct InputState
{
    bool exit = false;
    bool fastForward = false;
};

//! Get the graphics system's input state.
//! The input state can be modified by the main program at your own risk.
//! @return A modifiable reference to the graphics system's input state.
InputState& GetInputState();

} }
