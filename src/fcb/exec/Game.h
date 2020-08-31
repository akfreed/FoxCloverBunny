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

#include "database/Database.h"

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace fcb { namespace core {
    class Bunny;
} }

namespace fcb { namespace exec {

//! Contains the code to run the main simulation.
class Game
{
public:
    Game();
    explicit Game(std::string const& pathToExistingDatabase);

    void Run();

private:
    static size_t constexpr c_numBunnies = 50;

    fcb::database::Database m_database;
    std::vector<std::shared_ptr<fcb::core::Bunny>> m_bunnies;
    std::vector<std::shared_ptr<fcb::core::Bunny>> m_bunniesPrev;  // The previous generation.
    std::multimap<std::shared_ptr<fcb::core::Bunny>, std::shared_ptr<fcb::core::Bunny>> m_parentMap;  // For remembering each bunny's parents.

    void initNew();
    void initFromExisting(std::string const& path);
    void doBreeding();
};

} }

