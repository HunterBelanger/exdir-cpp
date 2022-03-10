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
#ifndef EXDIR_OBJECT_H
#define EXDIR_OBJECT_H

#include <yaml-cpp/yaml.h>

#include <exception>
#include <filesystem>
#include <fstream>
#include <string>

namespace exdir {

class Object {
 public:
  enum Type { File, Group, Dataset, Raw };

  virtual ~Object() {write();}

  // Returns true if the object is a file.
  bool is_file() const {
    return type_ == Type::File ? true : false;
  }

  // Returns true if the object is a group or a file.
  bool is_group() const {
    return (type_ == Type::Group) || (type_ == Type::File) ? true : false;
  }

  // Returns true if the object is a dataset.
  bool is_dataset() const {
    return type_ == Type::Dataset ? true : false; 
  }

  // Returns true if the object is a raw.
  bool is_raw() const {
    return type_ == Type::Raw ? true : false; 
  }

  // Returnes true if the object has attributes already defined.
  bool has_attributes() const {
    return !attrs.IsNull(); 
  }

  // Returns the name of the object as a string.
  const std::string& name() const { return name_; }

  // Returns the std::filesystem::path to the object from the root.
  const std::filesystem::path& path() const { return path_; }

  // Returns true if the two objects have the same path.
  bool operator==(const Object& obj) const {
    return (path_.relative_path() == obj.path().relative_path());
  }

  // Writes the attribues of object to disk.
  // Must be virtual so that Dataset can overload it for
  // writing the .npy data file
  virtual void write();

  // YAML Node with attributes for object.
  YAML::Node attrs;

 protected:
  Object(std::filesystem::path i_path);
  Type type_;
  std::filesystem::path path_;
  std::string name_;
  // Exidr info stored in a yaml node
  YAML::Node exdir_info;

};  // object
};  // namespace exdir

#endif  // EXDIR_OBJECT_H
