#ifndef EXDIR_FILE_H
#define EXDIR_FILE_H

#include "group.hpp"

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
