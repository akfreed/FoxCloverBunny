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

#include "World.h"

#include "core/Globals.h"

#include <FL/gl.h>
#include <FL/glu.h>

using fcb::core::Globals;

namespace fcb { namespace graphics {


World& World::Get()
{
    static World s_world;
    return s_world;
}

//! @return The world's GOM instance.
GraphicsObjectManager& World::GetGraphicsObjectManager()
{
    return m_graphicsObjectManager;
}

//! Set all the OpenGL setup settings again.
void World::Refresh()
{
    m_refresh = true;
}

//! Should only be called when OpenGL is ready to draw.
//! Draws everything.
void World::draw()
{
    // Initialize OpenGL on the first call.
    if (m_refresh)
    {
        m_refresh = false;

        // Set the clear color to grassy green.
        glClearColor(0.21f, 0.39f, 0.0f, 1.0f);

        // Turn on depth testing.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Turn on automatic vector normalization.
        glEnable(GL_NORMALIZE);

        // Orthographic transformation.
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(Globals::c_worldLeftBound, Globals::c_worldRightBound, Globals::c_worldBottomBound, Globals::c_worldTopBound);
    }

    // Clear the screen. Color and depth.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all the objects.
    GraphicsObjectManager::Attorney::Draw(m_graphicsObjectManager);
}


} }
