#pragma once

#include "../../memory.hpp"

#define INITIALIZE_INTERFACE(varname, intrtype, mod, name) namespace interfaces { inline memory::interface_t<intrtype> varname (mod, name); }
#define INITIALIZE_INTERFACE_FROM_ADDRESS(varname, intrtype, addr) namespace interfaces { inline memory::interface_t<intrtype> varname (memory::address_t(addr)); }