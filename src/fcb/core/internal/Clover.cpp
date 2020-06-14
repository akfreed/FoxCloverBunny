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

#include "core/Clover.h"

#include "core/Globals.h"

namespace fcb { namespace core {


Clover::Clover()
    : m_hp(Globals::c_cloverHp)
{
    this->Radius() = 0.005f;
    this->Angle() = static_cast<float>(M_PI) / 4;
}

//! @return the number of bites the clover has remaining.
unsigned Clover::Hp() const
{
    return m_hp;
}

//! Remove 1 HP from the clover.
//! @return false if the clover's HP was 0 before the bite.
bool Clover::Bite()
{
    if (m_hp == 0)
        return false;
    m_hp -= 1;
    return true;
}


} }
