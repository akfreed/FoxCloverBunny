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

#include "Game.h"

#include "core/Globals.h"
#include "core/Fox.h"
#include "core/Clover.h"
#include "core/Bunny.h"
#include "input/InputState.h"
#include "graphics/ObjectRegistry.h"
#include "gui/Gui.h"
#include "ml/GeneticAlgorithmPairing.h"
#include "util/Util.h"
#include "DatabaseSetup.h"

#include <PerformanceTimer98.hpp>

#include <random>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace fcb;
using namespace fcb::core;

using fcb::database::Database;

namespace fcb { namespace exec {

namespace {


std::uniform_real_distribution<float> s_distPosition(-1, 1);
std::uniform_real_distribution<float> s_distAngle(0, 2 * static_cast<float>(M_PI));

void resetObject(Clover& clover)
{
    clover = Clover();
    clover.X() = s_distPosition(util::rng());
    clover.Y() = s_distPosition(util::rng());
}

void resetObject(Bunny& bunny)
{
    bunny = Bunny();
    bunny.X() = s_distPosition(util::rng());
    bunny.Y() = s_distPosition(util::rng());
    bunny.Angle() = s_distAngle(util::rng());
}

void resetObject(Fox& fox)
{
    void(*unused)(Fox&) = resetObject;
    UNUSED(unused);
    fox = Fox();
    fox.X() = s_distPosition(util::rng());
    fox.Y() = s_distPosition(util::rng());
    fox.Angle() = s_distAngle(util::rng());
}

void enforceBounds(GameObject& object)
{
    if (object.X() > Globals::c_worldRightBound)
        object.X() = Globals::c_worldWrap ? Globals::c_worldLeftBound : Globals::c_worldRightBound;
    else if (object.X() < Globals::c_worldLeftBound)
        object.X() = Globals::c_worldWrap ? Globals::c_worldRightBound : Globals::c_worldLeftBound;

    if (object.Y() > Globals::c_worldTopBound)
        object.Y() = Globals::c_worldWrap ? Globals::c_worldBottomBound : Globals::c_worldTopBound;
    else if (object.Y() < Globals::c_worldBottomBound)
        object.Y() = Globals::c_worldWrap ? Globals::c_worldTopBound : Globals::c_worldBottomBound;
}


}  // Anonymous namespace.


//! Default constructor.
//! Initialize a new game and database on disk.
Game::Game()
{
    initNew();
}

//! Load and initialize a game from an existing database.
//! @param[in] pathToExistingDatabase The path to the database.
Game::Game(std::string const& pathToExistingDatabase)
{
    initFromExisting(pathToExistingDatabase);
}

//! The main game loop.
void Game::Run()
{
    std::cout << "RNG Initial Seed: " << util::RngGlobalInstance().GetSeed() << std::endl;

    std::vector<std::shared_ptr<Clover>> clovers;
    for (int i = 0; i < 200; ++i)
    {
        auto clover = std::make_shared<Clover>();
        resetObject(*clover);
        graphics::RegisterObject(clover);
        clovers.push_back(std::move(clover));
    }

    for (auto& bunny : m_bunnies)
        graphics::RegisterObject(bunny);

    PerformanceTimer98 timer;
    unsigned generation = m_database.CurrentGeneration() + 1;

    while (!input::GetInputState().exit)
    {
        std::cout << "\nBeginning generation: " << generation << std::endl;

        // Run the current generation.
        timer.Start();
        for (unsigned numCycles = 0; !input::GetInputState().exit && numCycles < Globals::c_secondsPerGeneration * 60; ++numCycles)
        {
            for (auto& bunny : m_bunnies)
            {
                // Find nearest clover.
                auto nearestCloverIter = std::min_element(clovers.begin(), clovers.end(), [&bunny](auto const& left, auto const& right) {
                    return bunny->DistanceSquared(*left) < bunny->DistanceSquared(*right); });
                auto nearestClover = *nearestCloverIter;

                bunny->Think(*nearestClover);

                bunny->Act();

                // Check bounds.
                enforceBounds(*bunny);

                // Handle bunny/clover collision.
                if (bunny->Distance(*nearestClover) < bunny->Radius())
                {
                    if (nearestClover->Bite())
                        bunny->NumCloversEaten() += 1;
                    // If the clover is out of HP, reset it.
                    if (nearestClover->Hp() == 0)
                        resetObject(**nearestCloverIter);
                }
            }

            // Draw.
            if (!input::GetInputState().fastForward)
            {
                gui::Draw();

                // Wait for the frame time to expire.
                for (timer.Stop(); !timer.IntervalHasElapsed(); timer.Stop())
                    std::this_thread::yield();
                timer.Start();
            }
            else
            {
                // When fast-forwarding, still check for events every so often.
                timer.Stop();
                if (timer.GetElapsed() > 100)
                {
                    gui::HandleEvents();
                    timer.Start();
                }
            }
        }

        // Handle keyboard events (needed if fast-forwarding).
        gui::HandleEvents();
        if (input::GetInputState().exit)
            continue;

        // Score and rank the bunnies.
        std::sort(m_bunnies.begin(), m_bunnies.end(), [](auto& left, auto& right) { return left->Score() > right->Score(); });

        // Display statistics for the generation.
        {
            assert(m_bunnies.size() > 0);

            std::vector<int> scores;
            std::transform(m_bunnies.cbegin(), m_bunnies.cend(), std::back_inserter(scores), [](auto const& bunny) { return bunny->Score(); });

            size_t const length = scores.size();
            int const max = scores.front();
            int const min = scores.back();
            double median = 0;
            if (length % 2 == 1)
                median = scores[length / 2];
            else
                median = (scores[length / 2 - 1] + scores[length / 2]) / 2;

            double const mean = std::accumulate(scores.cbegin(), scores.cend(), 0.0, [](double const acc, int const score) { return acc + score; }) / static_cast<double>(length);
            double const sumOfSquaredDiff = std::accumulate(scores.cbegin(), scores.cend(), 0.0, [mean](double const acc, int const score) { return acc + std::pow(score - mean, 2.0); });
            double const stddev = std::sqrt(sumOfSquaredDiff / static_cast<double>(length - 1));

            std::cout << "    Bunny Scores:\n"
                      << "        max:       " << max << "\n"
                      << "        min:       " << min << "\n"
                      << "        mean:      " << mean << "\n"
                      << "        median:    " << median << "\n"
                      << "        std. dev.: " << stddev << std::endl;
        }

        // Save to database.
        if (!m_database.SaveGeneration(m_bunnies, m_parentMap))
        {
            std::cout << "An error occured while saving to the database." << std::endl;
            assert(false);
        }

        // Create the next generation.
        for (auto& bunny : m_bunnies)
            graphics::UnregisterObject(bunny);
        std::swap(m_bunnies, m_bunniesPrev);
        for (auto& bunny : m_bunnies)
        {
            resetObject(*bunny);
            graphics::RegisterObject(bunny);
        }

        doBreeding();

        ++generation;
    }
}

//! Create a new database on disk and initialize the instance members.
void Game::initNew()
{
    m_bunnies.clear();
    m_bunniesPrev.clear();
    m_parentMap.clear();

    m_database = SetupNewDatabaseOnDisk();
    if (!m_database)
        throw std::runtime_error("Unable to initialize database.");

    for (size_t i = 0; i < c_numBunnies; ++i)
    {
        auto bunny = std::make_shared<Bunny>();
        resetObject(*bunny);
        // No parents.
        m_parentMap.emplace(bunny, nullptr);
        m_parentMap.emplace(bunny, nullptr);

        m_bunnies.push_back(std::move(bunny));
        m_bunniesPrev.push_back(std::make_shared<Bunny>());
    }
}

//! Open an existing database from disk, load the latest generation as parents, and breed a new generation.
//! @param[in] path The path to an existing database.
void Game::initFromExisting(std::string const& path)
{
    m_bunnies.clear();

    std::cout << "Loading database file: " << path << std::endl;
    m_database = Database::OpenExisting(path);
    if (!m_database)
        throw std::runtime_error("Unable to initialize database.");

    m_bunniesPrev = m_database.LoadGeneration();
    if (m_bunniesPrev.empty())
        throw std::runtime_error("Nothing saved in database.");

    for (size_t i = 0; i < m_bunniesPrev.size(); ++i)
    {
        m_bunnies.push_back(std::make_shared<Bunny>());
        resetObject(*m_bunnies.back());
    }

    doBreeding();
}

void Game::doBreeding()
{
    m_parentMap.clear();
    auto lCrossoverHelperBunny = [this](std::shared_ptr<Bunny> const& m, std::shared_ptr<Bunny> const& f, std::shared_ptr<Bunny>& out_c) {
        Bunny::Crossover(*m, *f, *out_c);
        // Record each bun's parents.
        assert(m_parentMap.count(out_c) == 0);
        // Use insert instead of emplace to preserve the order.
        m_parentMap.insert({ out_c, m });
        m_parentMap.insert({ out_c, f });
    };
    ml::BreedPopChance50(m_bunniesPrev, m_bunnies, lCrossoverHelperBunny);
    assert(m_parentMap.size() == m_bunnies.size() * 2);
}


} }

