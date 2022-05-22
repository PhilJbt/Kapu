// Os
#include <windows.h>
#include <PowrProf.h>

#pragma comment(lib, "PowrProf.lib")

// C++
#include <thread>
#include <chrono>
#include <iostream>

// Lib
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/textbox.hpp>

// Kapu
#include "threadsmanager.h"

#define THREADSMANAGER ThreadsManager::instanceGet()