#include<exdir/exdir.hpp>

int main() {

  exdir::File file = exdir::create_file("test.exdir");

  exdir::Group group1 = file.create_group("group1");

  std::vector<int> test_data { 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};

  exdir::NDArray<int> test_array(test_data, {4,4});

  test_array.reshape({4,2,2});

  exdir::Dataset<int> dset1 = group1.create_dataset<int>("data_set_1", test_array);

  dset1.data.reshape({4,4});

  return 0;
}
