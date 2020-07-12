#ifndef EXDIR_GROUP_H
#define EXDIR_GROUP_H

#include "dataset.hpp"
#include "raw.hpp"
#include "object.hpp"

namespace exdir {

class Group : public Object {
 public:
  Group(std::filesystem::path i_path);
  ~Group() = default;

  // Create a new group within the current group called <name>
  Group create_group(std::string name);

  // Create a new raw within the current group called <name>
  exdir::Raw create_raw(std::string name);

  // Create a new dataset within the current group called name,
  // and with type T.
  template <class T>
  exdir::Dataset<T> create_dataset(std::string name, const exdir::NDArray<T>& data) {
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
  Group get_group(std::string name) const;

  // Retrieve the groupe called <name> from current group
  exdir::Raw get_raw(std::string name) const;

  // Retrieve the groupe called <name> from current group
  template<class T>
  exdir::Dataset<T> get_dataset(std::string name) const {
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
  std::vector<std::string> member_groups() const;

  // Get vector of keys for member dataset
  std::vector<std::string> member_datasets() const;

  // Get vector of keys for member raws
  std::vector<std::string> member_raws() const;

 protected:
  std::vector<std::string> groups_;
  std::vector<std::string> raws_;
  std::vector<std::string> datasets_;

};  // Group

};      // namespace exdir
#endif  // EXDIR_GROUP_H
