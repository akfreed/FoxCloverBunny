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

#include "MainWindow.h"

#include "graphics/Draw.h"
#include "input/InputEvent.h"

#include <FL/gl.h>

namespace fcb { namespace gui {


//! Constructor.
//! @param[in] xPos   The initial x-position of the window.
//! @param[in] yPos   The initial y-position of the window.
//! @param[in] width  The width of the window.
//! @param[in] height The height of the window.
MainWindow::MainWindow(int const xPos, int const yPos, int const width, int const height)
    : Fl_Gl_Window(xPos, yPos, width, height, "Fox Clover Bunny")
{ }

//! Handle events.
//! Do not call directly. This function is called by the base class when it is time to handle events.
int MainWindow::handle(int event)
{
    int const result = fcb::input::ProcessFltkInputEvent(event);
    if (result != 0)
        return result;

    // Pass unhandled events to base class.
    return Fl_Gl_Window::handle(event);
}

//! Draw everything.
//! Do not call directly. This function is called by the base class with the OpenGL context enabled.
//! Don't call OpenGL code outside of this function.
void MainWindow::draw()
{
    if (!valid())
    {
        glViewport(0, 0, w(), h());

        fcb::graphics::Refresh();
    }

    // Call draw for everything.
    fcb::graphics::Draw();
}


} }
