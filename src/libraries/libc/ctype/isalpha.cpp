/** \file isalpha.cpp
  *
  * This file is part of the C standard library for the jsix operating
  * system.
  *
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  */

#include <ctype.h>

// Currently only implemented for "C" locale.

int isalpha( int c ) {
    return isupper(c) || islower(c);
}
