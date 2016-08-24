# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := /mnt/share/zeng/zbaselib/

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_MODULE    := zbase
LOCAL_SRC_FILES := zbaselib.c \
	zbaselib_buffer.c \
	zbaselib_filemap.c \
	zbaselib_list.c \
	zbaselib_md5.c \
	zbaselib_queue.c \
	zbaselib_rbtree.c \
	zbaselib_socket.c \
	zbaselib_stack.c \
	zbaselib_thread.c \
	zbaselib_array.c


include $(BUILD_SHARED_LIBRARY)
