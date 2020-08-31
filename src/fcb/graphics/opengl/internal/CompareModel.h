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

#include "Model.h"

namespace fcb { namespace graphics {

//! Does a < comparison on the object's weak pointer's managed pointer.
//! (Uses the result of std::owner_less on the respective weak-pointer-to-game-object.)
//! Allows Model instances in a collection to be searched given the linked object.
struct CompareModel
{
    using is_transparent = void;  // Enable equivalency comparisons.

    //! @return True if left < right based on the managed pointer to the linked game object.
    bool operator()(std::unique_ptr<Model> const& left, std::unique_ptr<Model> const& right) const
    {
        return left->GetGameObject().owner_before(right->GetGameObject());
    }

    //! @return True if left < right based on the managed pointer to the linked game object.
    bool operator()(std::unique_ptr<Model> const& left, std::weak_ptr<fcb::core::GameObject const> const& right) const
    {
        return left->GetGameObject().owner_before(right);
    }

    //! @return True if left < right based on the managed pointer to the linked game object.
    bool operator()(std::weak_ptr<fcb::core::GameObject const> const& left, std::unique_ptr<Model> const& right) const
    {
        return left.owner_before(right->GetGameObject());
    }
};

} }
