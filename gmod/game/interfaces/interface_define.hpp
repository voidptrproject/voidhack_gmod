#pragma once

#include "../../memory.hpp"

#define INITIALIZE_INTERFACE(varname, intrtype, mod, name) namespace interfaces { static inline memory::interface_t<intrtype> varname (mod, name); }