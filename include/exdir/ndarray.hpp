/*
 * exdir-cpp
 *
 * Copyright (C) 2020, Hunter Belanger (hunter.belanger@gmail.com)
 * All rights reserved.
 *
 * Released under the terms and conditions of the BSD 3-Clause license.
 * For more information, refer to the GitHub repo for this library at:
 * https://github.com/HunterBelanger/exdir-cpp
 *
 * */
#ifndef EXDIR_NDARRAY_H
#define EXDIR_NDARRAY_H

#include <ndarray.hpp>

namespace exdir{

template<class T>
using NDArray = ::NDArray<T>;

} // namespace exdir

#endif  // EXDIR_NDARRAY_H
