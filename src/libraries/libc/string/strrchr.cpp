/** \file strrchr.cpp
  *
  * This file is part of the C standard library for the jsix operating
  * system.
  *
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  */

#include <string.h>
#include <__j6libc/casts.h>

using namespace __j6libc;

char *strrchr(const char *s, int c) {
    if (!s) return nullptr;

    char const *p = nullptr;
    while(*s) {
        if (*s == c)
            p = s;
        s++;
    }

    return cast_to<char*>(p);
}
