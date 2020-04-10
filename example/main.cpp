#include<exdir/exdir.hpp>

int main() {

  exdir::File file = exdir::create_file("test.exdir");

  file.create_group("group1");

  exdir::Group group1 = file.get_group("group1");

  std::vector<int> test_data { 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};

  exdir::Array test_array(test_data, {4,4}, exdir::DType::INT64);

  test_array.reshape({4,2,2});

  group1.create_dataset("data_set_1", test_array);

  return 0;
}