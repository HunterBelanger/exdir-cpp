#include <exdir/array.hpp>

namespace exdir {

template <class T>
Array<T>::Array(T* data_pntr, std::vector<size_t> i_shape, DType i_dtype,
                bool c_cont)
    : shape_{i_shape},
      data_{},
      dtype_{i_dtype},
      n_elements_{0},
      c_contiguous_{c_cont} {
  // Get number of elements from shape_ vector
  n_elements_ = shape_[0];
  for (size_t i = 1; i < shape_.size(); i++) n_elements_ *= shape_[i];

  // Load data into vector
  data_ = {data_pntr, data_pntr + n_elements_};

  if (data_.size() != n_elements_) {
    std::string mssg = "Number of elements does not match shape.";
    throw std::runtime_error(mssg);
  }
}

template <class T>
Array<T>::Array(std::vector<T> i_data, std::vector<size_t> i_shape,
                DType i_dtype, bool c_cont)
    : shape_{i_shape},
      data_{i_data},
      dtype_{i_dtype},
      n_elements_{0},
      c_contiguous_{c_cont} {
  // Get number of elements from shape_ vector
  n_elements_ = shape_[0];
  for (size_t i = 1; i < shape_.size(); i++) n_elements_ *= shape_[i];

  if (data_.size() != n_elements_) {
    std::string mssg = "Number of elements does not match shape.";
    throw std::runtime_error(mssg);
  }
}

template <class T>
T& Array<T>::operator[](size_t i) {
  return data_[i];
}

template <class T>
T& Array<T>::operator()(std::vector<size_t> indicies) {
  if (indicies.size() != shape_.size()) {
    std::string mssg = "Improper number of indicies provided to exdir::Array.";
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

template <class T>
std::vector<size_t> Array<T>::shape() const {
  return shape_;
}

template <class T>
bool Array<T>::reshape(std::vector<size_t> new_shape) {
  uint64_t new_n_elements = new_shape[0];
  for (size_t i = 1; i < new_shape.size(); i++) {
    new_n_elements *= new_shape[i];
  }

  if (new_n_elements == n_elements_) {
    shape_ = new_shape;
    return true;
  } else {
    return false;
  }
}

template <class T>
size_t Array<T>::size() const {
  return n_elements_;
}

template <class T>
size_t Array<T>::c_contiguous_index(std::vector<size_t>& indicies) {
  size_t indx = 0;
  // Go through all dimensions
  for (size_t k = 1; k <= shape_.size(); k++) {
    size_t product = 0;
    if (k == shape_.size())
      product = 1;
    else {
      for (size_t l = k + 1; l <= shape_.size(); l++) {
        if (product == 0)
          product = shape_[l - 1];
        else
          product *= shape_[l - 1];
      }
    }

    indx += product * indicies[k - 1];
  }
  return indx;
}

template <class T>
size_t Array<T>::fortran_contiguous_index(std::vector<size_t>& indicies) {
  size_t indx = 0;
  // Go through all dimensions
  for (size_t k = 1; k <= shape_.size(); k++) {
    size_t product = 0;
    if (k == 1)
      product = 1;
    else {
      for (size_t l = 1; l <= k - 1; l++) {
        if (product == 0)
          product = shape_[l - 1];
        else
          product *= shape_[l - 1];
      }
    }

    indx += product * indicies[k - 1];
  }
  return indx;
}

template <class T>
const T* Array<T>::data_pointer() const {
  return data_.data();
}

template <class T>
bool Array<T>::c_contiguous() const {
  return c_contiguous_;
}

template <class T>
DType Array<T>::dtype() const {
  return dtype_;
}

template class Array<float>;
template class Array<double>;
template class Array<int16_t>;
template class Array<int32_t>;
template class Array<int64_t>;
template class Array<uint16_t>;
template class Array<uint32_t>;
template class Array<uint64_t>;
template class Array<char>;
template class Array<unsigned char>;

};  // namespace exdir