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

//! Translate a GUI input to the corresponding input state.
//! The FLTK window should call this function when an event occurs.
//! @param[in] event The FLTK event code.
//! @return 1 if the event was handled. 0 if not.
int ProcessFltkInputEvent(int event);

} }
