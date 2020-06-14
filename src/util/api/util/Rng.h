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

#include <random>
#include <chrono>
#include <memory>

namespace fcb { namespace util {


//! Stores the seed with the RNG so it can be retrieved.
class Rng
{
public:
    using Generator = std::mt19937_64;
    using result_type = Generator::result_type;

    Rng()
        : m_seed(std::chrono::steady_clock::now().time_since_epoch().count())
        , m_rand(std::make_unique<Generator>(m_seed))
    { }

    //! @return The seed for the random number generator.
    result_type GetSeed() const
    {
        return m_seed;
    }
    //! Set the seed for the random number generator.
    //! @param[in] seed The seed.
    void SetSeed(result_type const seed)
    {
        m_seed = seed;
        m_rand->seed(m_seed);
    }

    //! Set the seed for the random number generator to the default seed.
    void SeedDefault()
    {
        SetSeed(Generator::default_seed);
    }

    //! @return The internal RNG object.
    Generator& GetRng()
    {
        return *m_rand;
    }

private:
    result_type m_seed;
    std::unique_ptr<Generator> m_rand;
};


//! Get the random number generator instance.
//! @return A reference to the global Rng class instance.
inline Rng& RngGlobalInstance()
{
    static Rng s_rand;
    return s_rand;
}

//! Shorthand for RngGlobalInstance().GetRng()
//! @return A reference to the global Rng's URBG object.
inline Rng::Generator& rng()
{
    return RngGlobalInstance().GetRng();
}


} }
