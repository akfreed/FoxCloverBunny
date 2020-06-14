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

#include <cassert>
#include <vector>

namespace fcb { namespace ml {

size_t selectIndex20();
size_t selectIndex50();
float breedFloat(float const f_m, float const f_f);

//! Fixed breeding based on rank.
//! Population size must be 20.
//! Round-robin breeding of top 5 ranked chromosomes (every one with every other).
//! Round-robin breeding of ranks 6-10.
//! @param pop[in]       The current generation. Should be size 20.
//! @param out_pop[out]  Pre-allocated vector for the next generation. Must be a different collection than pop. Must be the same size as pop.
//! @param crossover[in] A callable that performs chromosome crossover. Signature must be (T const& m, T const& f, T& out_c) -> void.
template <typename T, typename CrossoverFunctor>
void BreedPopFixed20(std::vector<T> const& pop, std::vector<T>& out_pop, CrossoverFunctor&& crossover)
{
    if (&pop == &out_pop || pop.size() != out_pop.size() || pop.size() != 20)
    {
        assert(false);
        return;
    }

    crossover(pop[0], pop[1], out_pop[0]);
    crossover(pop[0], pop[2], out_pop[1]);
    crossover(pop[0], pop[3], out_pop[2]);
    crossover(pop[0], pop[4], out_pop[3]);
    crossover(pop[1], pop[2], out_pop[4]);
    crossover(pop[1], pop[3], out_pop[5]);
    crossover(pop[1], pop[4], out_pop[6]);
    crossover(pop[2], pop[3], out_pop[7]);
    crossover(pop[2], pop[4], out_pop[8]);
    crossover(pop[3], pop[4], out_pop[9]);

    crossover(pop[5], pop[6], out_pop[10]);
    crossover(pop[5], pop[7], out_pop[11]);
    crossover(pop[5], pop[8], out_pop[12]);
    crossover(pop[5], pop[9], out_pop[13]);
    crossover(pop[6], pop[7], out_pop[14]);
    crossover(pop[6], pop[8], out_pop[15]);
    crossover(pop[6], pop[9], out_pop[16]);
    crossover(pop[7], pop[8], out_pop[17]);
    crossover(pop[7], pop[9], out_pop[18]);
    crossover(pop[8], pop[9], out_pop[19]);
}

//! Select parents randomly, with higher ranks given a higher selection chance.
//! Population size must be 20.
//! The chance is determined in the sub-function.
//! Note that a parent can breed with itself.
//! @param pop[in]      The current generation. Should be size 20.
//! @param out_pop[out] Pre-allocated vector for the next generation. Must be a different collection than pop. Must be the same size as pop.
//! @param crossover[in] A callabe that performs chromosome crossover. Signature must be (T const& m, T const& f, T& out_c) -> void.
template <typename T, typename CrossoverFunctor>
void BreedPopChance20(std::vector<T> const& pop, std::vector<T>& out_pop, CrossoverFunctor&& crossover)
{
    if (&pop == &out_pop || pop.size() != out_pop.size() || pop.size() != 20)
    {
        assert(false);
        return;
    }

    for (auto& out_p : out_pop)
    {
        size_t const mIndex = selectIndex20();
        size_t const fIndex = selectIndex20();
        crossover(pop[mIndex], pop[fIndex], out_p);
    }
}

//! Select parents randomly, with higher ranks given a higher selection chance.
//! Population size must be 50.
//! The chance is determined in the sub-function.
//! Note that a parent can breed with itself.
//! @param pop[in]      The current generation. Should be size 50.
//! @param out_pop[out] Pre-allocated vector for the next generation. Must be a different collection than pop. Must be the same size as pop.
//! @param crossover[in] A callabe that performs chromosome crossover. Signature must be (T const& m, T const& f, T& out_c) -> void.
template <typename T, typename CrossoverFunctor>
void BreedPopChance50(std::vector<T> const& pop, std::vector<T>& out_pop, CrossoverFunctor&& crossover)
{
    if (&pop == &out_pop || pop.size() != out_pop.size() || pop.size() != 50)
    {
        assert(false);
        return;
    }

    for (auto& out_p : out_pop)
    {
        size_t const mIndex = selectIndex50();
        size_t const fIndex = selectIndex50();
        crossover(pop[mIndex], pop[fIndex], out_p);
    }
}

} }
