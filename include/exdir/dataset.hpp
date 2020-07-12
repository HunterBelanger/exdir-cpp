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
