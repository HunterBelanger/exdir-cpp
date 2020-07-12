#ifndef EXDIR_NDARRAY_H
#define EXDIR_NDARRAY_H

#include <algorithm>
#include <array>
#include <complex>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

// Macro to force function to be inlined. This is done for speed and to try and
// force the compiler to vectorize operatrions.
#if defined(_MSC_VER)
#define NDARRAY_INLINE inline __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define NDARRAY_INLINE inline __attribute__((always_inline))
#else
#define NDARRAY_INLINE inline
#endif

namespace exdir {
//==============================================================================
// Template Class NDArray
template <class T>
class NDArray {
 public:
  //==========================================================================
  // Constructors and Destructors
  NDArray();
  NDArray(std::vector<size_t> init_shape, bool c_continuous = true);
  NDArray(std::vector<T> data, std::vector<size_t> init_shape,
          bool c_continuous = true);
  ~NDArray() = default;

  // Static load function
  static NDArray load(std::string fname);

  //==========================================================================
  // Indexing

  // Indexing operators for indexing with vector
  T& operator()(const std::vector<size_t>& indices);
  const T& operator()(const std::vector<size_t>& indices) const;

  // Variadic indexing operators
  // Access data with array idicies.
  template <typename... INDS>
  T& operator()(INDS... inds);
  template <typename... INDS>
  const T& operator()(INDS... inds) const;

  // Linear Indexing operators
  T& operator[](size_t i);
  const T& operator[](size_t i) const;

  //==========================================================================
  // Constant Methods

  // Return vector describing shape of array
  std::vector<size_t> shape() const;

  // Return number of elements in array
  size_t size() const;

  size_t linear_index(const std::vector<size_t>& indices) const;

  template <typename... INDS>
  size_t linear_index(INDS... inds) const;

  // Returns true if data is stored as c continuous (row-major order),
  // and false if fortran continuous (column-major order)
  bool c_continuous() const;

  // Save array to the file fname.npy
  void save(std::string fname) const;

  //==========================================================================
  // Non-Constant Methods

  // Fills entire array with the value provided
  void fill(T val);

  // Will reshape the array to the given dimensions
  void reshape(std::vector<size_t> new_shape);

  // Realocates array to fit the new size.
  // DATA CAN BE LOST IF ARRAY IS SHRUNK
  void reallocate(std::vector<size_t> new_shape);

  //==========================================================================
  // Operators for Any Type (Same or Different)
  template <class C>
  NDArray& operator+=(const NDArray<C>& a);
  template <class C>
  NDArray& operator-=(const NDArray<C>& a);
  template <class C>
  NDArray& operator*=(const NDArray<C>& a);
  template <class C>
  NDArray& operator/=(const NDArray<C>& a);

  //==========================================================================
  // Operators for Constants
  template <class C>
  NDArray& operator+=(C c);
  template <class C>
  NDArray& operator-=(C c);
  template <class C>
  NDArray& operator*=(C c);
  template <class C>
  NDArray& operator/=(C c);

  //==========================================================================
  // Conversion Operator
  template <class C>
  operator NDArray<C>() const;

 private:
  std::vector<T> data_;
  std::vector<size_t> shape_;
  bool c_continuous_;
  size_t dimensions_;

  template <class C>
  friend class NDArray;

  size_t c_continuous_index(const std::vector<size_t>& indices) const;

  size_t fortran_continuous_index(const std::vector<size_t>& indices) const;

  template <size_t D>
  inline size_t c_continuous_index(const std::array<size_t, D>& indices) const;

