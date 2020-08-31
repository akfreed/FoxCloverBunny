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

#include "ModelClover.h"

#include "core/Clover.h"

#include <FL/gl.h>

using namespace fcb::core;

namespace fcb { namespace graphics {


//! Constructor.
//! @param[in] gameObject A weak pointer to a clover game object. The weak pointer is stored for the lifetime of this object.
ModelClover::ModelClover(std::weak_ptr<Clover const> gameObject)
    : m_gameObject(std::move(gameObject))
{ }

//! @return The linked game object that this model draws.
std::weak_ptr<fcb::core::GameObject const> ModelClover::GetGameObject() const
{
    return m_gameObject;
}

//! Draw the object if the referenced object still exists.
//! @return True if the referenced game object still exists.
//!         False if the referenced game object has been destroyed, in which case this object should be destroyed as well.
bool ModelClover::DrawIfValid() const
{
    // Check the referenced game object.
    std::shared_ptr<Clover const> const object = m_gameObject.lock();
    if (!object)
        return false;

    // Setup the transformation.
    glPushMatrix();
    glTranslatef(object->X(), object->Y(), .1f);
    glScalef(object->Radius(), object->Radius(), 0);
    float const degrees = object->Angle() * static_cast<float>(180.0 / M_PI);
    glRotatef(degrees, 0, 0, 1);

    // Color green.
    glColor3f(0, 0x9F / 256.0f, 0x61 / 256.0f);

    // Draw.
    glBegin(GL_QUADS);
    glVertex2f(-1,  1);
    glVertex2f(-1, -1);
    glVertex2f( 1, -1);
    glVertex2f( 1,  1);
    glEnd();

    // End transformation.
    glPopMatrix();

    return true;
}


} }
