#include <exdir/dataset.hpp>

namespace exdir {

template<class T>
Dataset<T>::Dataset(std::filesystem::path i_path) : Object{i_path} {
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
  data = NDArray<T>::load(path_/"data.npy");

  // Get any raw folders in directory
  // Look at all members in file, check if folder
  for (auto& f : std::filesystem::directory_iterator(path_)) {
    if (std::filesystem::is_directory(f.status())) {
      // Is a directory, must be raw if in dataset
      raws_.push_back(f.path().filename().string());
    }
  }
}

template<class T>
Dataset<T>::~Dataset() {
  write();
}

template<class T>
Raw Dataset<T>::create_raw(std::string name) {
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

  return get_raw(name);
}

template <class T>
void Dataset<T>::write() {
  // Write data to npy file
  data.save(path_/"data.npy"); 

  // Write attributes as well
  if (!attrs.IsNull()) {
    std::ofstream attributes_yaml(path_ / "attributes.yaml");
    attributes_yaml << attrs;
    attributes_yaml.close();
  }
}

template<class T>
Raw Dataset<T>::get_raw(std::string name) const {
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

template<class T>
std::vector<std::string> Dataset<T>::member_raws() const { return raws_; }

};  // namespace exdir

// Explicit Instantiation
template class exdir::Dataset<char>;
template class exdir::Dataset<unsigned char>;
template class exdir::Dataset<uint16_t>;
template class exdir::Dataset<uint32_t>;
template class exdir::Dataset<uint64_t>;
template class exdir::Dataset<int16_t>;
template class exdir::Dataset<int32_t>;
template class exdir::Dataset<int64_t>;
template class exdir::Dataset<float>;
template class exdir::Dataset<double>;
template class exdir::Dataset<std::complex<float>>;
template class exdir::Dataset<std::complex<double>>;
