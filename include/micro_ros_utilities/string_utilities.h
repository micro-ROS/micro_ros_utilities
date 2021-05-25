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

#ifndef MICRO_ROS_UTILITIES__STRING_UTILITIES_H_
#define MICRO_ROS_UTILITIES__STRING_UTILITIES_H_

#if __cplusplus
extern "C"
{
#endif  // if __cplusplus

#include <rosidl_runtime_c/string.h>


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

rosidl_runtime_c__String micro_ros_string_utilities_init(const char * data);
const char * micro_ros_string_utilities_get_c_str(rosidl_runtime_c__String str);
rosidl_runtime_c__String micro_ros_string_utilities_append(rosidl_runtime_c__String str, const char * data);
rosidl_runtime_c__String micro_ros_string_utilities_remove_tail_chars(rosidl_runtime_c__String str, const size_t n);
void micro_ros_string_utilities_destroy(rosidl_runtime_c__String * str);

#if __cplusplus
}
#endif  // if __cplusplus

#endif  // MICRO_ROS_UTILITIES__STRING_UTILITIES_H_
