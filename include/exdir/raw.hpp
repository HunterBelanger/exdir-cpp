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
#ifndef EXDIR_RAW_H
#define EXDIR_RAW_H

#include "object.hpp"

namespace exdir {

class Raw : public Object {
 public:
  Raw(std::filesystem::path i_path);
  ~Raw() = default;

  // A Raw cannot have any daughter nodes, only member files,
  // therefore a list of the members is provided. They are not
  // probed in any way however.
  std::vector<std::string> member_files() const;

};  // Raw

};      // namespace exdir
#endif  // EXDIR_RAW_H
