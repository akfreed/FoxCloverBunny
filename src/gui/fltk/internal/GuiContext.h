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

#include "MainWindow.h"

namespace fcb { namespace gui {

//! Manages the GUI.
//! Singleton class.
class GuiContext
{
public:
    //! @return The singleton instance.
    static GuiContext& Get();

    GuiContext(GuiContext const&)            = delete;
    GuiContext(GuiContext&&)                 = delete;
    GuiContext& operator=(GuiContext const&) = delete;
    GuiContext& operator=(GuiContext&&)      = delete;
    ~GuiContext();

    void Draw();
    void HandleEvents();

    MainWindow& GetMainWindow();

private:
    GuiContext();

    MainWindow m_mainWindow;
};

} }