  template <size_t D>
  inline size_t fortran_continuous_index(
      const std::array<size_t, D>& indices) const;
};

//==============================================================================
// NDArray Implementation
template <class T>
NDARRAY_INLINE T& NDArray<T>::operator()(const std::vector<size_t>& indices) {
  size_t indx;
  if (c_continuous_) {
    // Get linear index for row-major order
    indx = c_continuous_index(indices);
  } else {
    // Get linear index for column-major order
    indx = fortran_continuous_index(indices);
  }
  return data_[indx];
}

template <class T>
NDARRAY_INLINE const T& NDArray<T>::operator()(
    const std::vector<size_t>& indices) const {
  size_t indx;
  if (c_continuous_) {
    // Get linear index for row-major order
    indx = c_continuous_index(indices);
  } else {
    // Get linear index for column-major order
    indx = fortran_continuous_index(indices);
  }
  return data_[indx];
}

template <class T>
template <typename... INDS>
NDARRAY_INLINE T& NDArray<T>::operator()(INDS... inds) {
  std::array<size_t, sizeof...(inds)> indices{static_cast<size_t>(inds)...};

  size_t indx;
  if (c_continuous_) {
    // Get linear index for row-major order
    indx = c_continuous_index(indices);
  } else {
    // Get linear index for column-major order
    indx = fortran_continuous_index(indices);
  }
  return data_[indx];
}

template <class T>
template <typename... INDS>
NDARRAY_INLINE const T& NDArray<T>::operator()(INDS... inds) const {
  std::array<size_t, sizeof...(inds)> indices{static_cast<size_t>(inds)...};

  size_t indx;
  if (c_continuous_) {
    // Get linear index for row-major order
    indx = c_continuous_index(indices);
  } else {
    // Get linear index for column-major order
    indx = fortran_continuous_index(indices);
  }
  return data_[indx];
}

template <class T>
NDARRAY_INLINE T& NDArray<T>::operator[](size_t i) {
  return data_[i];
}

template <class T>
NDARRAY_INLINE const T& NDArray<T>::operator[](size_t i) const {
  return data_[i];
}

template <class T>
NDARRAY_INLINE std::vector<size_t> NDArray<T>::shape() const {
  return shape_;
}

template <class T>
NDARRAY_INLINE size_t NDArray<T>::size() const {
  return data_.size();
}

template <class T>
NDARRAY_INLINE size_t
NDArray<T>::linear_index(const std::vector<size_t>& indices) const {
  if (c_continuous_) {
    // Get linear index for row-major order
    return c_continuous_index(indices);
  } else {
    // Get linear index for column-major order
    return fortran_continuous_index(indices);
  }
}

template <class T>
template <typename... INDS>
NDARRAY_INLINE size_t NDArray<T>::linear_index(INDS... inds) const {
  std::array<size_t, sizeof...(inds)> indices{static_cast<size_t>(inds)...};

  if (c_continuous_) {
    // Get linear index for row-major order
    return c_continuous_index(indices);
  } else {
    // Get linear index for column-major order
    return fortran_continuous_index(indices);
  }
}

template <class T>
NDARRAY_INLINE bool NDArray<T>::c_continuous() const {
  return c_continuous_;
}

template <class T>
NDARRAY_INLINE size_t
NDArray<T>::c_continuous_index(const std::vector<size_t>& indices) const {
  // Make sure proper number of indices
  if (indices.size() != dimensions_) {
    std::string mssg = "Improper number of indicies provided to NDArray.";
    throw std::runtime_error(mssg);
  }

  size_t indx = indices[dimensions_ - 1];
  if (indx >= shape_[dimensions_ - 1]) {
    std::string mssg = "Index provided to NDArray out of range.";
    throw std::out_of_range(mssg);
  }

  size_t coeff = 1;

  for (size_t i = dimensions_ - 1; i > 0; i--) {
    if (indices[i] >= shape_[i]) {
      std::string mssg = "Index provided to NDArray out of range.";
      throw std::out_of_range(mssg);
    }

    coeff *= shape_[i];
    indx += coeff * indices[i - 1];
  }

  return indx;
}

template <class T>
NDARRAY_INLINE size_t
NDArray<T>::fortran_continuous_index(const std::vector<size_t>& indices) const {
  // Make sure proper number of indices
  if (indices.size() != dimensions_) {
    std::string mssg = "Improper number of indicies provided to NDArray.";
    throw std::runtime_error(mssg);
  }

  size_t indx = indices[0];
  if (indx >= shape_[0]) {
    std::string mssg = "Index provided to NDArray out of range.";
    throw std::out_of_range(mssg);
  }
  size_t coeff = 1;

  for (size_t i = 0; i < dimensions_ - 1; i++) {
    if (indices[i] >= shape_[i]) {
      std::string mssg = "Index provided to NDArray out of range.";
      throw std::out_of_range(mssg);
    }

    coeff *= shape_[i];
    indx += coeff * indices[i + 1];
  }

  return indx;
}

template <class T>
template <size_t D>
NDARRAY_INLINE size_t
NDArray<T>::c_continuous_index(const std::array<size_t, D>& indices) const {
  // Make sure proper number of indices
  if (indices.size() != dimensions_) {
    std::string mssg = "Improper number of indicies provided to NDArray.";
    throw std::runtime_error(mssg);
  }

  size_t indx = indices[dimensions_ - 1];
  if (indx >= shape_[dimensions_ - 1]) {
    std::string mssg = "Index provided to NDArray out of range.";
    throw std::out_of_range(mssg);
  }

  size_t coeff = 1;

  for (size_t i = dimensions_ - 1; i > 0; i--) {
    if (indices[i] >= shape_[i]) {
      std::string mssg = "Index provided to NDArray out of range.";
      throw std::out_of_range(mssg);
    }

    coeff *= shape_[i];
    indx += coeff * indices[i - 1];
  }

  return indx;
}

template <class T>
template <size_t D>
NDARRAY_INLINE size_t NDArray<T>::fortran_continuous_index(
    const std::array<size_t, D>& indices) const {
  // Make sure proper number of indices
  if (indices.size() != dimensions_) {
    std::string mssg = "Improper number of indicies provided to NDArray.";
    throw std::runtime_error(mssg);
  }

  size_t indx = indices[0];
  if (indx >= shape_[0]) {
    std::string mssg = "Index provided to NDArray out of range.";
    throw std::out_of_range(mssg);
  }
  size_t coeff = 1;

  for (size_t i = 0; i < dimensions_ - 1; i++) {
    if (indices[i] >= shape_[i]) {
      std::string mssg = "Index provided to NDArray out of range.";
      throw std::out_of_range(mssg);
    }

    coeff *= shape_[i];
    indx += coeff * indices[i + 1];
  }

  return indx;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator+=(const NDArray<C>& a) {
  // Ensure same number of dimensions
  if (dimensions_ != a.dimensions_) {
    std::string mssg = "Cannot add two NDArrays with different dimensions.";
    throw std::runtime_error(mssg);
  }

  // Ensure same shape
  for (size_t i = 0; i < dimensions_; i++) {
    if (shape_[i] != a.shape_[i]) {
      std::string mssg = "Cannot add two NDArrays with different shapes";
      throw std::runtime_error(mssg);
    }
  }

  // Do addition
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] += a.data_[i];
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator-=(const NDArray<C>& a) {
  // Ensure same number of dimensions
  if (dimensions_ != a.dimensions_) {
    std::string mssg =
        "Cannot subtract two NDArrays with different dimensions.";
    throw std::runtime_error(mssg);
  }

  // Ensure same shape
  for (size_t i = 0; i < dimensions_; i++) {
    if (shape_[i] != a.shape_[i]) {
      std::string mssg = "Cannot subtract two NDArrays with different shapes";
      throw std::runtime_error(mssg);
    }
  }

  // Do subtraction
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] -= a.data_[i];
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator*=(const NDArray<C>& a) {
  // Ensure same number of dimensions
  if (dimensions_ != a.dimensions_) {
    std::string mssg =
        "Cannot multiply two NDArrays with different dimensions.";
    throw std::runtime_error(mssg);
  }

  // Ensure same shape
  for (size_t i = 0; i < dimensions_; i++) {
    if (shape_[i] != a.shape_[i]) {
      std::string mssg = "Cannot multiply two NDArrays with different shapes";
      throw std::runtime_error(mssg);
    }
  }

  // Do multiplication
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] *= a.data_[i];
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator/=(const NDArray<C>& a) {
  // Ensure same number of dimensions
  if (dimensions_ != a.dimensions_) {
    std::string mssg = "Cannot divide two NDArrays with different dimensions.";
    throw std::runtime_error(mssg);
  }

  // Ensure same shape
  for (size_t i = 0; i < dimensions_; i++) {
    if (shape_[i] != a.shape_[i]) {
      std::string mssg = "Cannot divide two NDArrays with different shapes";
      throw std::runtime_error(mssg);
    }
  }

  // Do division
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] /= a.data_[i];
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator+=(C c) {
  // Do addition
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] += c;
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator-=(C c) {
  // Do subtraction
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] -= c.data_[i];
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator*=(C c) {
  // Do multiplication
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] *= c;
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>& NDArray<T>::operator/=(C c) {
  // Do division
  for (size_t i = 0; i < data_.size(); i++) {
    data_[i] /= c;
  }

  return *this;
}

template <class T>
template <class C>
NDArray<T>::operator NDArray<C>() const {
  // Make array to be returned of same shape
  NDArray<C> new_array(shape_);

  // Go through all elements
  for (size_t i = 0; i < data_.size(); i++) {
    new_array[i] = data_[i];
  }

  return new_array;
}

}; // namespace exdir
#endif  // EXDIR_NDARRAY_H
