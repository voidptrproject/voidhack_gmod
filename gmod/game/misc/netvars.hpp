#pragma once
#include <stdint.h>

#include "recv_props.hpp"
#include "c_client_class.hpp"
#include "fnv.hpp"

#define netvar(table, prop, func_name, type) \
	type& func_name( ) { \
      static uintptr_t offset = 0; \
      if(!offset) \
      { offset = netvar_manager::get_net_var(fnv::hash( table ), fnv::hash( prop ) ); } \
	  \
      return *reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
    }

#define netvar_ptr(table, prop, func_name, type) \
	type* func_name( ) { \
      static uintptr_t offset = 0; \
      if(!offset) \
      { offset = netvar_manager::get_net_var(fnv::hash( table ), fnv::hash( prop ) ); } \
	  \
      return reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
    }

#define offset(type, var, offset) \
	type& var() { \
		return *(type*)(uintptr_t(this) + offset); \
	} \

namespace netvar_manager {
    uintptr_t get_net_var(uint32_t table, uint32_t prop);
    void init_netvar_manager();
}