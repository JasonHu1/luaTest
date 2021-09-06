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
    -I$(LOCAL_PATH)/include

APP_SDK_INC_FILES += $(LOCAL_PATH)/include/hal_comm.h \
    $(LOCAL_PATH)/include/luademo.h \
    $(LOCAL_PATH)/include/LuaScriptEngine.h \
    $(LOCAL_PATH)/include/Message.h \
    $(LOCAL_PATH)/include/random-test-server.h  \
    $(LOCAL_PATH)/include/third_format.h \
    $(LOCAL_PATH)/include/tuya_proto.h \
    $(LOCAL_PATH)/include/user_dev_exmaple_intf.h \
    $(LOCAL_PATH)/include/lua_modbus.h \
    $(LOCAL_PATH)/include/parson.h \
    $(LOCAL_PATH)/include/subdev.h \
    $(LOCAL_PATH)/include/time_scale.h


# 生成静态库
include $(BUILD_STATIC_LIBRARY)

# 生成动态库
include $(BUILD_SHARED_LIBRARY)

#---------------------------------------

