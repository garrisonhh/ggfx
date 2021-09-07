#ifndef GLAD_H
#define GLAD_H

#pragma GCC system_header

#include "glad_bindings.h"

#ifdef GLAD_IMPL
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "glad_bindings.c"
#pragma GCC diagnostic pop
#endif

#endif
