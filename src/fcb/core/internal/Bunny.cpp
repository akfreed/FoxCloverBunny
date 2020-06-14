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

#include "core/Bunny.h"

#include "core/Clover.h"
#include "core/Globals.h"

#include <Eigen/Dense>

using namespace fcb::ml;

namespace fcb { namespace core {


Bunny::Bunny()
    : m_brain(Globals::c_numHiddenNodes)
{
    this->Radius() = 0.02f;
}

//! @return The number of clovers this bunny has eaten.
unsigned Bunny::NumCloversEaten() const
{
    return m_numCloversEaten;
}
//! @return A modifiable reference to the number of clovers this bunny has eaten.
unsigned& Bunny::NumCloversEaten()
{
    return m_numCloversEaten;
}

//! Activate the brain, converting inputs to outputs.
//! @param[in] nearestClover The nearest clover to the bunny.
void Bunny::Think(Clover const& nearestClover)
{
    NeuralNet::InputType inputs;
    // Set inputs 0 and 1 to this object's normalized look-at vector.
    this->GetLookAtVector(inputs(0), inputs(1));
    // Set inputs 2 and 3 to the normalized vector to the target.
    this->CalcVectorTo(nearestClover, inputs(2), inputs(3));

    m_brain.FeedForward(inputs, m_outputs);
}

//! Perform actions based on the outputs.
void Bunny::Act()
{
    float const leftForce  = (m_outputs(0)) * m_speed;
    float const rightForce = (m_outputs(1)) * m_speed;

    float const rotateBy = (leftForce - rightForce) * 200;
    float const speed = leftForce + rightForce;

    this->Rotate(rotateBy);
    this->MoveForward(speed);
}

//! Perform gene crossover. Combine m and f and output offspring genes.
//! @param[in]  m     A bunny.
//! @param[in]  f     A bunny. Can be the same as m.
//! @param[out] out_c A bunny instance into which to store the offspring.
void Bunny::Crossover(Bunny const& m, Bunny const& f, Bunny& out_c)
{
    NeuralNet::Crossover(m.m_brain, f.m_brain, out_c.m_brain);
}


} }
