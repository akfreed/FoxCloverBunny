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

#include "core/GameObject.h"
#include "ml/NeuralNet.h"

namespace fcb { namespace core {

class Bunny;

//! Holds the game state for a fox.
class Fox : public GameObject
{
public:
    Fox();

    unsigned  NumBunniesEaten() const;
    unsigned& NumBunniesEaten();
    void Think(Bunny const& nearestBunny);
    void Act();
    int Score() const;

    static void Crossover(Fox const& m, Fox const& f, Fox& out_c);

    uint64_t m_uuid = 0;  // Only used externally. Set and used by the database.

private:
    unsigned m_numBunniesEaten = 0;
    float m_speed = .0034f;
    fcb::ml::NeuralNet m_brain;
    fcb::ml::NeuralNet::OutputType m_outputs;
};

} }
