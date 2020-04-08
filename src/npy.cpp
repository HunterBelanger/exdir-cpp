#include "exdir/npy.hpp"

namespace exdir {

void load_npy(std::filesystem::path fpath, char*& data_ptr,
              std::vector<size_t>& shape, DType& dtype, bool& c_contiguous) {
  // Open file
  std::ifstream file(fpath);

  // Read magic string
  char* magic_string = new char[6];
  file.read(magic_string, 6);

  // Ensure magic string has right value
  if (magic_string[0] != '\x93' || magic_string[1] != 'N' ||
      magic_string[2] != 'U' || magic_string[3] != 'M' ||
      magic_string[4] != 'P' || magic_string[5] != 'Y') {
    std::string mssg = fpath.string() + " is an invalid .npy file.";
    throw std::runtime_error(mssg);
  }

  char major_verison = 0x00;
  char minor_version = 0x00;
  file.read(&major_verison, 1);
  file.read(&minor_version, 1);

  uint32_t length_of_header;
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

void write_npy(std::filesystem::path fpath, const char* data_ptr,
               std::vector<size_t> shape, DType dtype, bool c_contiguous) {
  // Calculate number of elements from the shape
  uint64_t n_elements = shape[0];
  for (size_t j = 1; j < shape.size(); j++) {
    n_elements *= shape[j];
  }

  // Open file
  std::ofstream file(fpath);

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
  for (uint64_t i = 0; i < n_bytes; i++) file << data_ptr[i];

  // Close file
  file.close();
}

DType descr_to_DType(std::string dtype) {
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
  else {
    std::string mssg = "Data type " + dtype + " is unknown.";
    throw std::runtime_error(mssg);
  }
}

std::string DType_to_descr(DType dtype) {
  if (dtype == DType::CHAR)
    return "b1";
  else if (dtype == DType::UCHAR)
    return "B1";
  else if (dtype == DType::INT16)
    return "i2";
  else if (dtype == DType::INT32)
    return "i4";
  else if (dtype == DType::INT64)
    return "i8";
  else if (dtype == DType::UINT16)
    return "u2";
  else if (dtype == DType::UINT32)
    return "u4";
  else if (dtype == DType::UINT64)
    return "u8";
  else if (dtype == DType::FLOAT32)
    return "f4";
  else if (dtype == DType::DOUBLE64)
    return "f8";
  else {
    std::string mssg = "Unknown DType identifier.";
    throw std::runtime_error(mssg);
  }
}

size_t size_of_DType(DType dtype) {
  if (dtype == DType::CHAR)
    return 1;
  else if (dtype == DType::UCHAR)
    return 1;
  else if (dtype == DType::INT16)
    return 2;
  else if (dtype == DType::INT32)
    return 4;
  else if (dtype == DType::INT64)
    return 8;
  else if (dtype == DType::UINT16)
    return 2;
  else if (dtype == DType::UINT32)
    return 4;
  else if (dtype == DType::UINT64)
    return 8;
  else if (dtype == DType::FLOAT32)
    return 4;
  else if (dtype == DType::DOUBLE64)
    return 8;
  else {
    std::string mssg = "Unknown DType identifier.";
    throw std::runtime_error(mssg);
  }
}

bool system_is_little_endian() {
  int x = 1;

  if (((char*)&x)[0] == 0x01)
    return true;
  else
    return false;
}

void swap_bytes(char* data, uint64_t n_elements, size_t element_size) {
  // Calculate number of total bytes
  uint64_t number_of_bytes = n_elements * element_size;

  // Iterate through all elements, and swap their bytes
  for (uint64_t i = 0; i < number_of_bytes; i += element_size) {
    if (element_size == 1) {
      // Nothing to do
    } else if (element_size == 2) {
      swap_two_bytes(data + i);
    } else if (element_size == 4) {
      swap_four_bytes(data + i);
    } else if (element_size == 8) {
      swap_eight_bytes(data + i);
    } else {
      std::string mssg = "Cannot swap bytes for data types of size " +
                         std::to_string(element_size);
      throw std::runtime_error(mssg);
    }
  }
}

void swap_two_bytes(char* bytes) {
  // Temporary array to store original bytes
  char temp[2];

  // Copyr original bytes into temp array
  std::memcpy(&temp, bytes, 2);

  // Set original bytes to new values
  bytes[0] = temp[1];
  bytes[1] = temp[0];
}

void swap_four_bytes(char* bytes) {
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

void swap_eight_bytes(char* bytes) {
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

};  // namespace exdir