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

#include "GuiContext.h"

#include <FL/Fl.H>

#include <cassert>

namespace fcb { namespace gui {

namespace {

//! An empty callback.
void callback_noop(void*)
{
}

}  // Anonymous namespace.

GuiContext::GuiContext()
    : m_mainWindow(400, 50, 600, 600)
{
    // Add a callback that will be called every time Fl::wait() is called.
    // This is just an empty callback, which causes Fl::wait() to be non-blocking.
    Fl::add_idle(callback_noop, &m_mainWindow);

    m_mainWindow.show();
}

GuiContext::~GuiContext()
{
    Fl::remove_idle(callback_noop, &m_mainWindow);
}

GuiContext& GuiContext::Get()
{
    static GuiContext s_instance;
    return s_instance;
}

//! Trigger the window to redraw and handle input events.
void GuiContext::Draw()
{
    m_mainWindow.redraw();
    Fl::wait();
}

//! Trigger the window to handle input events.
//! The drawing function already handles events, so this only needs to be called if not drawing.
void GuiContext::HandleEvents()
{
    Fl::wait();
}

//! @return A reference to the main window.
MainWindow& GuiContext::GetMainWindow()
{
    return m_mainWindow;
}


} }
