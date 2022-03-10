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

#include <exdir/ndarray.hpp>
#include <exdir/object.hpp>
#include <exdir/raw.hpp>

namespace exdir {

template<class T>
class Dataset : public Object {
 public:
  ~Dataset() = default;

  exdir::Raw create_raw(const std::string& name);

  exdir::Raw get_raw(const std::string& name) const;

  const std::vector<std::string>& member_raws() const {return raws_;}

  void write() override final;
  
  // NDArray containing data
  NDArray<T> data;
 private:
  // Constructor is private.
  // Only a Group can create a Dataset.
  friend class Group;
  Dataset(std::filesystem::path i_path);

  std::vector<std::string> raws_;

};  // Dataset

// Declaration of explicit instantiations.
extern template class Dataset<char>;
extern template class Dataset<unsigned char>;
extern template class Dataset<uint16_t>;
extern template class Dataset<uint32_t>;
extern template class Dataset<uint64_t>;
extern template class Dataset<int16_t>;
extern template class Dataset<int32_t>;
extern template class Dataset<int64_t>;
extern template class Dataset<float>;
extern template class Dataset<double>;
extern template class Dataset<std::complex<float>>;
extern template class Dataset<std::complex<double>>;

};      // namespace exdir
#endif  // EXDIR_DATASET_H
