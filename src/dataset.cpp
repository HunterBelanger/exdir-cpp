#include <exdir/dataset.hpp>

namespace exdir {

Dataset::Dataset(std::filesystem::path i_path)
    : Object{i_path},
      data_{nullptr},
      shape_{},
      n_elements_{0},
      element_size_{0},
      dtype_{DType::DOUBLE64},
      c_contiguous_{true} {
  if (!is_dataset()) {
    std::string mssg = path_.string() + " does not contain a Dataset object.";
    throw std::runtime_error(mssg);
  }

  // Make sure data.npy is present
  if (!std::filesystem::exists(path_ / "data.npy")) {
    std::string mssg = (path_ / "data.npy").string() + " does not exists.";
    throw std::runtime_error(mssg);
  }

  // Load data
  load_data();

  // Get any raw folders in directory
  // Look at all members in file, check if folder
  for (auto& f : std::filesystem::directory_iterator(path_)) {
    if (std::filesystem::is_directory(f.status())) {
      // Is a directory, must be raw if in dataset
      raws_.push_back(f.path().filename().string());
    }
  }
}

Dataset::~Dataset() {
  if (data_) {
    clear_data();
  }
}

void Dataset::create_raw(std::string name) {
  // Make sure directory does not yet exists
  if (!std::filesystem::exists(path_ / name)) {
    // Make directory
    std::filesystem::create_directory(path_ / name);

    // Make exdir.yaml file for directory
    std::ofstream exdir_yaml(path_ / name / "exdir.yaml");
    exdir_yaml << "exdir:";
    // TODO put version in a header eventuall
    exdir_yaml << "  version: " << 1 << "\n";
    exdir_yaml << "  type: \"raw\"";
    exdir_yaml.close();

    // Add raw name to raws_ for latter
    raws_.push_back(name);

  } else {
    std::string mssg =
        "The directory " + name + " already exists in " + path_.string();
    throw std::runtime_error(mssg);
  }
}

void Dataset::load_data() {
  load_npy((path_ / "data.npy"), data_, shape_, dtype_, c_contiguous_);
  element_size_ = size_of_DType(dtype_);
}

void Dataset::clear_data() {
  n_elements_ = 0;
  element_size_ = 0;
  delete[] data_;
  data_ = nullptr;
}

bool Dataset::data_loaded() const {
  if (data_)
    return true;
  else
    return false;
}

template <class T>
void Dataset::write(Array<T> data) {
  write_npy(path_ / "data.npy",
            reinterpret_cast<const char*>(data.data_pointer()), data.shape(),
            data.dtype(), data.c_contiguous());

  // Write attributes as well
  if (!attrs.IsNull()) {
    std::ofstream attributes_yaml(path_ / "attributes.yaml");
    attributes_yaml << attrs;
    attributes_yaml.close();
  }
}

DType Dataset::dtype() const {return dtype_;}

Raw Dataset::get_raw(std::string name) const {
  // Make sure in raws_
  for (const auto& raw : raws_) {
    if (name == raw) {
      return exdir::Raw(path_ / name);
    }
  }
  // throw error, wasn't valid Raw
  std::string mssg =
      "The raw " + name + " is not a member of " + this->name() + ".";
  throw std::runtime_error(mssg);
}

std::vector<std::string> Dataset::member_raws() const { return raws_; }

};  // namespace exdir