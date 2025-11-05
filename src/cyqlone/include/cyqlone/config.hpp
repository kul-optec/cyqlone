#pragma once

#include <batmat/config.hpp>

#ifndef CYQLONE_NAMESPACE
#if defined(__AVX512F__)
#define CYQLONE_NAMESPACE cyqlone::inline avx512
#elif defined(__AVX2__)
#define CYQLONE_NAMESPACE cyqlone::inline avx2
#else
#define CYQLONE_NAMESPACE cyqlone
#endif
#endif

namespace CYQLONE_NAMESPACE {
using batmat::index_t;
using batmat::real_t;
} // namespace CYQLONE_NAMESPACE
