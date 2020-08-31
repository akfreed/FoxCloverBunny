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

#include "database/Database.h"

#include "core/Bunny.h"
#include "core/Fox.h"
#include "util/Rng.h"

#include <sqlite_modern_cpp.h>

#include <cstdio>
#include <limits>
#include <fstream>
#include <stdexcept>

using namespace fcb::core;

using sqlite::sqlite_exception;

using SqlDatabase = sqlite::database;

namespace fcb { namespace database {

namespace {


//! Build the schema for a fresh database.
//! @param[in/out] An empty database that will have its tables created.
//! @return true if none of the operations failed.
bool setupTables(SqlDatabase& db)
{
    try
    {
        // Create tables.
        db << "PRAGMA foreign_keys = ON;";

        db << "CREATE TABLE Settings("
                  "rngSeed UNSIGNED);";

        db << "CREATE TABLE Generation("
                  "genNumber INTEGER NOT NULL PRIMARY KEY);";

        db << "CREATE TABLE Species("
                  "speciesName TEXT NOT NULL PRIMARY KEY);";

        db << "CREATE TABLE Animal("
                  "genNumber   NOT NULL REFERENCES Generation(genNumber),"
                  "speciesName NOT NULL REFERENCES Species(speciesName),"
                  "popRank     INTEGER NOT NULL CHECK(popRank >= 0),"
                  "score       INTEGER NOT NULL,"
                  "parent1Id   INTEGER REFERENCES Animal(animalId),"
                  "parent2Id   INTEGER REFERENCES Animal(animalId),"
                  "animalId    INTEGER NOT NULL PRIMARY KEY,"
                  "UNIQUE(genNumber, speciesName, popRank));";

        db << "CREATE TABLE Weights("
                  "animalId INTEGER NOT NULL REFERENCES Animal(animalId),"
                  "w        INTEGER NOT NULL CHECK(w >= 0),"
                  "x        INTEGER NOT NULL CHECK(x >= 0),"
                  "y        INTEGER NOT NULL CHECK(y >= 0),"
                  "value    FLOAT   NOT NULL,"
                  "PRIMARY KEY(animalId, w, x, y));";

        // Populate the species table.
        db << "INSERT INTO Species VALUES('Bunny');";
        db << "INSERT INTO Species VALUES('Fox');";

        // Populate the settings table.
        db << "INSERT INTO Settings (rngSeed) VALUES(?)" << util::RngGlobalInstance().GetSeed();

        return true;
    }
    catch (sqlite_exception& e)
    {
        std::cout << e.what() << '\n'
                  << e.get_sql() << std::endl;
        return false;
    }
}


}  // Anonymous namespace.


// The default ctor/dtor have to be in this file so the database unique_ptr (std::unique_ptr<sqlite::database> m_db),
// which was declared with incomplete type, can be instantiated.
Database::Database() { }
Database::~Database() { }

//! Move constructor.
//! @param[in/out] other The object to move. Will be default-initialized after the operation.
Database::Database(Database&& other) noexcept
    : Database()
{
    swap(*this, other);
}

//! Move assignment.
//! @param[in/out] other The object to move. Will be default-initialized after the operation.
Database& Database::operator=(Database&& other) noexcept
{
    Database temp(std::move(other));
    swap(*this, temp);
    return *this;
}

//! Swaps the two objects.
void swap(Database& left, Database& right) noexcept
{
    using std::swap;
    swap(left.m_db,                right.m_db);
    swap(left.m_currentGeneration, right.m_currentGeneration);
}

//! @return True if the database has been initialized.
//! Encountering an error when using the database will usually disable it.
Database::operator bool() const
{
    return !!m_db;
}

//! The current generation always has database entries.
//! The first generation is 1.
//! @return The current generation. 0 indicates nothing has been saved yet.
int Database::CurrentGeneration() const
{
    return m_currentGeneration;
}

//! Create a new database with storage on the filesystem.
//! @param[in] filename  The path to save the database file. Passing ":memory:" will create the database in memory.
//! @param[in] overwrite Overwrite the file if it already exists.
Database Database::CreateNew(std::string const& filename, bool const overwrite /* = false */)
{
    // Special handling for in-memory database.
    if (filename != "" && filename != ":memory:")
    {
        // Check if file exists.
        std::ifstream file(filename);
        if (file)
        {
            if (!overwrite)
                return {};
            if (std::remove(filename.c_str()) != 0)
            {
                std::perror("Unable to remove already-existing file");
                std::cout << "Disabling database." << std::endl;
                return {};
            }
        }
    }

    auto db = std::make_unique<SqlDatabase>(filename);

    if (!setupTables(*db))
    {
        std::cout << "Unable to setup database. Disabling database." << std::endl;

        assert(false);
        return {};
    }

    Database database;
    database.m_db = std::move(db);
    return database;
}

//! Load a saved database from the filesystem.
//! No consistency checking is done to ensure the loaded database is setup correctly.
//! @param[in] filename The path to the database file.
Database Database::OpenExisting(std::string const& filename)
{
    try
    {
        sqlite::sqlite_config config;
        config.flags = sqlite::OpenFlags::READWRITE;
        auto db = std::make_unique<SqlDatabase>(filename, config);
        // Get the generation number.
        int currentGenNumber = 0;
        *db << "SELECT MAX(genNumber) FROM Generation" >> currentGenNumber;

        Database database;
        database.m_db = std::move(db);
        database.m_currentGeneration = currentGenNumber;
        return database;
    }
    catch (sqlite_exception& e)
    {
        std::cout << e.what() << '\n'
                  << e.get_sql() << std::endl;

        std::cout << "Unable to load database." << std::endl;
        assert(false);
        return {};
    }
}

//! Get the bunnies from the last saved generation.
//! @return A collection of bunnies instantiated from the database.
//!         If nothing is saved in the database, an empty collection is returned.
//! @throw std::runtime_error If the loaded weight dimensions do not fit.
std::vector<std::shared_ptr<Bunny>> Database::LoadGeneration() const
{
    return LoadGeneration(m_currentGeneration);
}

//! Get the bunnies from the given generation.
//! @param[in] generation The given generation.
//! @return A collection of bunnies instantiated from the database.
//!         If the generation is not in the database, an empty collection is returned.
//! @throw std::runtime_error If the loaded weight dimensions do not fit.
std::vector<std::shared_ptr<Bunny>> Database::LoadGeneration(int const generation) const
{
    if (!*this)
    {
        assert(false);
        return {};
    }

    if (generation < 0)
        return {};

    std::vector<std::shared_ptr<Bunny>> parents;

    // If the generation isn't in the database, an empty vector will be returned.

    ((*m_db) << "SELECT animalId FROM Animal WHERE genNumber = ? ORDER BY popRank;" << generation) >> [&parents](int const uuid) {
        parents.push_back(std::make_shared<Bunny>());
        parents.back()->m_uuid = uuid;
    };

    // Load the weights.
    for (auto& bunny : parents)
    {
        auto& weights = bunny->Weights();
        ((*m_db) << "SELECT w, x, y, value FROM Weights WHERE animalId = ? ORDER BY w, x, y;" << bunny->m_uuid)
            >> [&weights](int const w, int const col, int const row, float const value) {
                // Consistency check.
                if (w < 0 || static_cast<size_t>(w) > weights.size() || col > weights[w].cols() || row > weights[w].rows())
                    throw std::runtime_error("Weight dimensions saved in database are too large.");
                weights[w](row, col) = value; 
            };
    }

    return parents;
}

void Database::incrementGeneration()
{
    m_currentGeneration += 1;
    (*m_db) << "INSERT INTO Generation VALUES(?);" << m_currentGeneration;
}

//! Record the given generation.
//! Note that a parent must be from the previous generation.
//! @param[in] bunnies   A vector of shared-pointers to bunny objects representing the current generation. 
//!                      This must be already sorted by rank where the first element is ranked #1.
//! @param[in] parentMap A multimap { child, parent }, where the key is a shared-pointer to the child and the value is a shared-pointer to the parent.
//!                      Each key should have two entries, one for each parent.
bool Database::SaveGeneration(std::vector<std::shared_ptr<Bunny>> const& bunnies, std::multimap<std::shared_ptr<Bunny>, std::shared_ptr<Bunny>> const& parentMap)
{
    if (!*this)
        return false;

    bool success = true;

    (*m_db) << "BEGIN;";
    incrementGeneration();
    for (size_t i = 0; i < bunnies.size(); ++i)
    {
        if (parentMap.count(bunnies[i]) != 2)
        {
            std::cout << "An error occured while saving to the database." << std::endl;
            assert(false);
            success = false;
            break;
        }
        auto const parentsIter = parentMap.lower_bound(bunnies[i]);
        auto const& parent1 = parentsIter->second;
        auto const& parent2 = std::next(parentsIter)->second;
        assert(bunnies.size() <= static_cast<size_t>(std::numeric_limits<int>::max()));
        if (!saveAnimal(m_currentGeneration, *bunnies[i], static_cast<int>(i), parent1.get(), parent2.get()))
        {
            std::cout << "An error occured while saving to the database." << std::endl;
            assert(false);
            success = false;
            break;
        }
    }
    // Commit even if there was an error. It may help with debugging.
    (*m_db) << "COMMIT;";
    if (!success)
    {
        // Disable the database.
        m_db = nullptr;
        return false;
    }

    return true;
}

//! Records a new bunny in the database.
//! This is done after its generation has ended.
//! Note that a parent must be from the previous generation.
//! @param[in]     genNumber The current generation number. No lower numbers can be entered. Besides the current number,
//!                          the next number can be entered, which results in the generation being incremented in the databse.
//! @param[in/out] newBunny  The game object to save. Its UUID will be set so its record can be found later.
//! @param[in]     rank      The rank of the animal among the current generation.
//! @param[in]     parent1   A pointer to first parent or nullptr if this is the first generation.
//! @param[in]     parent2   A pointer to second parent or nullptr if this is the first generation.
//! @return true if the record was successfully inserted. False if there was an error with the arguments and the record was not inserted.
bool Database::saveAnimal(int const genNumber, Bunny& newBunny, int const rank, Bunny const* const parent1, Bunny const* const parent2)
{
    assert((parent1 && parent2) || (!parent1 && !parent2));
    auto const parent1Uuid = parent1 ? parent1->m_uuid : 0;
    auto const parent2Uuid = parent2 ? parent2->m_uuid : 0;

    // We will be casting these to int.
    if (parent1Uuid > std::numeric_limits<int>::max() || parent2Uuid > std::numeric_limits<int>::max())
    {
        assert(false);
        return false;
    }

    return saveGeneric(genNumber, "Bunny", newBunny.Score(), rank, static_cast<int>(parent1Uuid), static_cast<int>(parent2Uuid), newBunny.m_uuid) &&
           saveWeights(newBunny);
}

//! Records a new fox in the database.
//! This is done after its generation has ended.
//! Note that a parent must be from the previous generation.
//! @param[in]     genNumber The current generation number. No lower numbers can be entered. Besides the current, the next number can be entered, which results in the generation being incremented in the databse.
//! @param[in/out] newFox    The game object to save. Its UUID will be set so its record can be found later.
//! @param[in]     rank      The rank of the animal among the current generation.
//! @param[in]     parent1   A pointer to first parent or nullptr if this is the first generation.
//! @param[in]     parent2   A pointer to second parent or nullptr if this is the first generation.
//! @return true if the record was successfully inserted. False if there was an error with the arguments and the record was not inserted.
bool Database::saveAnimal(int const genNumber, Fox& newFox, int const rank, Fox const* parent1, Fox const* parent2)
{
    assert((parent1 && parent2) || (!parent1 && !parent2));
    auto const parent1Uuid = parent1 ? parent1->m_uuid : 0;
    auto const parent2Uuid = parent2 ? parent2->m_uuid : 0;

    // We will be casting these to int.
    if (parent1Uuid > std::numeric_limits<int>::max() || parent2Uuid > std::numeric_limits<int>::max())
    {
        assert(false);
        return false;
    }

    return saveGeneric(genNumber, "Fox", newFox.Score(), rank, static_cast<int>(parent1Uuid), static_cast<int>(parent2Uuid), newFox.m_uuid);
}

//! Insert a new animal into the database.
//! @param[in]  genNumber   The current generation number. Must be > 0. If 1 (first generation), will not assign parents.
//! @param[in]  species     The species of the animal. Must be a valid key in the Species table.
//! @param[in]  score       The animal's score.
//! @param[in]  rank        The rank of the animal among the current generation.
//! @param[in]  parent1Uuid The table ID of the first parent or 0 if this is the first generation. The parent must be from the preceding generation.
//! @param[in]  parent2Uuid The table ID of the first parent or 0 if this is the first generation. The parent must be from the preceding generation.
//! @param[out] out_newUuid The resulting table ID of this animal after registering it.
//! @return true if the record was successfully inserted. False if there was an error and the record was not inserted.
bool Database::saveGeneric(int const genNumber, std::string species, int const score, int const rank, int const parent1Uuid, int const parent2Uuid, uint64_t& out_newUuid)
{
    if (!*this)
    {
        assert(false);
        return false;
    }

    // Alias the pointer;
    SqlDatabase& db = *m_db;

    try
    {
        assert(genNumber > 0 && genNumber == m_currentGeneration);

        if (genNumber == 1)
        {
            db << "INSERT INTO Animal (genNumber, speciesName, popRank, score) VALUES(?, ?, ?, ?)"
               << genNumber << species << rank << score;
        }
        else
        {
            // Verify the parent IDs.
            for (auto id : { parent1Uuid, parent2Uuid })
            {
                bool valid = false;
                (db << "SELECT genNumber, speciesName FROM Animal WHERE animalId = ?;" << id) >> [&valid, genNumber, species](int const parentGenNumber, std::string const& parentSpeciesName) { valid = (parentGenNumber == genNumber - 1 && parentSpeciesName == species); };
                if (!valid)
                    return false;
            }
            db << "INSERT INTO Animal (genNumber, speciesName, popRank, score, parent1Id, parent2Id) VALUES(?, ?, ?, ?, ?, ?)"
               << genNumber << species << rank << score << parent1Uuid << parent2Uuid;
        }
        // Get the ID of the record we just inserted and set the UUID.
        auto const lastId = db.last_insert_rowid();
        if (lastId <= 0)
        {
            std::cout << "Unable to insert record in database. Disabling database." << std::endl;
            assert(false);
            m_db = nullptr;
            return false;
        }
        out_newUuid = static_cast<uint64_t>(lastId);

        return true;
    }
    catch (sqlite_exception const& e)
    {
        std::cout << e.what() << '\n'
                  << e.get_sql() << std::endl;

        std::cout << "Unable to insert record in database. Disabling database." << std::endl;
        assert(false);
        m_db = nullptr;
        return false;
    }
}

//! Save an animal's weights to the database.
//! @param[in] bunny The animal whose weights to record.
bool Database::saveWeights(fcb::core::Bunny const& bunny)
{
    if (!*this || bunny.m_uuid == 0)
    {
        assert(false);
        return false;
    }

    auto const& weights = bunny.Weights();
    try
    {
        for (size_t w = 0; w < weights.size(); ++w)
        {
            for (Eigen::Index col = 0; col < weights[w].cols(); ++col)
            {
                for (Eigen::Index row = 0; row < weights[w].rows(); ++row)
                {
                    float const value = weights[w](row, col);
                    assert(w <= static_cast<size_t>(std::numeric_limits<int>::max()));
                    (*m_db) << "INSERT INTO Weights (animalId, w, x, y, value) VALUES(?, ?, ?, ?, ?)"
                            << bunny.m_uuid << static_cast<int>(w) << col << row << value;
                }
            }
        }
    }
    catch (sqlite_exception const& e)
    {
        std::cout << e.what() << '\n'
                  << e.get_sql() << std::endl;

        std::cout << "Unable to insert record in database. Disabling database." << std::endl;
        assert(false);
        m_db = nullptr;
        return false;
    }

    return true;
}


} }
