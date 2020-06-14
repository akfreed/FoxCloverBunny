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

#include "GraphicsObjectManager.h"

using namespace fcb::core;

namespace fcb { namespace graphics {

namespace {

// Used with std::visit.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;  // Not needed after C++20.

//! Draw the models in the given collection. The collection should be a std::vector.
//! Removes the models from the collection if their associated game objects have been destroyed.
//! @param[in/out] collection A std::vector of ModelClover, ModelBunny, or ModelFox.
template <typename Model>
void drawGroup(std::vector<Model>& collection)
{
    // A vector to hold const_iterators of another vector.
    std::vector<typename std::vector<Model>::const_iterator> invalidIndexes;

    for (auto iter = collection.cbegin(); iter != collection.cend(); ++iter)
    {
        bool const result = iter->DrawIfValid();
        if (!result)
            invalidIndexes.push_back(iter);
    }

    // Remove invalidated weak pointers. Erase in reverse so the iterators aren't invalidated.
    for (auto iterIter = invalidIndexes.crbegin(); iterIter != invalidIndexes.crend(); ++iterIter)
        collection.erase(*iterIter);
}

}  // Anonymous namespace.


//! Saves a weak reference to a game object.
//! @param[in] gameObject A weak pointer to a Fox, Clover, or Bunny.
//! @return True if successful.
bool GraphicsObjectManager::RegisterObject(GameObjectPointer gameObject)
{
    std::visit(overloaded{
        [this](std::weak_ptr<Clover> clover) { m_clovers.emplace_back(std::move(clover)); },
        [this](std::weak_ptr<Bunny>  bunny)  { m_bunnies.emplace_back(std::move(bunny)); },
        [this](std::weak_ptr<Fox>    fox)    {   m_foxes.emplace_back(std::move(fox)); },
    }, gameObject);

    return true;
}

//! Should only be called when OpenGL is ready to draw.
//! Draws the objects. If an object is invalid, it is removed from the collection.
void GraphicsObjectManager::draw()
{
    drawGroup(m_clovers);
    drawGroup(m_bunnies);
    drawGroup(m_foxes);
}


} }
