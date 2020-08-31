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

#include <string>
#include <memory>
#include <map>
#include <vector>

namespace fcb { namespace core {
    class Bunny;
    class Fox;
} }

namespace sqlite {
    class database;
}

namespace fcb { namespace database {

//! Contains the database and manages operations on it.
class Database
{
public:
    Database();
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;
    Database(Database const&)            = delete;
    Database& operator=(Database const&) = delete;
    friend void swap(Database& left, Database& right) noexcept;
    ~Database();

    explicit operator bool() const;
    int CurrentGeneration() const;

    static Database CreateNew(std::string const& filename, bool const overwrite=false);
    static Database OpenExisting(std::string const& filename);

    std::vector<std::shared_ptr<fcb::core::Bunny>> LoadGeneration() const;
    std::vector<std::shared_ptr<fcb::core::Bunny>> LoadGeneration(int const generation) const;
    bool SaveGeneration(std::vector<std::shared_ptr<fcb::core::Bunny>> const& bunnies, std::multimap<std::shared_ptr<fcb::core::Bunny>, std::shared_ptr<fcb::core::Bunny>> const& parentMap);

private:
    void incrementGeneration();
    bool saveAnimal(int const genNumber, fcb::core::Bunny& newBunny, int const rank, fcb::core::Bunny const* const parent1, fcb::core::Bunny const* const parent2);
    bool saveAnimal(int const genNumber, fcb::core::Fox&   newFox,   int const rank, fcb::core::Fox   const* const parent1, fcb::core::Fox   const* const parent2);
    bool saveGeneric(int const genNumber, std::string species, int const score, int const rank, int const parent1Uuid, int const parent2Uuid, uint64_t& out_newUuid);
    bool saveWeights(fcb::core::Bunny const& bunny);

    std::unique_ptr<sqlite::database> m_db;
    int m_currentGeneration = 0;
};

} }
