#ifndef EXDIR_NPY_H
#define EXDIR_NPY_H

#include<cstdint>
#include<cstring>
#include<fstream>
#include<filesystem>
#include<iostream>
#include<vector>
#include<exception>

namespace exdir {

  // Enum of possible data types which are currently handeled by this Exdir implementation.
  enum class DType {CHAR, UCHAR, INT16, INT32, INT64, UINT16, UINT32, UINT64, FLOAT32, DOUBLE64};

  // Function to check wether or not host system is little-endian or not
  bool system_is_little_endian();

  // Function to switch bye order of data for an array which contains n_elements,
  // each with element_size bytes. The size of data should therefore be
  // n_elements*element_size; if not, this is undefined behavior.
  void swap_bytes(char* data, uint64_t n_elements, size_t element_size);

  // Function which opens file fname, and loads in the binary data into 1D array of
  // chars, which must latter be type cast be the user. The char* to the data is returned
  // as a reference, along with the number of elements, continuits, and element size in bytes.
  // A vector describing the shape is returned.
  std::vector<size_t> load_npy(std::filesystem::path fpath, char*& data_ptr, uint64_t& n_elements, DType& dtype, bool& c_contiguous);

  // Function whihc writes binary data to a Numpy .npy file.
  void write_npy(std::filesystem::path fpath, const char* data_ptr, uint64_t n_elements, std::vector<size_t> shape, DType dtype, bool c_contiguous);

  // Returns the proper DType for a given Numpy dtype.descr string.
  DType descr_to_DType(std::string dtype);

  // Returns Numpy descr string for given DType
  std::string DType_to_descr(DType dtype);

  // Returns the number of bytes used to represent the provided DType.
  size_t size_of_DType(DType dtype);

  // Swaps the first and second byte pointed to by char* bytes.
  void swap_two_bytes(char* bytes);

  // Swaps the first four bytes pointed to by char* bytes.
  void swap_four_bytes(char* bytes);

  // Swaps the first eight bytes pointed to by char* bytes.
  void swap_eight_bytes(char* bytes);

}; // exdir
#endif // EXDIR_NPY_H