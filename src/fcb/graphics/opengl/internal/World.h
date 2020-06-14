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

#include "GraphicsObjectManager.h"

namespace fcb { namespace graphics {

//! Draws the world.
//! Singleton class.
class World
{
public:
    //! @return The singleton instance.
    static World& Get();

    World(World const&)            = delete;
    World(World&&)                 = delete;
    World& operator=(World const&) = delete;
    World& operator=(World&&)      = delete;

    GraphicsObjectManager& GetGraphicsObjectManager();

    void Refresh();

private:
    World() = default;

    // Friend access for the API function.
    friend void Draw();
    //! Should only be called when OpenGL is ready to draw.
    void draw();

    bool m_refresh = true;
    GraphicsObjectManager m_graphicsObjectManager;
};

} }
