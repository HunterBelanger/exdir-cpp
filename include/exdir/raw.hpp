#ifndef EXDIR_RAW_H
#define EXDIR_RAW_H

#include<exdir/object.hpp>

namespace exdir {

  class Raw : public Object {
    public:
      Raw(std::filesystem::path i_path);
      ~Raw() = default;

      // A Raw cannot have any daughter nodes, only member files,
      // therefore a list of the members is provided. They are not
      // Probed in any way howver.
      std::vector<std::string> member_files() const;

  }; // Raw

}; // exdir
#endif // EXDIR_RAW_H