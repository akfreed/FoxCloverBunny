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

namespace fcb { namespace core {
    class Fox;
} }

namespace fcb { namespace graphics {

//! The model for a cute fox.
class ModelFox : public Model
{
public:
    explicit ModelFox(std::weak_ptr<fcb::core::Fox const> gameObject);
    ~ModelFox() override = default;

    std::weak_ptr<fcb::core::GameObject const> GetGameObject() const override;

    bool DrawIfValid() const override;

private:
    std::weak_ptr<fcb::core::Fox const> m_gameObject;
};

} }
