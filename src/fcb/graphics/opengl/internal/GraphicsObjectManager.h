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

#include "graphics/ObjectRegistry.h"
#include "Model.h"
#include "CompareModel.h"

#include <set>
#include <memory>

namespace fcb { namespace graphics {

//! Manages the extra information needed to represent objects graphically.
class GraphicsObjectManager
{
public:
    bool RegisterObject(GameObjectPointer gameObject);
    bool UnregisterObject(GameObjectPointer const& gameObject);

    class Attorney
    {
        friend class World;
        static void Draw(GraphicsObjectManager& instance) { instance.draw(); }
    };

private:
    //! Should only be called when OpenGL is ready to draw.
    void draw();

    std::set<std::unique_ptr<Model>, CompareModel> m_models;
};

} }
