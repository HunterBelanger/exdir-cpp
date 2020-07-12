#include<exdir/ndarray.hpp>

#include"npy.hpp"

namespace exdir {

template <class T>
NDArray<T>::NDArray() {}

template <class T>
NDArray<T>::NDArray(std::vector<size_t> init_shape, bool c_continuous) {
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
                    bool c_continuous) {
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
  DType expected_dtype;
  const char* T_type_name = typeid(T).name();

  if (T_type_name == typeid(char).name())
    expected_dtype = DType::CHAR;
  else if (T_type_name == typeid(unsigned char).name())
    expected_dtype = DType::UCHAR;
  else if (T_type_name == typeid(uint16_t).name())
    expected_dtype = DType::UINT16;
  else if (T_type_name == typeid(uint32_t).name())
    expected_dtype = DType::UINT32;
  else if (T_type_name == typeid(uint64_t).name())
    expected_dtype = DType::UINT64;
  else if (T_type_name == typeid(int16_t).name())
    expected_dtype = DType::INT16;
  else if (T_type_name == typeid(int32_t).name())
    expected_dtype = DType::INT32;
  else if (T_type_name == typeid(int64_t).name())
    expected_dtype = DType::INT64;
  else if (T_type_name == typeid(float).name())
    expected_dtype = DType::FLOAT32;
  else if (T_type_name == typeid(double).name())
    expected_dtype = DType::DOUBLE64;
  else if (T_type_name == typeid(std::complex<float>).name())
    expected_dtype = DType::COMPLEX64;
  else if (T_type_name == typeid(std::complex<double>).name())
    expected_dtype = DType::COMPLEX128;
  else {
    std::string mssg = "The datatype is not supported for NDArray.";
    throw std::runtime_error(mssg);
  }

  // Variables to send to npy function
  char* data_ptr;
  std::vector<T> data_vector;
  std::vector<size_t> data_shape;
  DType data_dtype;
  bool data_c_continuous;

  // Load data into variables
  load_npy(fname, data_ptr, data_shape, data_dtype, data_c_continuous);

  // Ensuire DType variables match
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
void NDArray<T>::save(std::string fname) const {
  // Get expected DType according to T
  DType dtype;
  const char* T_type_name = typeid(T).name();

  if (T_type_name == typeid(char).name())
    dtype = DType::CHAR;
  else if (T_type_name == typeid(unsigned char).name())
    dtype = DType::UCHAR;
  else if (T_type_name == typeid(uint16_t).name())
    dtype = DType::UINT16;
  else if (T_type_name == typeid(uint32_t).name())
    dtype = DType::UINT32;
  else if (T_type_name == typeid(uint64_t).name())
    dtype = DType::UINT64;
  else if (T_type_name == typeid(int16_t).name())
    dtype = DType::INT16;
  else if (T_type_name == typeid(int32_t).name())
    dtype = DType::INT32;
  else if (T_type_name == typeid(int64_t).name())
    dtype = DType::INT64;
  else if (T_type_name == typeid(float).name())
    dtype = DType::FLOAT32;
  else if (T_type_name == typeid(double).name())
    dtype = DType::DOUBLE64;
  else if (T_type_name == typeid(std::complex<float>).name())
    dtype = DType::COMPLEX64;
  else if (T_type_name == typeid(std::complex<double>).name())
    dtype = DType::COMPLEX128;
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

}; // namespace exdir

// Explicit Instantiation
template class exdir::NDArray<char>;
template class exdir::NDArray<unsigned char>;
template class exdir::NDArray<uint16_t>;
template class exdir::NDArray<uint32_t>;
template class exdir::NDArray<uint64_t>;
template class exdir::NDArray<int16_t>;
template class exdir::NDArray<int32_t>;
template class exdir::NDArray<int64_t>;
template class exdir::NDArray<float>;
template class exdir::NDArray<double>;
template class exdir::NDArray<std::complex<float>>;
template class exdir::NDArray<std::complex<double>>;
