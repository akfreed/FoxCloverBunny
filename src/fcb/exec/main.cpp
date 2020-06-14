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

#define _USE_MATH_DEFINES
#include <cmath>

#include "core/Clover.h"
#include "core/Bunny.h"
#include "core/Fox.h"
#include "core/Globals.h"
#include "input/InputState.h"
#include "graphics/ObjectRegistry.h"
#include "gui/Gui.h"
#include "ml/GeneticAlgorithmPairing.h"
#include "util/Util.h"

#include <PerformanceTimer98.hpp>

#include <thread>
#include <memory>
#include <random>
#include <algorithm>
#include <iostream>

using namespace fcb;
using namespace fcb::core;


namespace {

std::uniform_real_distribution<float> s_distPosition(-1, 1);
std::uniform_real_distribution<float> s_distAngle(0, 2 * static_cast<float>(M_PI));

std::shared_ptr<Clover> MakeClover()
{
    auto clover = std::make_shared<Clover>();
    clover->X() = s_distPosition(util::rng());
    clover->Y() = s_distPosition(util::rng());
    graphics::RegisterObject(clover);
    return clover;
}

std::shared_ptr<Bunny> MakeBunny()
{
    auto bunny = std::make_shared<Bunny>();
    bunny->X() = s_distPosition(util::rng());
    bunny->Y() = s_distPosition(util::rng());
    bunny->Angle() = s_distAngle(util::rng());
    graphics::RegisterObject(bunny);
    return bunny;
}

std::shared_ptr<Fox> MakeFox()
{
    auto fox = std::make_shared<Fox>();
    fox->X() = s_distPosition(util::rng());
    fox->Y() = s_distPosition(util::rng());
    fox->Angle() = s_distAngle(util::rng());
    graphics::RegisterObject(fox);
    return fox;
}

void EnforceBounds(GameObject& object)
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

void run()
{
    UNUSED(MakeFox());

    std::vector<std::shared_ptr<Clover>> clovers;
    std::vector<std::shared_ptr<Bunny>> bunnies;

    for (int i = 0; i < 200; ++i)
        clovers.push_back(MakeClover());

    for (int i = 0; i < 50; ++i)
        bunnies.push_back(MakeBunny());

    PerformanceTimer98 timer;
    unsigned generation = 0;

    while (!input::GetInputState().exit)
    {
        std::cout << "Generation: " << generation << std::endl;

        // Run the current generation.
        timer.Start();
        for (unsigned numCycles = 0; !input::GetInputState().exit && numCycles < Globals::c_secondsPerGeneration * 60; ++numCycles)
        {
            for (auto& bunny : bunnies)
            {
                // Find nearest clover.
                auto nearestCloverIter = std::min_element(clovers.begin(), clovers.end(), [&bunny](auto const& left, auto const& right) {
                    return bunny->DistanceSquared(*left) < bunny->DistanceSquared(*right); });
                auto nearestClover = *nearestCloverIter;

                bunny->Think(*nearestClover);

                bunny->Act();

                // Check bounds.
                EnforceBounds(*bunny);

                // Handle bunny/clover collision.
                if (bunny->Distance(*nearestClover) < bunny->Radius())
                {
                    if (nearestClover->Bite())
                        bunny->NumCloversEaten() += 1;
                    // If the clover is out of HP, reset it.
                    if (nearestClover->Hp() == 0)
                        *nearestCloverIter = MakeClover();
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

        // Rank the bunnies.
        std::sort(bunnies.begin(), bunnies.end(), [](auto& left, auto& right) { return left->NumCloversEaten() > right->NumCloversEaten(); });
        std::cout << "    Bunny top score: " << bunnies[0]->NumCloversEaten() << std::endl;

        // Create the next generation.
        std::vector<std::shared_ptr<Bunny>> bunniesSwap;
        for (size_t i = 0; i < bunnies.size(); ++i)
            bunniesSwap.push_back(MakeBunny());

        // Do GA breeding.
        auto lCrossoverHelperBunny = [](std::shared_ptr<Bunny> const& m, std::shared_ptr<Bunny> const& f, std::shared_ptr<Bunny>& out_c) {
            Bunny::Crossover(*m, *f, *out_c); };
        ml::BreedPopChance50(bunnies, bunniesSwap, lCrossoverHelperBunny);
        std::swap(bunnies, bunniesSwap);

        ++generation;
    }
}

}  // Anonymous namespace.


int main()
{
    gui::Init();
    run();
    gui::Deinit();

    return 0;
}
