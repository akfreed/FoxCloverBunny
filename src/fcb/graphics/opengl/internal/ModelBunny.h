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

#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>

namespace fcb { namespace core {
    class Bunny;
} }

namespace fcb { namespace graphics {

//! The model for a cute bunny.
class ModelBunny
{
public:
    explicit ModelBunny(std::weak_ptr<fcb::core::Bunny> gameObject);

    bool DrawIfValid() const;

private:
    std::weak_ptr<fcb::core::Bunny> m_gameObject;
};

} }
