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

#ifndef MICRO_ROS_UTILITIES__TYPE_UTILITIES_H_
#define MICRO_ROS_UTILITIES__TYPE_UTILITIES_H_

#if __cplusplus
extern "C"
{
#endif  // if __cplusplus

#include <rosidl_typesupport_introspection_c/message_introspection.h>
#include <rosidl_runtime_c/string.h>
#include <rcutils/allocator.h>

typedef struct micro_ros_utilities_memory_rule_t
{
  const char * rule;
  size_t size;
} micro_ros_utilities_memory_rule_t;

typedef struct micro_ros_utilities_memory_conf_t
{
    size_t max_string_capacity;
    size_t max_ros2_type_sequence;
    size_t max_basic_type_sequence;
    size_t n_rules;
    const micro_ros_utilities_memory_rule_t * rules;
    const rcutils_allocator_t * allocator;
} micro_ros_utilities_memory_conf_t;

extern const micro_ros_utilities_memory_conf_t memory_conf_default;

/**
 *  Initializes a RCLC parameter server in a node with default configuration
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | No
 *
 * \param[inout] parameter_server preallocated rclc_parameter_server_t
 * \param[in] node related node
 * \return `RCL_RET_OK` if the \p parameter_server was initialized successfully
 */

rosidl_runtime_c__String micro_ros_utilities_type_info(const rosidl_message_type_support_t* type_support);
size_t micro_ros_utilities_get_dynamic_size(const rosidl_message_type_support_t* type_support, const micro_ros_utilities_memory_conf_t conf);
bool micro_ros_utilities_create_message_memory(const rosidl_message_type_support_t* type_support, void* ros_msg, const micro_ros_utilities_memory_conf_t conf);
bool micro_ros_utilities_create_static_message_memory( const rosidl_message_type_support_t* type_support, void* ros_msg, const micro_ros_utilities_memory_conf_t conf, uint8_t * buffer, size_t buffer_len);
bool micro_ros_utilities_destroy_message_memory(const rosidl_message_type_support_t* type_support, void* ros_msg, const micro_ros_utilities_memory_conf_t conf);

#if __cplusplus
}
#endif  // if __cplusplus

#endif  // MICRO_ROS_UTILITIES__TYPE_UTILITIES_H_
