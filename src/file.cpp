#include<exdir/file.hpp>

namespace exdir {

  File::File(std::filesystem::path i_path): Group{i_path} {
    // Use is_file() to make sure a File object was loaded.
    if(!is_file()) {
      std::string mssg = path_.string() + " does not contain a File object.";
      throw std::runtime_error(mssg);
    }
  }

  File create_file(std::filesystem::path name) {
    // Make sure directory does not yet exists
    if(!std::filesystem::exists(name)) {
      // Make directory
      std::filesystem::create_directory(name);

      // Make exdir.yaml file for directory
      std::ofstream exdir_yaml(name/"exdir.yaml");
      exdir_yaml << "exdir:\n";
      // TODO put version in a header eventuall
      exdir_yaml << "  version: " << 1 << "\n";
      exdir_yaml << "  type: \"file\"";
      exdir_yaml.close();

      // Return file
      return File(name);

    } else {
      std::string mssg = "The directory " + name.string() + " already exists.";
      throw std::runtime_error(mssg);
    }
  }

}; // exdir