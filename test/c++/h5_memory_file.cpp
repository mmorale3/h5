// Copyright (c) 2021 Simons Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: Nils Wentzell

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <map>
#include <fstream>

TEST(H5, MemoryFile) {

  // Some data to write
  auto vec_int = std::vector<int>{1, 2, 3};
  auto vec_dbl = std::vector<double>{4.0, 5.0, 6.0};
  auto vec_str = std::vector<std::string>{"Hello", "there!"};

  // The data buffers
  std::vector<std::byte> buf_mem, buf_disk, buf_raw;

  {
    // Create file in memory
    auto f_mem = h5::file{};
    h5::write(f_mem, "vec_int", vec_int);
    h5::write(f_mem, "vec_dbl", vec_dbl);

    // Write memory file to disk as binary data
    buf_mem = f_mem.as_buffer();
    std::ofstream ostrm("h5_bin_out.h5", std::ios::binary);
    ostrm.write(reinterpret_cast<char *>(buf_mem.data()), buf_mem.size());
  }

  {
    // Write to on-disk h5 file
    auto f_disk = h5::file{"h5_bin_in.h5", 'w'};
    h5::write(f_disk, "vec_int", vec_int);
    h5::write(f_disk, "vec_dbl", vec_dbl);
    buf_disk = f_disk.as_buffer();

    EXPECT_EQ(buf_mem, buf_disk);
  }

  {
    // Read from disk as raw data
    std::ifstream istrm{"h5_bin_in.h5", std::ios::binary | std::ios::ate};
    buf_raw.resize(istrm.tellg(), std::byte{0});
    istrm.seekg(0);
    istrm.read(reinterpret_cast<char *>(buf_raw.data()), buf_raw.size());
    EXPECT_EQ(buf_disk, buf_raw);
  }

  std::vector<int> vec_int_read;
  std::vector<double> vec_dbl_read;
  std::vector<std::string> vec_str_read;
  {
    // Create file in memory from buffer
    auto f = h5::file{buf_raw};

    h5::read(f, "vec_int", vec_int_read);
    h5::read(f, "vec_dbl", vec_dbl_read);

    h5::write(f, "vec_str", vec_str);
    h5::read(f, "vec_str", vec_str_read);

    EXPECT_EQ(vec_int, vec_int_read);
    EXPECT_EQ(vec_dbl, vec_dbl_read);
    EXPECT_EQ(vec_str, vec_str_read);
  }
}
