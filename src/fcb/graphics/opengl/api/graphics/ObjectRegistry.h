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

#include <memory>
#include <variant>

namespace fcb { namespace core {
    class Bunny;
    class Clover;
    class Fox;
} }

namespace fcb { namespace graphics {

using GameObjectPointer = std::variant<
    std::weak_ptr<fcb::core::Clover>,
    std::weak_ptr<fcb::core::Bunny>,
    std::weak_ptr<fcb::core::Fox>>;

//! Give a weak object reference to the graphics system.
//! The graphics system will draw all the objects given as long as the reference remains valid.
//! Having registration allows the graphics system to associate an object with display state (e.g. animations).
//! @param[in] gameObject A weak pointer to a game object to be drawn.
//! @return True if successful.
bool RegisterObject(GameObjectPointer gameObject);

} }
