#include"exdir/exdir.hpp"
#include<gtest/gtest.h>

namespace {

  using namespace exdir;

  TEST(Array, construction) {
    double data[] {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    
    Array pointer_array(data, {16}, DType::DOUBLE64);

    EXPECT_DOUBLE_EQ(13.0, pointer_array(12));
    EXPECT_EQ(16,pointer_array.size());

    std::vector<double> data2 {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    Array vector_array(data2, {16}, DType::DOUBLE64);
    EXPECT_DOUBLE_EQ(13.0, vector_array(12));
    EXPECT_EQ(16,vector_array.size());
  }

  TEST(Array, linear_indexing) {
    std::vector<double> data2 {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    Array vector_array(data2, {16}, DType::DOUBLE64);

    EXPECT_DOUBLE_EQ(1.0, vector_array[0]);
    EXPECT_DOUBLE_EQ(16.0, vector_array[15]);
    EXPECT_DOUBLE_EQ(4.0, vector_array[3]);
    EXPECT_DOUBLE_EQ(13.0, vector_array[12]);
  }

  TEST(Array, array_indexing) {
    std::vector<double> data2 {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    Array vector_array(data2, {2,2,4}, DType::DOUBLE64);

    EXPECT_DOUBLE_EQ(1.0, vector_array(0,0,0));
    EXPECT_DOUBLE_EQ(16.0, vector_array(1,1,3));
    EXPECT_DOUBLE_EQ(4.0, vector_array(0,0,3));
    EXPECT_DOUBLE_EQ(13.0, vector_array(1,1,0));
  }

  TEST(Array, shape) {
    std::vector<double> data2 {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    Array vector_array(data2, {2,2,4}, DType::DOUBLE64);

    std::vector<size_t> shape = vector_array.shape();
    
    EXPECT_EQ(3,shape.size());
    EXPECT_EQ(2,shape[0]);
    EXPECT_EQ(2,shape[1]);
    EXPECT_EQ(4,shape[2]);
  }

  TEST(Array, reshape) {
    std::vector<double> data2 {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    Array vector_array(data2, {16}, DType::DOUBLE64);

    EXPECT_DOUBLE_EQ(1.0, vector_array[0]);
    EXPECT_DOUBLE_EQ(16.0, vector_array[15]);
    EXPECT_DOUBLE_EQ(4.0, vector_array[3]);
    EXPECT_DOUBLE_EQ(13.0, vector_array[12]);

    EXPECT_TRUE(vector_array.reshape({2,2,4}));
    EXPECT_FALSE(vector_array.reshape({1,2,4}));

    EXPECT_DOUBLE_EQ(1.0, vector_array(0,0,0));
    EXPECT_DOUBLE_EQ(16.0, vector_array(1,1,3));
    EXPECT_DOUBLE_EQ(4.0, vector_array(0,0,3));
    EXPECT_DOUBLE_EQ(13.0, vector_array(1,1,0));
  }

  TEST(Array, c_contiguous) {
    std::vector<double> data2 {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                   11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    Array vector_array(data2, {16}, DType::DOUBLE64);

    EXPECT_TRUE(vector_array.c_contiguous());
  }

  TEST(Array, fortran_contiguous) {
    std::vector<double> data2 {1.0, 5.0, 9.0, 13.0, 2.0, 6.0, 10.0, 14.0, 3.0, 7.0,
                               11.0, 15.0, 4.0, 8.0, 12.0, 16.0};
    Array vector_array(data2, {4,4}, DType::DOUBLE64, false);
    
    EXPECT_FALSE(vector_array.c_contiguous());

    EXPECT_DOUBLE_EQ(1.0, vector_array(0,0));
    EXPECT_DOUBLE_EQ(16.0, vector_array(3,3));
    EXPECT_DOUBLE_EQ(4.0, vector_array(0,3));
    EXPECT_DOUBLE_EQ(13.0, vector_array(3,0));
  }

  TEST(Array, dtype) {
    std::vector<double> data2 {1.0, 5.0, 9.0, 13.0, 2.0, 6.0, 10.0, 14.0, 3.0, 7.0,
                               11.0, 15.0, 4.0, 8.0, 12.0, 16.0};
    Array vector_array(data2, {4,4}, DType::DOUBLE64, false);
    
    EXPECT_EQ(DType::DOUBLE64, vector_array.dtype());

    std::vector<int> test_data { 1,  2,  3,  4,
                                 5,  6,  7,  8,
                                 9, 10, 11, 12,
                                13, 14, 15, 16};
    exdir::Array test_array(test_data, {4,4}, exdir::DType::INT64);
    EXPECT_EQ(DType::INT64, test_array.dtype());
  }

};