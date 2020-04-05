#ifndef EXDIR_ARRAY_H
#define EXDIR_ARRAY_H

#include<exdir/npy.hpp>

#include<vector>
#include<string>
#include<exception>
#include<stdexcept>
#include<iostream>

namespace exdir {

  template<class T>
  class Array {
    public:
      Array(T* data_pntr, std::vector<size_t> i_shape, DType i_dytpe, bool c_cont=true);
      Array(std::vector<T> i_data, std::vector<size_t> i_shape, DType i_dtype, bool c_cont=true);
      ~Array() = default;

      // Access data with linear index.
      T& operator[](size_t i);

      // Access data with array indicies.
      T& operator()(std::vector<size_t> indicies);

      // Returns a vector describing the shape of the array.
      std::vector<size_t> shape() const;

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

  }; // Array

}; // exdir
#endif // EXDIR_ARRAY_H