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

namespace exdir{

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

namespace details {
//==============================================================================
// Declarations for NPY functions

// Enum of possible data types handeled by this implementation.
enum class DType {
  CHAR,
  UCHAR,
  INT16,
  INT32,
  INT64,
  UINT16,
  UINT32,
  UINT64,
  FLOAT32,
  DOUBLE64,
  COMPLEX64,
  COMPLEX128
};

// Function which opens file fname, and loads in the binary data into 1D
// array of chars, which must latter be type cast be the user. The char* to
// the data is returned as a reference, along with the number of elements,
// continuits, and element size in bytes. Returned is a vector for the shape.
void load_npy(std::string fname, char*& data_ptr, std::vector<size_t>& shape,
              DType& dtype, bool& c_contiguous);

// Function which writes binary data to a Numpy .npy file.
void write_npy(std::string fname, const char* data_ptr,
               std::vector<size_t> shape, DType dtype, bool c_contiguous);

// Returns the proper DType for a given Numpy dtype.descr string.
DType descr_to_DType(std::string dtype);

// Returns Numpy descr string for given DType
std::string DType_to_descr(DType dtype);

// Returns the number of bytes used to represent the provided DType.
size_t size_of_DType(DType dtype);

// Function to check wether or not host system is little-endian or not
bool system_is_little_endian();

// Function to switch bye order of data for an array which contains
// n_elements, each with element_size bytes. The size of data should
// therefore be n_elements*element_size; if not, this is undefined behavior.
void swap_bytes(char* data, uint64_t n_elements, size_t element_size);

// Swaps the first and second byte pointed to by char* bytes.
void swap_two_bytes(char* bytes);

// Swaps the first four bytes pointed to by char* bytes.
void swap_four_bytes(char* bytes);

// Swaps the first eight bytes pointed to by char* bytes.
void swap_eight_bytes(char* bytes);

// Swaps the first sixteen bytes pointed to by char* bytes.
void swap_sixteen_bytes(char* bytes);
} // namespace details

//==============================================================================
// NDArray Implementation
template <class T>
NDArray<T>::NDArray():data_(), shape_(), c_continuous_(true), dimensions_(0) {}

template <class T>
NDArray<T>::NDArray(std::vector<size_t> init_shape, bool c_continuous):data_(), shape_(), c_continuous_(), dimensions_() {
  if (init_shape.size() > 0) {
    shape_ = init_shape;
    dimensions_ = shape_.size();

    size_t ne = init_shape[0];
    for (size_t i = 1; i < dimensions_; i++) {
      ne *= init_shape[i];
    }

    data_.resize(ne);

    c_continuous_ = c_continuous;

  } else {
    std::string mssg = "NDArray shape vector must have at least one element.";
    throw std::runtime_error(mssg);
  }
}

template <class T>
NDArray<T>::NDArray(std::vector<T> data, std::vector<size_t> init_shape,
                    bool c_continuous):data_(), shape_(), c_continuous_(), dimensions_() {
  if (init_shape.size() > 0) {
    shape_ = init_shape;
    dimensions_ = shape_.size();

    size_t ne = init_shape[0];
    for (size_t i = 1; i < dimensions_; i++) {
      ne *= init_shape[i];
    }

    if (ne != data.size()) {
      std::string mssg =
          "Shape is incompatible with number of elements provided for NDArray.";
      throw std::runtime_error(mssg);
    }

    data_ = data;

    c_continuous_ = c_continuous;

  } else {
    std::string mssg =
        "Shape vector must have at least one element for NDArray.";
    throw std::runtime_error(mssg);
  }
}

template <class T>
NDArray<T> NDArray<T>::load(std::string fname) {
  // Get expected DType according to T
  details::DType expected_dtype;
  const char* T_type_name = typeid(T).name();

  if (T_type_name == typeid(char).name())
    expected_dtype = details::DType::CHAR;
  else if (T_type_name == typeid(unsigned char).name())
    expected_dtype = details::DType::UCHAR;
  else if (T_type_name == typeid(uint16_t).name())
    expected_dtype = details::DType::UINT16;
  else if (T_type_name == typeid(uint32_t).name())
    expected_dtype = details::DType::UINT32;
  else if (T_type_name == typeid(uint64_t).name())
    expected_dtype = details::DType::UINT64;
  else if (T_type_name == typeid(int16_t).name())
    expected_dtype = details::DType::INT16;
  else if (T_type_name == typeid(int32_t).name())
    expected_dtype = details::DType::INT32;
  else if (T_type_name == typeid(int64_t).name())
    expected_dtype = details::DType::INT64;
  else if (T_type_name == typeid(float).name())
    expected_dtype = details::DType::FLOAT32;
  else if (T_type_name == typeid(double).name())
    expected_dtype = details::DType::DOUBLE64;
  else if (T_type_name == typeid(std::complex<float>).name())
    expected_dtype = details::DType::COMPLEX64;
  else if (T_type_name == typeid(std::complex<double>).name())
    expected_dtype = details::DType::COMPLEX128;
  else {
    std::string mssg = "The datatype is not supported for NDArray.";
    throw std::runtime_error(mssg);
  }

  // Variables to send to npy function
  char* data_ptr;
  std::vector<T> data_vector;
  std::vector<size_t> data_shape;
  details::DType data_dtype;
  bool data_c_continuous;

  // Load data into variables
  load_npy(fname, data_ptr, data_shape, data_dtype, data_c_continuous);

  // Ensure DType variables match
  if (expected_dtype != data_dtype) {
    std::string mssg =
        "NDArray template datatype does not match specified datatype in npy "
        "file.";
    throw std::runtime_error(mssg);
  }

  if (data_shape.size() < 1) {
    std::string mssg =
        "Shape vector must have at least one element for NDArray.";
    throw std::runtime_error(mssg);
  }

  // Number of elements
  size_t ne = data_shape[0];
  for (size_t i = 1; i < data_shape.size(); i++) {
    ne *= data_shape[i];
  }

  data_vector = {reinterpret_cast<T*>(data_ptr),
                 reinterpret_cast<T*>(data_ptr) + ne};

  // Create NDArray object
  NDArray<T> return_object(data_vector, data_shape);
  return_object.c_continuous_ = data_c_continuous;

  // Free data_ptr
  delete[] data_ptr;

  // Return object
  return return_object;
}

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
void NDArray<T>::save(std::string fname) const {
  // Get expected DType according to T
  details::DType dtype;
  const char* T_type_name = typeid(T).name();

  if (T_type_name == typeid(char).name())
    dtype = details::DType::CHAR;
  else if (T_type_name == typeid(unsigned char).name())
    dtype = details::DType::UCHAR;
  else if (T_type_name == typeid(uint16_t).name())
    dtype = details::DType::UINT16;
  else if (T_type_name == typeid(uint32_t).name())
    dtype = details::DType::UINT32;
  else if (T_type_name == typeid(uint64_t).name())
    dtype = details::DType::UINT64;
  else if (T_type_name == typeid(int16_t).name())
    dtype = details::DType::INT16;
  else if (T_type_name == typeid(int32_t).name())
    dtype = details::DType::INT32;
  else if (T_type_name == typeid(int64_t).name())
    dtype = details::DType::INT64;
  else if (T_type_name == typeid(float).name())
    dtype = details::DType::FLOAT32;
  else if (T_type_name == typeid(double).name())
    dtype = details::DType::DOUBLE64;
  else if (T_type_name == typeid(std::complex<float>).name())
    dtype = details::DType::COMPLEX64;
  else if (T_type_name == typeid(std::complex<double>).name())
    dtype = details::DType::COMPLEX128;
  else {
    std::string mssg = "The datatype is not supported for NDArray.";
    throw std::runtime_error(mssg);
  }

  // Write data to file
  write_npy(fname, reinterpret_cast<const char*>(data_.data()), shape_, dtype,
            c_continuous_);
}

template <class T>
void NDArray<T>::fill(T val) {
  std::fill(data_.begin(), data_.end(), val);
}

template <class T>
void NDArray<T>::reshape(std::vector<size_t> new_shape) {
  // Ensure new shape has proper dimensions
  if (new_shape.size() < 1) {
    std::string mssg =
        "Shape vector must have at least one element to"
        " reshpae NDArray.";
    throw std::runtime_error(mssg);
  } else {
    size_t ne = new_shape[0];

    for (size_t i = 1; i < new_shape.size(); i++) {
      ne *= new_shape[i];
    }

    if (ne == data_.size()) {
      shape_ = new_shape;
      dimensions_ = shape_.size();
    } else {
      std::string mssg =
          "Shape is incompatible with number of elements in"
          " NDArray.";
      throw std::runtime_error(mssg);
    }
  }
}

template <class T>
void NDArray<T>::reallocate(std::vector<size_t> new_shape) {
  // Ensure new shape has proper dimensions
  if (new_shape.size() < 1) {
    std::string mssg =
        "Shape vector must have at least one element to"
        " reallocate NDArray.";
    throw std::runtime_error(mssg);
  } else {
    size_t ne = new_shape[0];

    for (size_t i = 1; i < new_shape.size(); i++) {
      ne *= new_shape[i];
    }

    shape_ = new_shape;
    dimensions_ = shape_.size();
    data_.resize(ne);
  }
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
    data_[i] -= c;
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

namespace details {
//==============================================================================
// NPY Function Definitions
inline void load_npy(std::string fname, char*& data_ptr,
                     std::vector<size_t>& shape, DType& dtype,
                     bool& c_contiguous) {
  // Open file
  std::ifstream file(fname, std::ios::binary);

  // Read magic string
  char* magic_string = new char[6];
  file.read(magic_string, 6);

  // Ensure magic string has right value
  if (magic_string[0] != '\x93' || magic_string[1] != 'N' ||
      magic_string[2] != 'U' || magic_string[3] != 'M' ||
      magic_string[4] != 'P' || magic_string[5] != 'Y') {
    std::string mssg = fname + " is an invalid .npy file.";
    throw std::runtime_error(mssg);
  }

  char major_verison = 0x00;
  char minor_version = 0x00;
  file.read(&major_verison, 1);
  file.read(&minor_version, 1);

  uint32_t length_of_header = 0;
  // Do any version checks here if required
  if (major_verison == 0x01) {
    uint16_t length_temp = 0;
    file.read((char*)&length_temp, 2);

    // Value is stored as little endian. If system is big-endian, swap bytes
    if (!system_is_little_endian()) {
      swap_bytes((char*)&length_temp, 2, 1);
    }

    // Cast to main variable
    length_of_header = static_cast<uint32_t>(length_temp);
  } else if (major_verison >= 0x02) {
    uint32_t length_temp = 0;
    file.read((char*)&length_temp, 4);

    // Value is stored as little endian. If system is big-endian, swap bytes
    if (!system_is_little_endian()) {
      swap_bytes((char*)&length_temp, 4, 1);
    }

    // Set to main variable
    length_of_header = length_temp;
  }

  // Array for header, and read in
  char* header_char = new char[length_of_header];
  file.read(header_char, length_of_header);
  std::string header(header_char);

  // Parse header to get continuity
  size_t loc1 = header.find("'fortran_order': ");
  loc1 += 17;
  std::string f_order_string = "";
  size_t i = 0;
  while (true) {
    if (header[loc1 + i] != ' ') {
      if (header[loc1 + i] == ',')
        break;
      else
        f_order_string += header[loc1 + i];
    }
    i++;
  }
  if (f_order_string == "False")
    c_contiguous = true;
  else
    c_contiguous = false;

  // Parse header to get type description
  loc1 = header.find("'descr': ");
  loc1 += 9;
  std::string descr_string = "";
  bool data_is_little_endian = false;
  if (header[loc1 + 1] == '>')
    data_is_little_endian = false;
  else
    data_is_little_endian = true;
  i = 2;
  while (true) {
    if (header[loc1 + i] != ' ') {
      if (header[loc1 + i] != '\'') {
        descr_string += header[loc1 + i];
      } else
        break;
    }
    i++;
  }
  dtype = descr_to_DType(descr_string);
  size_t element_size = size_of_DType(dtype);

  // Parse header to get shape
  loc1 = header.find('(');
  size_t loc2 = header.find(')');
  loc1 += 1;
  // Clear shape vector to ensure it's empty (even though it should already be
  // empty)
  shape.clear();
  if (loc1 == loc2) {
    shape.push_back(1);
  } else {
    std::string temp = "";
    for (i = loc1; i < loc2; i++) {
      if (header[i] != ',')
        temp += header[i];
      else {
        if (temp.size() > 0) {
          shape.push_back(std::stoi(temp));
          temp = "";
        }
      }
    }
    if (temp.size() > 0) {
      shape.push_back(std::stoi(temp));
    }
  }

  // Get number of bytes to be read into system
  uint64_t n_elements = shape[0];
  for (size_t j = 1; j < shape.size(); j++) n_elements *= shape[j];
  uint64_t n_bytes_to_read = n_elements * element_size;
  char* data = new char[n_bytes_to_read];
  file.read(data, n_bytes_to_read);

  // If byte order of data different from byte order of system, swap data bytes
  if (system_is_little_endian() != data_is_little_endian) {
    swap_bytes(data, n_elements, element_size);
  }

  // Set pointer reference
  data_ptr = data;

  // Clear Memory
  delete[] magic_string;
  delete[] header_char;
  file.close();
}

inline void write_npy(std::string fname, const char* data_ptr,
                      std::vector<size_t> shape, DType dtype,
                      bool c_contiguous) {
  // Calculate number of elements from the shape
  uint64_t n_elements = shape[0];
  for (size_t j = 1; j < shape.size(); j++) {
    n_elements *= shape[j];
  }

  // Open file
  std::ofstream file(fname, std::ios::binary);

  // First write magic string
  file << '\x93' << 'N' << 'U' << 'M' << 'P' << 'Y';

  // Next make the header. This is needed to know what version number to use
  std::string header = "{'descr': '";
  // Get system endianness
  if (system_is_little_endian())
    header += "<";
  else
    header += ">";
  header += DType_to_descr(dtype) + "', ";

  // Fortran ordering
  header += "'fortran_order': ";
  if (c_contiguous)
    header += "False, ";
  else
    header += "True, ";

  // Shape
  header += "'shape': (";
  for (const auto& e : shape) {
    header += std::to_string(e) + ",";
  }
  header += "), }";

  // Based on header length, get version
  char major_version;

  uint64_t beginning_length = 6 + 2 + 2 + header.size();
  uint8_t padding_needed = 64 - (beginning_length % 64);
  if (beginning_length + padding_needed > 65535) {
    major_version = 0x02;
    beginning_length += 2;
  } else
    major_version = 0x01;
  // Add padding
  for (uint8_t p = 0; p < padding_needed - 1; p++) header += '\x20';
  header += '\n';

  char minor_version = 0x00;
  file << major_version << minor_version;

  // Inf for len of header
  if (major_version == 0x01) {
    uint16_t len = static_cast<uint16_t>(header.size());
    if (!system_is_little_endian()) {
      swap_two_bytes((char*)&len);
    }
    file << ((char*)&len)[0] << ((char*)&len)[1];
  } else {
    uint32_t len = static_cast<uint32_t>(header.size());
    if (!system_is_little_endian()) {
      swap_four_bytes((char*)&len);
    }
    file << ((char*)&len)[0] << ((char*)&len)[1] << ((char*)&len)[2]
         << ((char*)&len)[3];
  }

  // Write header to file
  file << header.c_str();

  // Write all data to file
  uint64_t n_bytes = n_elements * size_of_DType(dtype);
  file.write(data_ptr, n_bytes);

  // Close file
  file.close();
}

inline DType descr_to_DType(std::string dtype) {
  if (dtype == "b1")
    return DType::CHAR;
  else if (dtype == "B1")
    return DType::UCHAR;
  else if (dtype == "i2")
    return DType::INT16;
  else if (dtype == "i4")
    return DType::INT32;
  else if (dtype == "i8")
    return DType::INT64;
  else if (dtype == "u2")
    return DType::UINT16;
  else if (dtype == "u4")
    return DType::UINT32;
  else if (dtype == "u8")
    return DType::UINT64;
  else if (dtype == "f4")
    return DType::FLOAT32;
  else if (dtype == "f8")
    return DType::DOUBLE64;
  else if (dtype == "c8")
    return DType::COMPLEX64;
  else if (dtype == "c16")
    return DType::COMPLEX128;
  else {
    std::string mssg = "Data type " + dtype + " is unknown.";
    throw std::runtime_error(mssg);
  }
}

inline std::string DType_to_descr(DType dtype) {
  switch (dtype) {
    case DType::CHAR:
      return "b1"; 
      break;
    case DType::UCHAR:
      return "B1";
      break;
    case DType::INT16:
      return "i2";
      break;
    case DType::INT32:
      return "i4";
      break;
    case DType::INT64:
      return "i8";
      break;
    case DType::UINT16:
      return "u2";
      break;
    case DType::UINT32:
      return "u4";
      break;
    case DType::UINT64:
      return "u8";
      break;
    case DType::FLOAT32:
      return "f4";
      break;
    case DType::DOUBLE64:
      return "f8";
      break;
    case DType::COMPLEX64:
      return "c8";
      break;
    case DType::COMPLEX128:
      return "c16";
      break;
    default: {
      std::string mssg = "Unknown DType identifier.";
      throw std::runtime_error(mssg);        
      break;
    }
  }
}

inline size_t size_of_DType(DType dtype) {
  switch (dtype) {
    case DType::CHAR:
      return 1;
      break;
    case DType::UCHAR:
      return 1;
      break;
    case DType::INT16:
      return 2;
      break;
    case DType::INT32:
      return 4;
      break;
    case DType::INT64:
      return 8;
      break;
    case DType::UINT16:
      return 2;
      break;
    case DType::UINT32:
      return 4;
      break;
    case DType::UINT64:
      return 8;
      break;
    case DType::FLOAT32:
      return 4;
      break;
    case DType::DOUBLE64:
      return 8;
      break;
    case DType::COMPLEX64:
      return 8;
      break;
    case DType::COMPLEX128:
      return 16;
      break;
    default: {
      std::string mssg = "Unknown DType identifier.";
      throw std::runtime_error(mssg);        
      break;
    }
  }
}

inline bool system_is_little_endian() {
  int x = 1;

  if (((char*)&x)[0] == 0x01)
    return true;
  else
    return false;
}

inline void swap_bytes(char* data, uint64_t n_elements, size_t element_size) {
  // Calculate number of total bytes
  uint64_t number_of_bytes = n_elements * element_size;

  // Iterate through all elements, and swap their bytes
  for (uint64_t i = 0; i < number_of_bytes; i += element_size) {
    switch (element_size) {
      case 1:
        // Nothing to do
        break;
      case 2:
        swap_two_bytes(data + i);  
        break;
      case 4:
        swap_four_bytes(data + i);
        break;
      case 8:
        swap_eight_bytes(data + i);
        break;
      case 16:
        swap_sixteen_bytes(data + i);
        break;
      default:
        std::string mssg = "Cannot swap bytes for data types of size " +
                           std::to_string(element_size);
        throw std::runtime_error(mssg);
        break;
    }
  }
}

inline void swap_two_bytes(char* bytes) {
  // Temporary array to store original bytes
  char temp[2];

  // Copyr original bytes into temp array
  std::memcpy(&temp, bytes, 2);

  // Set original bytes to new values
  bytes[0] = temp[1];
  bytes[1] = temp[0];
}

inline void swap_four_bytes(char* bytes) {
  // Temporary array to store original bytes
  char temp[4];

  // Copyr original bytes into temp array
  std::memcpy(&temp, bytes, 4);

  // Set original bytes to new values
  bytes[0] = temp[3];
  bytes[1] = temp[2];
  bytes[2] = temp[1];
  bytes[3] = temp[0];
}

inline void swap_eight_bytes(char* bytes) {
  // Temporary array to store original bytes
  char temp[8];

  // Copyr original bytes into temp array
  std::memcpy(&temp, bytes, 8);

  // Set original bytes to new values
  bytes[0] = temp[7];
  bytes[1] = temp[6];
  bytes[2] = temp[5];
  bytes[3] = temp[4];
  bytes[4] = temp[3];
  bytes[5] = temp[2];
  bytes[6] = temp[1];
  bytes[7] = temp[0];
}

inline void swap_sixteen_bytes(char* bytes) {
  // Temporary array to store original bytes
  char temp[16];

  // Copyr original bytes into temp array
  std::memcpy(&temp, bytes, 16);

  // Set original bytes to new values
  bytes[0] = temp[15];
  bytes[1] = temp[14];
  bytes[2] = temp[13];
  bytes[3] = temp[12];
  bytes[4] = temp[11];
  bytes[5] = temp[10];
  bytes[6] = temp[9];
  bytes[7] = temp[8];
  bytes[8] = temp[7];
  bytes[9] = temp[6];
  bytes[10] = temp[5];
  bytes[11] = temp[4];
  bytes[12] = temp[3];
  bytes[13] = temp[2];
  bytes[14] = temp[1];
  bytes[15] = temp[0];
}
} // namespace details
} // namespace exdir

#endif  // EXDIR_NDARRAY_H
