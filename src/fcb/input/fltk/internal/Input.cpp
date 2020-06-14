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

#include "input/InputEvent.h"
#include "input/InputState.h"

#include <FL/Fl.H>

namespace fcb { namespace input {


InputState& GetInputState()
{
    static InputState s_inputState;
    return s_inputState;
}

int ProcessFltkInputEvent(int event)
{
    InputState& inputState = GetInputState();

    switch (event)
    {
    // Window closed.
    case FL_HIDE:
        inputState.exit = true;
        return 1;

    // Key events.
    case FL_KEYBOARD:
        // Esc.
        // If unhandled, ESC automatically closes the window.
        if (Fl::event_key() == FL_Escape)
        {
            return 1;
        }

        // Space.
        // Fast-forward.
        if (Fl::event_key() == ' ')
        {
            inputState.fastForward = !inputState.fastForward;
            return 1;
        }
    }

    return 0;
}


} }
