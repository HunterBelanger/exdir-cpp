#ifndef EXDIR_ARRAY_H
#define EXDIR_ARRAY_H

#include <exception>
#include <exdir/npy.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace exdir {

template <class T>
class Array {
 public:
  Array(T* data_pntr, std::vector<size_t> i_shape, DType i_dytpe,
        bool c_cont = true);
  Array(std::vector<T> i_data, std::vector<size_t> i_shape, DType i_dtype,
        bool c_cont = true);
  ~Array() = default;

  // Access data with linear index.
  T& operator[](size_t i);

  // Access data with array indicies passed as a vector.
  T& operator()(std::vector<size_t> indicies);

  // Access data with array idicies.
  template <typename IND, typename... INDS>
  T& operator()(IND ind0, INDS... inds) {
    std::vector<size_t> indicies{static_cast<size_t>(ind0),
                                 static_cast<size_t>(inds)...};
    if (indicies.size() != shape_.size()) {
      std::string mssg =
          "Improper number of indicies provided to exdir::Array.";
      throw std::runtime_error(mssg);
    } else {
      size_t indx;
      if (c_contiguous_) {
        // Get linear index for row-major order
        indx = c_contiguous_index(indicies);
      } else {
        // Get linear index for column-major order
        indx = fortran_contiguous_index(indicies);
      }
      return data_[indx];
    }
  }

  // Returns a vector describing the shape of the array.
  std::vector<size_t> shape() const;

  // Reshapes the array to the prescribed shape, provided as a vector of
  // size_t type values. If the reshape is possible, the function returns
  // true, and the shape is modified. If the provided shape is not compatible
  // with the current data, the shape is not changed and false is returned.
  bool reshape(std::vector<size_t> new_shape);

  // Returns the number of elements in the array.
  size_t size() const;

  // Returns raw pointer to data in vector.
  const T* data_pointer() const;

  // Returns true if data is C contiguous.
  bool c_contiguous() const;

  // Returns the DType of the data.
  DType dtype() const;

 private:
  std::vector<size_t> shape_;
  std::vector<T> data_;
  DType dtype_;
  size_t n_elements_;
  bool c_contiguous_;

  size_t c_contiguous_index(std::vector<size_t>& indicies);
  size_t fortran_contiguous_index(std::vector<size_t>& indicies);

};  // Array

};      // namespace exdir
#endif  // EXDIR_ARRAY_H