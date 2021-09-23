# 当前文件所在目录
LOCAL_PATH := $(call my-dir)

#---------------------------------------

# 清除 LOCAL_xxx 变量
include $(CLEAR_VARS)

# 当前模块名
LOCAL_MODULE := $(notdir $(LOCAL_PATH))

# 模块源代码
LOCAL_SRC_FILES := $(shell find $(LOCAL_PATH)/src -name "*.c" -o -name "*.cpp" -o -name "*.cc")

# 模块的 CFLAGS
LOCAL_CFLAGS := \
    -Werror \
    -I$(LOCAL_PATH)/include

APP_SDK_INC_FILES += $(LOCAL_PATH)/include/modbus.h \
    $(LOCAL_PATH)/include/modbus-version.h \
    $(LOCAL_PATH)/include/modbus-private.h \
    $(LOCAL_PATH)/include/modbus-rtu.h \
    $(LOCAL_PATH)/include/modbus-rtu-private.h  \
    $(LOCAL_PATH)/include/modbus-tcp.h \
    $(LOCAL_PATH)/include/modbus-version.h \
    $(LOCAL_PATH)/include/modbus-tcp-private.h

# 生成静态库
include $(BUILD_STATIC_LIBRARY)

# 生成动态库
include $(BUILD_SHARED_LIBRARY)

#---------------------------------------

