#ifndef EXDIR_DATASET_H
#define EXDIR_DATASET_H

#include "ndarray.hpp"
#include "object.hpp"
#include "raw.hpp"

namespace exdir {

template<class T>
class Dataset : public Object {
 public:
  Dataset(std::filesystem::path i_path);
  ~Dataset();

  exdir::Raw create_raw(std::string name);

  exdir::Raw get_raw(std::string) const;
  std::vector<std::string> member_raws() const;

  void write() final;
  

  // NDArray containing data
  NDArray<T> data;
 private:
  std::vector<std::string> raws_;

};  // Dataset

};      // namespace exdir
#endif  // EXDIR_DATASET_H
