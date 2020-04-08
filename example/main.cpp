#include<exdir/exdir.hpp>

int main() {

  exdir::File file = exdir::create_file("test.exdir");

  file.create_group("group1");

  exdir::Group group1 = file.get_group("group1");

  std::vector<double> test_data {1.0, 2.0, 3.0, 4.0, 5.0};
  exdir::Array test_array(test_data, {5}, exdir::DType::DOUBLE64);

  group1.create_dataset("data_set_1", test_array);

  for(int i = 0 ; i < 5; i++) {
    std::cout << test_array(i) << "\n";
  }

  return 0;
}