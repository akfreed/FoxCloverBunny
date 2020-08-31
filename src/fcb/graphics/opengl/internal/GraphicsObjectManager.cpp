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

#include "ModelBunny.h"
#include "ModelClover.h"
#include "ModelFox.h"
#include "core/Bunny.h"
#include "core/Clover.h"
#include "core/Fox.h"
#include "util/Util.h"

#include <vector>

using namespace fcb::core;

namespace fcb { namespace graphics {

namespace {

// Used with std::visit.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;  // Not needed after C++20.

}  // Anonymous namespace.


//! Saves a weak reference to a game object.
//! @param[in] gameObject A weak pointer to a Fox, Clover, or Bunny.
//! @return True if successful, false if the object is already registered.
bool GraphicsObjectManager::RegisterObject(GameObjectPointer gameObject)
{
    auto result = std::visit(overloaded{
        [this](std::weak_ptr<Clover const>&& clover) { return m_models.insert(std::make_unique<ModelClover>(std::move(clover))); },
        [this](std::weak_ptr<Bunny const>&&  bunny)  { return m_models.insert(std::make_unique<ModelBunny> (std::move(bunny)));  },
        [this](std::weak_ptr<Fox const>&&    fox)    { return m_models.insert(std::make_unique<ModelFox>   (std::move(fox)));    },
    }, std::move(gameObject));

    return result.second;
}

//! Removes the saved weak reference to a game object.
//! @param[in] gameObject A weak pointer to a Fox, Clover, or Bunny.
//! @return True if successful, false if the object was not already registered.
bool GraphicsObjectManager::UnregisterObject(GameObjectPointer const& gameObject)
{
    return std::visit([this](auto const& obj) {
        auto const iter = m_models.find(obj);
        if (iter == m_models.cend())
            return false;
        m_models.erase(iter);
        return true;
    }, gameObject);
}

//! Should only be called when OpenGL is ready to draw.
//! Draws all the models.
//! Removes the models from the collection if their associated game objects have been destroyed.
void GraphicsObjectManager::draw()
{
    // A vector to hold const_iterators of another collection.
    std::vector<typename decltype(m_models)::const_iterator> invalidIndexes;

    for (auto iter = m_models.cbegin(); iter != m_models.cend(); ++iter)
    {
        bool const result = (*iter)->DrawIfValid();
        if (!result)
            invalidIndexes.push_back(iter);
    }

    // Remove invalidated weak pointers.
    auto const startSize = m_models.size();
    UNUSED(startSize);
    for (auto const& iter : invalidIndexes)
        m_models.erase(iter);
    // Make sure all elements were removed.
    assert(startSize - invalidIndexes.size() == m_models.size());
}


} }
