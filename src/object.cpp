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
#include <exdir/object.hpp>

namespace exdir {

Object::Object(std::filesystem::path i_path) : attrs(), type_(Type::Raw), path_(i_path), name_(), exdir_info() {
  // Check if exdir.yaml exists, if so, load into exdir_info
  if (std::filesystem::exists(path_ / "exdir.yaml")) {
    exdir_info = YAML::LoadFile((path_ / "exdir.yaml").string());

    // Set data type from exdir.yaml
    if (exdir_info["exdir"] && exdir_info["exdir"]["type"]) {
      if (exdir_info["exdir"]["type"].as<std::string>() == "file")
        type_ = Type::File;
      else if (exdir_info["exdir"]["type"].as<std::string>() == "group")
        type_ = Type::Group;
      else if (exdir_info["exdir"]["type"].as<std::string>() == "dataset")
        type_ = Type::Dataset;
      else
        type_ = Type::Raw;
    } else {
      // throw error, bad exdir.yaml
      std::string mssg = path_.string() + " exdir.yaml is invalid.";
      throw std::runtime_error(mssg);
    }
  } else {
    // If not exdir.yaml, must be a raw
    type_ = Type::Raw;
  }

  // Set name from path
  name_ = path_.filename().string();

  // Check if attributes.yaml exists. If so, load into attributes
  if (std::filesystem::exists(path_ / "attributes.yaml")) {
    attrs = YAML::LoadFile((path_ / "attributes.yaml").string());
  }
}

void Object::write() {
  // Write attributes to file
  if (!attrs.IsNull()) {
    std::ofstream attributes_yaml(path_ / "attributes.yaml");
    attributes_yaml << attrs;
    attributes_yaml.close();
  }
}
};  // namespace exdir
