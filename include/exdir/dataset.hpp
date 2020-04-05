#ifndef EXDIR_DATASET_H
#define EXDIR_DATASET_H

#include<exdir/object.hpp>
#include<exdir/raw.hpp>
#include<exdir/npy.hpp>
#include<exdir/array.hpp>

namespace exdir {

  class Dataset : public Object {
    public:
      Dataset(std::filesystem::path i_path);
      ~Dataset();

      void create_raw(std::string name);

      // Loads data file into object for temporary storage.
      void load_data();

      // Errases data in object. To obtain data, must use load_data();
      void clear_data();

      // Removes data from temporary object sorage, and places it in the returned
      // array. After data has been retrieved, the object is empty, and the method
      // load_data() must be used in-order to retrieve another copy of the data.
      template<class T>
      exdir::Array<T> retrieve_data() {
        if(data_) {
          exdir::Array<T> out(reinterpret_cast<T*>(data_), shape_, dtype_, c_contiguous_);
          clear_data();
          return out;
        } else {
          std::string mssg = " Cannot retrieve data unless data has been loaded.";
          throw std::runtime_error(mssg);
        }
      }

      bool data_loaded() const;

      // Overrides write method in Object.
      template<class T>
      void write(Array<T> data);

      exdir::Raw get_raw(std::string) const;
      std::vector<std::string> member_raws() const;

    private:
      char* data_;
      std::vector<size_t> shape_;
      DType dtype_;
      uint64_t n_elements_;
      size_t element_size_;
      bool c_contiguous_;

      std::vector<std::string> raws_;
  }; // Dataset

}; // exdir
#endif // EXDIR_DATASET_H