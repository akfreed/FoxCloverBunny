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

#include "core/GameObject.h"

namespace fcb { namespace core {


//! @return The x coordinate of the object.
float GameObject::X() const
{
    return m_x;
}
//! @return A modifiable reference to the x coordinate of the object.
float& GameObject::X()
{
    return m_x;
}

//! @return The y coordinate of the object.
float GameObject::Y() const
{
    return m_y;
}
//! @return A modifiable reference to the y coordinate of the object.
float& GameObject::Y()
{
    return m_y;
}

//! @return The orientation of the object in radians. Range is [-2*pi, 2*pi].
float GameObject::Angle() const
{
    return m_angle;
}
//! @return A modifiable reference to the angle of the object in radians.
float& GameObject::Angle()
{
    return m_angle;
}

//! @return The radius of the object's circular hitbox.
float GameObject::Radius() const
{
    return m_radius;
}
//! @return A modifiable reference to the radius angle of the object's hitbox.
float& GameObject::Radius()
{
    return m_radius;
}

//! @param[in] x An amount to add to the x coordinate.
//! @param[in] y An amount to add to the y coordinate.
void GameObject::Translate(float const x, float const y)
{
    m_x += x;
    m_y += y;
}

//! Move the object in the direction it is facing (look-at vector) by the given distance.
//! @param[in] distance The amount to advance the object.
void GameObject::MoveForward(float const distance)
{
    float x = 0;
    float y = 0;
    GetLookAtVector(x, y);
    Translate(x * distance, y * distance);
}

//! @param[in] angle An amount (in radians) to add to the current angle.
void GameObject::Rotate(float const angle)
{
    m_angle += angle;
    if (m_angle > static_cast<float>(2 * M_PI))
        m_angle -= static_cast<float>(2 * M_PI);
    else if (m_angle < static_cast<float>(-2 * M_PI))
        m_angle += static_cast<float>(2 * M_PI);
}

// Get the normalized look-at vector of this object based on its angle.
//! @param[out] out_x Will be set to the normalized x component of the look-at vector depending on the object's angle.
//! @param[out] out_y Will be set to the normalized y component of the look-at vector depending on the object's angle.
void GameObject::GetLookAtVector(float& out_x, float& out_y) const
{
    out_x = cosf(m_angle);
    out_y = sinf(m_angle);
}
// Set the angle of this object given a look-at vector.
//! @param[in] x The x component of the look-at vector to set the angle to. Does not need to be normalized.
//! @param[in] y The y component of the look-at vector to set the angle to. Does not need to be normalized.
void GameObject::SetLookAtVector(float const x, float const y)
{
    if (x == 0 && y == 0)
        return;
    m_angle = atan2f(y, x);
}

// Given another object, calculate the normalized vector between them.
//! @param[in]  other The other object to calculate the vector to.
//! @param[out] out_x Will be set to the normalized x component of the vector from this object to the other.
//! @param[out] out_y Will be set to the normalized y component of the vector from this object to the other.
void GameObject::CalcVectorTo(GameObject const& other, float& out_x, float& out_y) const
{
    float const distance = Distance(other);
    if (distance == 0)
    {
        out_x = 0;
        out_y = 0;
    }
    else
    {
        out_x = (other.m_x - m_x) / distance;
        out_y = (other.m_y - m_y) / distance;
    }
}

//! @param[in] other Another class instance.
//! @return The euclidean distance from this object to the other.
float GameObject::Distance(GameObject const& other) const
{
    return sqrtf(DistanceSquared(other));
}

//! Returns a value that can be used to compare distances.
//! Avoids a call to the square-root function.
//! @param[in] other    Another class instance.
//! @param[in] distance A given distance.
//! @return The square of the euclidean distance from this object to the other.
float GameObject::DistanceSquared(GameObject const& other) const
{
    return (m_x - other.m_x) * (m_x - other.m_x) + (m_y - other.m_y) * (m_y - other.m_y);
}

//! Checks if another object is within a given distance.
//! Avoids a call to the square-root function.
//! @param[in] other    Another class instance.
//! @param[in] distance A given distance.
//! @return True, if the euclidean distance from this object to the other is less than or equal to the given argument.
bool GameObject::DistanceLessOrEqual(GameObject const& other, float const distance) const
{
    return DistanceSquared(other) <= distance * distance;
}


} }
