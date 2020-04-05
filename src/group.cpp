#include<exdir/group.hpp>

namespace exdir {

  Group::Group(std::filesystem::path i_path): Object{i_path} {
    // use is_group() and is_file() to make sure group object was loaded.
    if(!is_group()) {
      std::string mssg = path_.string() + " does not contain a Group object.";
      throw std::runtime_error(mssg);
    }

    // Look at all members in file, check if folder
    for(auto& f : std::filesystem::directory_iterator(path_)) {
      if(std::filesystem::is_directory(f.status())) {
        // Is a directory, check if exdir.yaml exists
        if(std::filesystem::exists(f.path()/"exdir.yaml")) {
          YAML::Node daughter_node = YAML::LoadFile(f.path()/"exdir.yaml");

          if(daughter_node["exdir"] and daughter_node["exdir"]["type"]) {
            // f.path() yields /dir/to/parent/current_node/daughter_node
            // Using f.path().filename() yields daughter_node as path
            std::string node_name = f.path().filename().string();

            if(daughter_node["exdir"]["type"].as<std::string>() == "group") groups_.push_back(node_name);
            else if(daughter_node["exdir"]["type"].as<std::string>() == "dataset") datasets_.push_back(node_name);
            else if(daughter_node["exdir"]["type"].as<std::string>() == "raw") raws_.push_back(node_name);
            else {
              // throw error, unknown type
              std::string mssg = f.path().string() + " has an undefined type.";
              throw std::runtime_error(mssg);
            }

          } else {
            // throw error, bad exdir.yaml
            std::string mssg = f.path().string() + " exdir.yaml file is invalid.";
            throw std::runtime_error(mssg);
          }
        } else {
          // No exdir.yaml, must be a raw, save to raw vector
          raws_.push_back(f.path().filename().string());
        }
      }
    }
  }

  void Group::create_group(std::string name) {
    // Make sure directory does not yet exists
    if(!std::filesystem::exists(path_/name)) {
      // Make directory
      std::filesystem::create_directory(path_/name);

      // Make exdir.yaml file for directory
      std::ofstream exdir_yaml(path_/name/"exdir.yaml");
      exdir_yaml << "exdir:\n";
      // TODO put version in a header eventuall
      exdir_yaml << "  version: " << 1 << "\n";
      exdir_yaml << "  type: \"group\"";
      exdir_yaml.close();

      // Add raw name to raws_ for latter
      groups_.push_back(name);

    } else {
      std::string mssg = "The directory " + name + " already exists in " + path_.string();
      throw std::runtime_error(mssg);
    }
  }

  void Group::create_raw(std::string name) {
    // Make sure directory does not yet exists
    if(!std::filesystem::exists(path_/name)) {
      // Make directory
      std::filesystem::create_directory(path_/name);

      // Make exdir.yaml file for directory
      std::ofstream exdir_yaml(path_/name/"exdir.yaml");
      exdir_yaml << "exdir:\n";
      // TODO put version in a header eventuall
      exdir_yaml << "  version: " << 1 << "\n";
      exdir_yaml << "  type: \"raw\"";
      exdir_yaml.close();

      // Add raw name to raws_ for latter
      raws_.push_back(name);

    } else {
      std::string mssg = "The directory " + name + " already exists in " + path_.string();
      throw std::runtime_error(mssg);
    }
  }

  Group Group::get_group(std::string name) const {
    // Make sure in groups_
    for(const auto& group : groups_) {
      if(name == group) {
        return Group(path_/name);
      }
    }
    // throw error, wasn't a valid group
    std::string mssg = "The group " + name + " is not a member of "+this->name()+".";
    throw std::runtime_error(mssg);
  }

  Dataset Group::get_dataset(std::string name) const {
    // Make sure in datasets_
    for(const auto& dset : datasets_) {
      if(name == dset) {
        return exdir::Dataset(path_/name);
      }
    }
    // throw error, wasn't a valid Dataset
    std::string mssg = "The Dataset " + name + " is not a member of this Group.";
    throw std::runtime_error(mssg);
  }

  Raw Group::get_raw(std::string name) const {
    // Make sure in raws_
    for(const auto& raw : raws_) {
      if(name == raw) {
        return exdir::Raw(path_/name);
      }
    }
    // throw error, wasn't valid Raw
    std::string mssg = "The raw " + name + " is not a member of "+this->name()+".";
    throw std::runtime_error(mssg);
  }

  std::vector<std::string> Group::member_groups() const {
    return groups_;
  }

  std::vector<std::string> Group::member_datasets() const {
    return datasets_;
  }

  std::vector<std::string> Group::member_raws() const {
    return raws_;
  }

}; // exdir