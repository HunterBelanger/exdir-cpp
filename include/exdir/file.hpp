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
#ifndef EXDIR_FILE_H
#define EXDIR_FILE_H

#include <exdir/group.hpp>

namespace exdir {

class File : public Group {
 public:
  File(std::filesystem::path i_path);
  ~File() = default;

  // File has no special method compared to Group
};

//========================================================
// Non Member function to create a new Exdir file
File create_file(std::filesystem::path name);

};      // namespace exdir
#endif  // EXDIR_FILE_H
