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

#include "ml/GeneticAlgorithmPairing.h"

#include "util/Util.h"

#include <numeric>

namespace fcb { namespace ml {

namespace {

    //! Pick an index from the "pie chart".
    //! Assumes 0 <= pick < sum(pie).
    //! @param[in] pie  A vector of probabilities.
    //! @param[in] pick A number on a scale.
    //! @return An index.
    size_t selectIndex_unchecked(std::vector<size_t> const& pie, size_t const pick)
    {
        // Find the index corresponding to the selected number.
        size_t index = 0;
        size_t sum = 0;
        while (true)
        {
            sum += pie[index];
            if (pick < sum)
                break;
            ++index;
        }

        return index;
    }

}  // Anonymous namespace.


//! Bit-wise breed two floats.
//! @param[in] f_m Parent float one.
//! @param[in] f_f Parent float two.
//! @return A mix of the common bits plus a chance to flip bits.
float breedFloat(float const f_m, float const f_f)
{
    float constexpr mutationRate = .05f;

    static_assert(sizeof(float) == sizeof(uint32_t), "Floating points on this architecture are not 32-bit.");

    std::uniform_real_distribution<> distReal(0, 1);
    std::uniform_int_distribution<> distInt(0, 31);

    // Convert to uint.
    uint32_t u_m, u_f;
    std::memcpy(&u_m, &f_m, sizeof(uint32_t));
    std::memcpy(&u_f, &f_f, sizeof(uint32_t));

    // Create a mask.
    // The 0's mean no change. The 1's should be randomly selected.
    auto const mask = u_m ^ u_f;
    // Create a random mask.
    auto const rand = static_cast<uint32_t>(util::rng()());
    // Create a flipping mask. 1=flip. 0=no flip.
    auto flipper = mask & rand;

    for (int i = 0; i < 32; ++i)
    {
        if (distReal(util::rng()) < mutationRate)
            flipper ^= 1 << distInt(util::rng());
    }

    //0101 m
    //0011 f
    //0110 ^
    //???? r
    //0??0 flip = (^) & (mask)
    //0??1 flip ^ m

    // Could have picked m or f.
    uint32_t const u_c = u_f ^ flipper;

    // Convert result to float.
    float f_c;
    std::memcpy(&f_c, &u_c, sizeof(float));
    return f_c;
}

//! Select a parent by returning a random rank index.
//! Lower indexes (i.e. higher rank) has a higher chance of being selected.
//! The chance is hard-coded in this function (bad design, but works for now).
//! @return An index into the ranked parent vector (assumes size 20).
size_t selectIndex20()
{
    // Create a "pie chart" of probabilities.
    // Lower indexes have a better chance of being selected.
    // e.g. Index 0 has 20/100 chance, index 1 has 18/100 chance...
    // Keep in mind that parent selection happens 40 times (2 for each of the 20 children).
    static std::vector<size_t> const c_pie{ 20, 18, 15, 10, 8, 5, 3, 3, 3, 2,
                                                2,  2,  2,  1, 1, 1, 1, 1, 1, 1 };
    size_t constexpr total = 100;
    assert(std::accumulate(cbegin(c_pie), cend(c_pie), size_t(0)) == total && c_pie.size() == 20);

    // Pick a number between 0 and 99.
    std::uniform_int_distribution<size_t> dist(0, total - 1);
    size_t const pick = dist(util::rng());

    return selectIndex_unchecked(c_pie, pick);
}

//! Select a parent by returning a random rank index.
//! Lower indexes (i.e. higher rank) has a higher chance of being selected.
//! The chance is hard-coded in this function (bad design, but works for now).
//! @return An index into the ranked parent vector (assumes size 50).
size_t selectIndex50()
{
    // Create a "pie chart" of probabilities.
    // Lower indexes have a better chance of being selected.
    // e.g. Index 0 has 30/200 chance, index 1 has 25/200 chance...
    // Keep in mind that parent selection happens 100 times (2 for each of the 50 children).
    static std::vector<size_t> const c_pie{ 30, 25, 20, 15, 12, 10, 10, 8, 6, 4,
                                                3,  3,  3,  3,  3,  3,  3, 2, 2, 2,
                                                2,  2,  2,  1,  1,  1,  1, 1, 1, 1,
                                                1,  1,  1,  1,  1,  1,  1, 1, 1, 1,
                                                1,  1,  1,  1,  1,  1,  1, 1, 1, 1 };
    size_t constexpr total = 200;
    assert(std::accumulate(cbegin(c_pie), cend(c_pie), size_t(0)) == total && c_pie.size() == 50);

    // Pick a number between 0 and 199.
    std::uniform_int_distribution<size_t> dist(0, total - 1);
    size_t const pick = dist(util::rng());

    return selectIndex_unchecked(c_pie, pick);
}


} }
