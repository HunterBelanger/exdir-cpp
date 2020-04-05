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

  ~Object() = default;

  // Returns true if the object is a file.
  bool is_file() const;

  // Returns true if the object is a group or a file.
  bool is_group() const;

  // Returns true if the object is a dataset.
  bool is_dataset() const;

  // Returns true if the object is a raw.
  bool is_raw() const;

  // Returnes true if the object has attributes already defined.
  bool has_attributes() const;

  // Returns the name of the object as a string.
  std::string name() const;

  // Returns the std::filesystem::path to the object from the root.
  std::filesystem::path path() const;

  // Returns true if the two objects have the same path.
  bool operator==(const Object& obj) const;

  // Writes the attribues of object to disk.
  void write();

  YAML::Node attributes;

 protected:
  Object(std::filesystem::path i_path);
  Type type_;
  std::filesystem::path path_;
  // Exidr info stored in a yaml node
  YAML::Node exdir_info;

};  // object
};  // namespace exdir

#endif  // EXDIR_OBJECT_H