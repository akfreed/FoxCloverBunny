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
    std::weak_ptr<fcb::core::Clover const>,
    std::weak_ptr<fcb::core::Bunny const>,
    std::weak_ptr<fcb::core::Fox> const>;

//! Give a weak object reference to the graphics system.
//! The graphics system will draw all the objects given as long as the reference remains valid.
//! Having registration allows the graphics system to associate an object with display state (e.g. animations).
//! @param[in] gameObject A weak pointer to a game object to be drawn.
//! @return True if successful.
bool RegisterObject(GameObjectPointer gameObject);

//! Find and remove a weak object reference from the graphics system.
//! The object will no longer be drawn.
//! Call this if the object still exists, but you don't want it to be drawn anymore.
//! Calling this after an object is destroyed is not strictly necessary because an item is automatically removed if 
//! its reference is invalid when drawing is attempted. However, it can help amortize a noticable workload if drawing
//! has not occured in a while, and many objects have been registered and subsequently destroyed in the meantime.
//! @param[in] gameObject A weak pointer to a game object to be removed from the draw list.
//! @return True if successful.
bool UnregisterObject(GameObjectPointer const& gameObject);

} }
