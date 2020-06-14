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

namespace fcb { namespace core {

//! Holds common state and operations for game objects.
class GameObject
{
public:
    float  X() const;
    float& X();
    float  Y() const;
    float& Y();
    float  Angle() const;
    float& Angle();
    float  Radius() const;
    float& Radius();

    void Translate(float const x, float const y);
    void MoveForward(float const distance);
    void Rotate(float const angle);
    void GetLookAtVector(float& out_x, float& out_y) const;
    void SetLookAtVector(float const x, float const y);
    void CalcVectorTo(GameObject const& other, float& out_x, float& out_y) const;
    float Distance(GameObject const& other) const;
    float DistanceSquared(GameObject const& other) const;
    bool DistanceLessOrEqual(GameObject const& other, float const distance) const;

private:
    float m_x = 0;
    float m_y = 0;
    float m_angle = 0;
    float m_radius = 1;
};

} }
