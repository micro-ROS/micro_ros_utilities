// Copyright (c) 2021 - for information on the respective copyright owner
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <malloc.h>

#include <micro_ros_utilities/string_utilities.h>
#include <micro_ros_utilities/type_utilities.h>
#include <std_msgs/msg/multi_array_layout.h>
#include <std_msgs/msg/multi_array_layout.h>

TEST(Test, micro_ros_utilities_strings)
{
  std::string data("Test string");
  rosidl_runtime_c__String str = micro_ros_string_utilities_init(data.c_str());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_TRUE(strcmp(data.c_str(), str.data) == 0);
  ASSERT_EQ(data.length(), str.size);
  ASSERT_EQ(data.length() + 1, str.capacity);

  std::string append("more text");
  std::string complete = data;
  complete.append(append);

  str = micro_ros_string_utilities_append(str, append.c_str());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_TRUE(strcmp(complete.c_str(), str.data) == 0);
  ASSERT_EQ(complete.length(), str.size);
  ASSERT_EQ(complete.length() + 1, str.capacity);

  str = micro_ros_string_utilities_remove_tail_chars(str, append.length());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_TRUE(strcmp(data.c_str(), str.data) == 0);
  ASSERT_EQ(data.length(), str.size);
  ASSERT_EQ(complete.length() + 1, str.capacity);

  micro_ros_string_utilities_destroy(&str);

  ASSERT_TRUE(str.data == NULL);
  ASSERT_TRUE(str.size == 0l);
  ASSERT_TRUE(str.capacity == 0l);

  rosidl_runtime_c__String empty = micro_ros_string_utilities_init("");

  ASSERT_FALSE(empty.data == NULL);
  ASSERT_EQ(empty.data[0], '\0');
  ASSERT_TRUE(empty.size == 0l);
  ASSERT_TRUE(empty.capacity == 1l);
}

static std::map<void *, size_t> allocated_memory_map;

extern "C" void * allocate(size_t size, void * /* state */)
{
  void * ptr = malloc(size);
  allocated_memory_map.emplace(std::pair<void *, size_t>(ptr, size));
  return ptr;
}

extern "C" void deallocate(void * pointer, void * /* state */)
{
  auto it = allocated_memory_map.find(pointer);
  allocated_memory_map.erase(it);
  free(pointer);
}

extern "C" void * reallocate(void * pointer, size_t size, void * /* state */)
{
  auto it = allocated_memory_map.find(pointer);
  allocated_memory_map.erase(it);
  void * ptr = realloc(pointer, size);
  allocated_memory_map.emplace(std::pair<void *, size_t>(ptr, size));
  return ptr;
}

extern "C" void * zero_allocate(
  size_t number_of_elements, size_t size_of_element,
  void * /* state */)
{
  void * ptr = calloc(number_of_elements, size_of_element);
  allocated_memory_map.emplace(
    std::pair<void *, size_t>(
      ptr,
      number_of_elements * size_of_element));
  return ptr;
}

extern "C" void * zero_allocate_fail(
  size_t /* number_of_elements */, size_t /* size_of_element */,
  void * /* state */)
{
  return NULL;
}


TEST(Test, default_config)
{
  const rosidl_message_type_support_t * typesupport = ROSIDL_GET_MSG_TYPE_SUPPORT(
    std_msgs, msg,
    MultiArrayLayout);
  allocated_memory_map.clear();

  auto ret = micro_ros_utilities_type_info(typesupport);
  std::cout << ret.data;

  ASSERT_NE(ret.data, nullptr);
  ASSERT_NE(ret.size, 0ul);
  ASSERT_NE(ret.capacity, 0ul);

  rcutils_allocator_t test_allocators = {
    allocate,
    deallocate,
    reallocate,
    zero_allocate,
    NULL
  };

  std_msgs__msg__MultiArrayLayout msg;
  micro_ros_utilities_memory_conf_t conf = {};
  conf.allocator = &test_allocators;

  size_t size = micro_ros_utilities_get_dynamic_size(
    typesupport,
    conf
  );

  // Size should be:
  //   Introspection for std_msgs__msg/MultiArrayLayout - 2 members, 32 B
  //  - dim [Sequence of ROS2 type std_msgs__msg/MultiArrayDimension] <- This should be inited
  //          - label [string] <- This should be inited
  //          - size [uint32]
  //          - stride [uint32]
  //  - data_offset [uint32]
  //
  //  dim.label.size -> 0
  //  dim.label.capacity -> 0
  //  dim.label.data -> 20 chars = 20 B -> We have 10 dim -> 20 B x 10 = 200 B

  //  dim.size -> 0
  //  dim.stride -> 0
  //  dim.data -> 10 std_msgs__msg/MultiArrayDimension = 10 * (aligned) 32 B = 320 B

  ASSERT_EQ(size, 520ul);

  ASSERT_TRUE(
    micro_ros_utilities_create_message_memory(
      typesupport,
      &msg,
      conf));

  size_t allocated_memory = 0;
  for (auto const & x: allocated_memory_map) {
    allocated_memory += x.second;
  }

  ASSERT_EQ(allocated_memory, size);

  ASSERT_EQ(msg.data_offset, 0ul);
  ASSERT_EQ(msg.dim.size, 0ul);
  ASSERT_EQ(msg.dim.capacity, memory_conf_default.max_ros2_type_sequence);
  ASSERT_NE(msg.dim.data, nullptr);

  for (size_t i = 0; i < msg.dim.capacity; i++) {
    ASSERT_EQ(msg.dim.data[i].size, 0ul);
    ASSERT_EQ(msg.dim.data[i].stride, 0ul);
    ASSERT_NE(msg.dim.data[i].label.data, nullptr);
    ASSERT_EQ(msg.dim.data[i].label.size, 0ul);
    ASSERT_EQ(msg.dim.data[i].label.capacity, memory_conf_default.max_string_capacity);
  }

  ASSERT_TRUE(
    micro_ros_utilities_destroy_message_memory(
      typesupport,
      &msg,
      conf));

  allocated_memory = 0;
  for (auto const & x: allocated_memory_map) {
    allocated_memory += x.second;
  }

  ASSERT_EQ(allocated_memory, 0ul);
}


