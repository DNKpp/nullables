//           Copyright Dominic (DNKpp) Koepke 2025
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#ifndef GIMO_CONFIG_HPP
#define GIMO_CONFIG_HPP

#ifndef GIMO_ASSERT
    #include <cassert>
    #define GIMO_ASSERT(condition, msg, ...) assert((condition) && msg)
#endif

#endif
