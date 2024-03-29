/*
 * blogcxx :: https://www.blogcxx.de
 * Variant class detector.
 */

#pragma once

#if !defined(USE_VARIANT_API) || USE_VARIANT_API < 1 || USE_VARIANT_API > 2
#error "Variant API not defined"
#endif

#if USE_VARIANT_API == 1
#include <variant>
namespace var = std;
#endif

#if USE_VARIANT_API == 2
#include <boost/variant.hpp>
namespace var = boost;
#endif
