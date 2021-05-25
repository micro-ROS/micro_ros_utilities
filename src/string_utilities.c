#include <micro_ros_utilities/string_utilities.h>

#include <stddef.h>
#include <string.h>

#include <rcutils/allocator.h>

rosidl_runtime_c__String micro_ros_string_utilities_init(const char * data)
{
    const rcutils_allocator_t allocator = rcutils_get_default_allocator();

    rosidl_runtime_c__String ret;

    ret.size = strlen(data);
    ret.capacity = ret.size + 1;
    ret.data = allocator.zero_allocate(ret.capacity, 1, allocator.state);

    memcpy(ret.data, data, ret.size);

    return ret;
}

const char * micro_ros_string_utilities_get_c_str(rosidl_runtime_c__String str){
    return (const char *) str.data;
}


rosidl_runtime_c__String micro_ros_string_utilities_append(rosidl_runtime_c__String str, const char * data)
{
    const rcutils_allocator_t allocator = rcutils_get_default_allocator();

    rosidl_runtime_c__String ret = str;
    size_t append_size = strlen(data);

    if (append_size + ret.size > ret.capacity)
    {
        ret.data = allocator.reallocate(ret.data, ret.size + append_size + 1, allocator.state);
        ret.capacity = ret.size + append_size + 1;
    }

    memcpy(&ret.data[ret.size], data, append_size + 1);

    ret.size += append_size;

    return ret;
}

rosidl_runtime_c__String micro_ros_string_utilities_remove_tail_chars(rosidl_runtime_c__String str, const size_t n)
{
    rosidl_runtime_c__String ret = str;

    ret.size -= n;
    ret.data[ret.size] = '\0';

    return ret;
}


void micro_ros_string_utilities_destroy(rosidl_runtime_c__String * str)
{
    const rcutils_allocator_t allocator = rcutils_get_default_allocator();

    allocator.deallocate(str->data, allocator.state);
    str->data = NULL;
    str->size = 0;
    str->capacity = 0;
}
