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

#include <micro_ros_utilities/type_utilities.h>

#include <string.h>

#include <rosidl_typesupport_introspection_c/identifier.h>
#include <micro_ros_utilities/string_utilities.h>

static const char * basic_type_names[] =
{"none", "float", "double", "long double", "char", "wchar", "bool", "octec", "uint8", "int8",
  "uint16", "int16", "uint32", "int32", "uint64", "int64", "string", "wstring", "ROS2 type"};
static const size_t basic_types_size[] =
{0, sizeof(float), sizeof(double), sizeof(double), sizeof(char), sizeof(char), sizeof(bool),
  sizeof(char), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint16_t), sizeof(uint16_t),
  sizeof(uint32_t), sizeof(uint32_t), sizeof(uint64_t), sizeof(uint64_t), sizeof(char),
  sizeof(char), 0};

typedef struct generic_sequence_t
{
  void * data;
  size_t size;
  size_t capacity;
} generic_sequence_t;

const micro_ros_utilities_memory_conf_t memory_conf_default = {
  .max_string_capacity = 20,
  .max_ros2_type_sequence = 10,
  .max_basic_type_sequence = 20,
  .rules = NULL,
  .n_rules = 0,
};

static uint8_t * operation_buffer;
static size_t operation_buffer_index;

void * get_static_memory(size_t size)
{
  void * ptr = &operation_buffer[operation_buffer_index];
  operation_buffer_index += size;
  return ptr;
}

static micro_ros_utilities_memory_conf_t get_configuration(
  const micro_ros_utilities_memory_conf_t conf)
{
  micro_ros_utilities_memory_conf_t ret = memory_conf_default;
  ret.max_basic_type_sequence =
    (conf.max_basic_type_sequence ==
    0) ? memory_conf_default.max_basic_type_sequence : conf.max_basic_type_sequence;
  ret.max_ros2_type_sequence =
    (conf.max_ros2_type_sequence ==
    0) ? memory_conf_default.max_ros2_type_sequence : conf.max_ros2_type_sequence;
  ret.max_string_capacity =
    (conf.max_string_capacity ==
    0) ? memory_conf_default.max_string_capacity : conf.max_string_capacity;

  ret.rules = (conf.rules == NULL) ? memory_conf_default.rules : conf.rules;
  ret.n_rules = conf.n_rules;

  ret.allocator = conf.allocator;
  return ret;
}

