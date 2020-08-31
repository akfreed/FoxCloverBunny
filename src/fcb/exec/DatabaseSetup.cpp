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

#include "DatabaseSetup.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

using fcb::database::Database;

namespace fcb { namespace exec {


//! Creates a new database file and sets up the table.
//! The database file has the current date and time in the name.
//! @return The newly created database or a default-initialized database on failure.
Database SetupNewDatabaseOnDisk()
{
    std::time_t sinceEpoch = std::time(nullptr);
    std::tm localTime;

    bool success = true;
    // This localtime_s might be MT-Unsafe.
#ifdef _MSC_VER
    // MSCV reverses the arguments (non-compliant).
    if (localtime_s(&localTime, &sinceEpoch) != 0)
        success = false;
#else
    if (!localtime_r(&sinceEpoch, &localTime))
        success = false;
#endif
    if (!success)
    {
        std::cout << "Failed to get local time." << std::endl;
        return {};
    }

    std::stringstream dirname;
    dirname << std::put_time(&localTime, "database_%F__%H-%M-%S.sqlite3");
    std::cout << "Creating database file: " << dirname.str() << std::endl;
    return Database::CreateNew(dirname.str());
}


} }
