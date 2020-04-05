#include<exdir/raw.hpp>

namespace exdir {

  Raw::Raw(std::filesystem::path i_path): Object{i_path} {
    // use is_raw() to make sure a Raw object was loaded
    if(!is_raw()) {
      std::string mssg = path_.string() + " does not contian a Raw object.";
      throw std::runtime_error(mssg);
    }
  }

  std::vector<std::string> Raw::member_files() const {
    // Initialize empty vector
    std::vector<std::string> files;
    for(const auto& f : std::filesystem::directory_iterator(path_)) {
      files.push_back(f.path().filename().string());
    }
    return files;
  }
}; // exdir