void print_type_info(
  const rosidl_typesupport_introspection_c__MessageMembers * members,
  rosidl_runtime_c__String * out, rosidl_runtime_c__String indent_buffer,
  uint32_t level)
{
  indent_buffer = micro_ros_string_utilities_append(indent_buffer, "\t");
  char buffer[500];

  if (level == 0) {
    snprintf(
      buffer, sizeof(buffer), "%sIntrospection for %s/%s - %d members, %ld B\n",
      indent_buffer.data,
      members->message_namespace_,
      members->message_name_,
      members->member_count_,
      members->size_of_
    );

    *out = micro_ros_string_utilities_append(*out, buffer);
  }

  for (size_t i = 0; i < members->member_count_; i++) {
    rosidl_typesupport_introspection_c__MessageMember m = members->members_[i];

    rosidl_typesupport_introspection_c__MessageMembers * rec_members = NULL;
    if (m.type_id_ == 18) {
      const rosidl_message_type_support_t * introspection = get_message_typesupport_handle(
        m.members_, rosidl_typesupport_introspection_c__identifier);
      rec_members = (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;
    }

    snprintf(
      buffer, sizeof(buffer), "%s - %s [%s%s%s%s%s%s]%s\n",
      indent_buffer.data,
      m.name_,
      (m.is_array_) ? (m.array_size_ == 0) ? "Sequence of " : "Array of " : "",
      basic_type_names[m.type_id_],
      m.type_id_ == 18 ? " " : "",
      m.type_id_ == 18 ? rec_members->message_namespace_ : "",
      m.type_id_ == 18 ? "/" : "",
      m.type_id_ == 18 ? rec_members->message_name_ : "",
      m.type_id_ == 16 || ((m.is_array_) && (m.array_size_ == 0)) ? " <- This should be inited" : ""
    );

    *out = micro_ros_string_utilities_append(*out, buffer);

    if (m.type_id_ == 18) {
      print_type_info(rec_members, out, indent_buffer, level + 1);
    }
  }

  indent_buffer = micro_ros_string_utilities_remove_tail_chars(indent_buffer, 1);
}

rosidl_runtime_c__String micro_ros_utilities_type_info(
  const rosidl_message_type_support_t * type_support)
{
  rosidl_runtime_c__String ret = micro_ros_string_utilities_init("");

  const rosidl_message_type_support_t * introspection =
    get_message_typesupport_handle(
    type_support,
    rosidl_typesupport_introspection_c__identifier
    );

  const rosidl_typesupport_introspection_c__MessageMembers * members =
    (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;

  rosidl_runtime_c__String indent_buffer = micro_ros_string_utilities_init("");
  print_type_info(members, &ret, indent_buffer, 0);
  micro_ros_string_utilities_destroy(&indent_buffer);

  return ret;
}

typedef enum handle_message_memory_operation_t
{
  CREATE_OPERATION,
  CREATE_PREALLOCATED_OPERATION,
  DESTROY_OPERATION,
  CALCULATE_SIZE_OPERATION,
} handle_message_memory_operation_t;

size_t handle_message_memory(
  const rosidl_typesupport_introspection_c__MessageMembers * members,
  void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf,
  rosidl_runtime_c__String name_tree,
  handle_message_memory_operation_t operation)
{
  rcutils_allocator_t allocator =
    (conf.allocator == NULL) ?
    rcutils_get_default_allocator() :
    *conf.allocator;

  size_t used_memory = 0;

  for (size_t i = 0; i < members->member_count_; i++) {
    rosidl_typesupport_introspection_c__MessageMember m = members->members_[i];
    bool is_sequence = (m.is_array_) && (m.array_size_ == 0);

    if (conf.n_rules > 0) {
      name_tree = micro_ros_string_utilities_append(name_tree, m.name_);
      name_tree = micro_ros_string_utilities_append(name_tree, ".");
    }

    size_t sequence_size = 0;

    if (m.type_id_ == 16 || is_sequence) {
      uint8_t * aux_ptr = (uint8_t *) ros_msg;
      aux_ptr += m.offset_;
      generic_sequence_t * ptr = (generic_sequence_t *) aux_ptr;
      rosidl_typesupport_introspection_c__MessageMembers * rec_members = NULL;

      if (m.type_id_ == 18) {
        const rosidl_message_type_support_t * introspection = get_message_typesupport_handle(
          m.members_, rosidl_typesupport_introspection_c__identifier);
        rec_members = (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;
      }

      size_t member_size =
        (m.type_id_ == 18) ? rec_members->size_of_ : basic_types_size[m.type_id_];
      sequence_size = (m.type_id_ == 18) ? conf.max_ros2_type_sequence :
        (m.type_id_ == 16) ? conf.max_string_capacity :
        conf.max_basic_type_sequence;

      if (conf.n_rules > 0) {
        for (size_t i = 0; i < conf.n_rules; i++) {
          if (memcmp(conf.rules[i].rule, name_tree.data, strlen(conf.rules[i].rule)) == 0 &&
            memcmp(conf.rules[i].rule, name_tree.data, name_tree.size - 1) == 0)
          {
            sequence_size = conf.rules[i].size;
            break;
          }
        }
      }

      if (operation == CREATE_OPERATION) {
        ptr->data = allocator.zero_allocate(sequence_size, member_size, allocator.state);
        if (ptr->data == NULL) {
          return 0;
        }
        ptr->size = 0;
        ptr->capacity = sequence_size;
      } else if (operation == CREATE_PREALLOCATED_OPERATION) {
        ptr->data = get_static_memory(sequence_size * member_size);
        if (ptr->data == NULL) {
          return 0;
        }
        ptr->size = 0;
        ptr->capacity = sequence_size;
      }
      used_memory += sequence_size * member_size;
    }

    if (m.type_id_ == 18) {
      const rosidl_message_type_support_t * introspection = get_message_typesupport_handle(
        m.members_, rosidl_typesupport_introspection_c__identifier);
      rosidl_typesupport_introspection_c__MessageMembers * rec_members =
        (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;

      if ((m.type_id_ == 16 || m.type_id_ == 18) && is_sequence) {
        generic_sequence_t * ptr = (generic_sequence_t *)((uint8_t *)ros_msg + m.offset_);
        for (size_t i = 0; i < sequence_size; i++) {
          uint8_t * data =
            (operation ==
            CALCULATE_SIZE_OPERATION) ? NULL : (uint8_t *)ptr->data + (i * rec_members->size_of_);
          used_memory += handle_message_memory(rec_members, data, conf, name_tree, operation);
        }

      } else {
        used_memory += handle_message_memory(
          rec_members, ((uint8_t *)ros_msg + m.offset_), conf,
          name_tree, operation);
      }
    }

    if ((m.type_id_ == 16 || is_sequence) && operation == DESTROY_OPERATION) {
      generic_sequence_t * ptr = (generic_sequence_t *)((uint8_t *)ros_msg + m.offset_);
      allocator.deallocate(ptr->data, allocator.state);
      ptr->size = 0;
      ptr->capacity = 0;
    }

    if (conf.n_rules > 0) {
      name_tree = micro_ros_string_utilities_remove_tail_chars(name_tree, strlen(m.name_) + 1);
    }
  }

  return used_memory;
}

size_t micro_ros_utilities_get_dynamic_size(
  const rosidl_message_type_support_t * type_support,
  const micro_ros_utilities_memory_conf_t conf)
{
  const micro_ros_utilities_memory_conf_t actual_conf = get_configuration(conf);

  const rosidl_message_type_support_t * introspection =
    get_message_typesupport_handle(
    type_support,
    rosidl_typesupport_introspection_c__identifier
    );

  const rosidl_typesupport_introspection_c__MessageMembers * members =
    (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;

  rosidl_runtime_c__String name_tree = {0};

  if (conf.n_rules > 0) {
    name_tree = micro_ros_string_utilities_init("");
  }

  size_t size = handle_message_memory(
    members, NULL, actual_conf, name_tree,
    CALCULATE_SIZE_OPERATION);

  if (conf.n_rules > 0) {
    micro_ros_string_utilities_destroy(&name_tree);
  }

  return size;
}

bool micro_ros_utilities_create_message_memory(
  const rosidl_message_type_support_t * type_support,
  void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf)
{
  size_t calculated_size = micro_ros_utilities_get_dynamic_size(type_support, conf);

  const micro_ros_utilities_memory_conf_t actual_conf = get_configuration(conf);

  const rosidl_message_type_support_t * introspection =
    get_message_typesupport_handle(
    type_support,
    rosidl_typesupport_introspection_c__identifier
    );

  const rosidl_typesupport_introspection_c__MessageMembers * members =
    (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;

  rosidl_runtime_c__String name_tree = {0};

  if (conf.n_rules > 0) {
    name_tree = micro_ros_string_utilities_init("");
  }

  memset(ros_msg, 0, members->size_of_);
  size_t used_size = handle_message_memory(
    members, ros_msg, actual_conf, name_tree,
    CREATE_OPERATION);

  if (conf.n_rules > 0) {
    micro_ros_string_utilities_destroy(&name_tree);
  }

  return used_size == calculated_size;
}

bool micro_ros_utilities_create_static_message_memory(
  const rosidl_message_type_support_t * type_support,
  void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf,
  uint8_t * buffer,
  size_t buffer_len)
{
  size_t calculated_size = micro_ros_utilities_get_dynamic_size(type_support, conf);

  if (calculated_size > buffer_len) {
    return false;
  }

  operation_buffer = buffer;
  operation_buffer_index = 0;

  const micro_ros_utilities_memory_conf_t actual_conf = get_configuration(conf);

  const rosidl_message_type_support_t * introspection =
    get_message_typesupport_handle(
    type_support,
    rosidl_typesupport_introspection_c__identifier
    );

  const rosidl_typesupport_introspection_c__MessageMembers * members =
    (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;

  rosidl_runtime_c__String name_tree = {0};

  if (conf.n_rules > 0) {
    name_tree = micro_ros_string_utilities_init("");
  }

  memset(ros_msg, 0, members->size_of_);
  size_t used_size = handle_message_memory(
    members, ros_msg, actual_conf, name_tree,
    CREATE_PREALLOCATED_OPERATION);

  if (conf.n_rules > 0) {
    micro_ros_string_utilities_destroy(&name_tree);
  }

  return used_size == calculated_size;
}

bool micro_ros_utilities_destroy_message_memory(
  const rosidl_message_type_support_t * type_support,
  void * ros_msg,
  const micro_ros_utilities_memory_conf_t conf)
{
  size_t calculated_size = micro_ros_utilities_get_dynamic_size(type_support, conf);

  const micro_ros_utilities_memory_conf_t actual_conf = get_configuration(conf);

  const rosidl_message_type_support_t * introspection =
    get_message_typesupport_handle(
    type_support,
    rosidl_typesupport_introspection_c__identifier
    );

  const rosidl_typesupport_introspection_c__MessageMembers * members =
    (rosidl_typesupport_introspection_c__MessageMembers *)introspection->data;

  rosidl_runtime_c__String name_tree = {0};

  if (conf.n_rules > 0) {
    name_tree = micro_ros_string_utilities_init("");
  }

  size_t released_size = handle_message_memory(
    members, ros_msg, actual_conf, name_tree,
    DESTROY_OPERATION);
  memset(ros_msg, 0, members->size_of_);

  if (conf.n_rules > 0) {
    micro_ros_string_utilities_destroy(&name_tree);
  }

  return calculated_size == released_size;
}
