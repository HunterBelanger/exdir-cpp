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
#ifndef EXDIR_GROUP_H
#define EXDIR_GROUP_H

#include <exdir/dataset.hpp>
#include <exdir/raw.hpp>
#include <exdir/object.hpp>

namespace exdir {

class Group : public Object {
 public:
  ~Group() = default;

  // Create a new group within the current group called <name>
  Group create_group(const std::string& name);

  // Create a new raw within the current group called <name>
  exdir::Raw create_raw(const std::string& name);

  // Create a new dataset within the current group called name,
  // and with type T.
  template <class T>
  exdir::Dataset<T> create_dataset(const std::string& name, const exdir::NDArray<T>& data) {
    // Make sure directory does not yet exists
    if (!std::filesystem::exists(path_ / name)) {
      // Make directory
      std::filesystem::create_directory(path_ / name);

      // Make exdir.yaml file for directory
      std::ofstream exdir_yaml(path_ / name / "exdir.yaml");
      exdir_yaml << "exdir:\n";
      // TODO put version in a header eventuall
      exdir_yaml << "  version: " << 1 << "\n";
      exdir_yaml << "  type: \"dataset\"";
      exdir_yaml.close();

      // Add raw name to raws_ for latter
      datasets_.push_back(name);

      // Write data to data.npy
      data.save(path_ / name / "data.npy");

    } else {
      std::string mssg =
          "The directory " + name + " already exists in " + path_.string();
      throw std::runtime_error(mssg);
    }

    return get_dataset<T>(name);
  }

  // Retrieve the groupe called <name> from current group
  Group get_group(const std::string& name) const;

  // Retrieve the groupe called <name> from current group
  exdir::Raw get_raw(const std::string& name) const;

  // Retrieve the groupe called <name> from current group
  template<class T>
  exdir::Dataset<T> get_dataset(const std::string& name) const {
    // Make sure in datasets_
    for (const auto& dset : datasets_) {
      if (name == dset) {
        return exdir::Dataset<T>(path_ / name);
      }
    }
    // throw error, wasn't a valid Dataset
    std::string mssg = "The Dataset " + name + " is not a member of this Group.";
    throw std::runtime_error(mssg);
  }


  // Get vector of keys for member groups
  const std::vector<std::string>& member_groups() const {return groups_;}

  // Get vector of keys for member dataset
  const std::vector<std::string>& member_datasets() const {return datasets_;}

  // Get vector of keys for member raws
  const std::vector<std::string>& member_raws() const {return raws_;}

 protected:
  // Constructor is private.
  // Only a File or another Group can create an new group.
  Group(std::filesystem::path i_path);

  std::vector<std::string> groups_;
  std::vector<std::string> raws_;
  std::vector<std::string> datasets_;
};  // Group

};      // namespace exdir
#endif  // EXDIR_GROUP_H