TEST(Test, custom_config)
{
  const rosidl_message_type_support_t * typesupport = ROSIDL_GET_MSG_TYPE_SUPPORT(
    std_msgs, msg,
    MultiArrayLayout);
  allocated_memory_map.clear();

  rcutils_allocator_t test_allocators = {
    allocate,
    deallocate,
    reallocate,
    zero_allocate,
    NULL
  };

  std_msgs__msg__MultiArrayLayout msg;

  micro_ros_utilities_memory_conf_t conf = {};
  conf.allocator = &test_allocators;

  micro_ros_utilities_memory_rule_t rules[] = {
    {"dim", 3},
    {"dim.label", 100}
  };

  conf.rules = rules;
  conf.n_rules = sizeof(rules) / sizeof(rules[0]);

  size_t size = micro_ros_utilities_get_dynamic_size(
    typesupport,
    conf
  );

  ASSERT_TRUE(
    micro_ros_utilities_create_message_memory(
      typesupport,
      &msg,
      conf));

  size_t allocated_memory = 0;
  for (auto const & x: allocated_memory_map) {
    allocated_memory += x.second;
  }

  ASSERT_EQ(allocated_memory, size);

  ASSERT_EQ(msg.data_offset, 0ul);
  ASSERT_EQ(msg.dim.size, 0ul);
  ASSERT_EQ(msg.dim.capacity, rules[0].size);
  ASSERT_NE(msg.dim.data, nullptr);

  for (size_t i = 0; i < msg.dim.capacity; i++) {
    ASSERT_EQ(msg.dim.data[i].size, 0ul);
    ASSERT_EQ(msg.dim.data[i].stride, 0ul);
    ASSERT_NE(msg.dim.data[i].label.data, nullptr);
    ASSERT_EQ(msg.dim.data[i].label.size, 0ul);
    ASSERT_EQ(msg.dim.data[i].label.capacity, rules[1].size);
  }

  ASSERT_TRUE(
    micro_ros_utilities_destroy_message_memory(
      typesupport,
      &msg,
      conf));

  allocated_memory = 0;
  for (auto const & x: allocated_memory_map) {
    allocated_memory += x.second;
  }

  ASSERT_EQ(allocated_memory, 0ul);
}

TEST(Test, preallocated_custom_config)
{
  const rosidl_message_type_support_t * typesupport = ROSIDL_GET_MSG_TYPE_SUPPORT(
    std_msgs, msg,
    MultiArrayLayout);

  std_msgs__msg__MultiArrayLayout msg;

  micro_ros_utilities_memory_conf_t conf = {};
  micro_ros_utilities_memory_rule_t rules[] = {
    {"dim", 3},
    {"dim.label", 100}
  };

  conf.rules = rules;
  conf.n_rules = sizeof(rules) / sizeof(rules[0]);

  size_t size = micro_ros_utilities_get_dynamic_size(
    typesupport,
    conf
  );

  uint8_t * static_buffer = (uint8_t *) malloc(size * sizeof(uint8_t));

  ASSERT_TRUE(
    micro_ros_utilities_create_static_message_memory(
      typesupport,
      &msg,
      conf,
      static_buffer,
      size));

  ASSERT_EQ(msg.data_offset, 0ul);
  ASSERT_EQ(msg.dim.size, 0ul);
  ASSERT_EQ(msg.dim.capacity, rules[0].size);
  ASSERT_NE(msg.dim.data, nullptr);

  for (size_t i = 0; i < msg.dim.capacity; i++) {
    ASSERT_EQ(msg.dim.data[i].size, 0ul);
    ASSERT_EQ(msg.dim.data[i].stride, 0ul);
    ASSERT_NE(msg.dim.data[i].label.data, nullptr);
    ASSERT_EQ(msg.dim.data[i].label.size, 0ul);
    ASSERT_EQ(msg.dim.data[i].label.capacity, rules[1].size);
  }
}

TEST(Test, failures)
{
  const rosidl_message_type_support_t * typesupport = ROSIDL_GET_MSG_TYPE_SUPPORT(
    std_msgs, msg,
    MultiArrayLayout);

  std_msgs__msg__MultiArrayLayout msg;

  rcutils_allocator_t test_allocators = {
    allocate,
    deallocate,
    reallocate,
    zero_allocate_fail,
    NULL
  };

  micro_ros_utilities_memory_conf_t conf = {};
  conf.allocator = &test_allocators;

  size_t size = micro_ros_utilities_get_dynamic_size(
    typesupport,
    conf
  );

  size = size - 100;

  uint8_t * static_buffer = (uint8_t *) malloc(size * sizeof(uint8_t));

  ASSERT_FALSE(
    micro_ros_utilities_create_static_message_memory(
      typesupport,
      &msg,
      conf,
      static_buffer,
      size));

  ASSERT_FALSE(
    micro_ros_utilities_create_message_memory(
      typesupport,
      &msg,
      conf));

}
