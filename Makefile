APP_BIN_NAME=modbus_server
USER_SW_VER=1.0.0

ROOT_DIR ?= $(shell pwd)

#COMPILE_PREX ?=~/toolchains/gcc-linaro-6.2.1-2016.11/bin/arm-linux-gnueabihf-
COMPILE_PREX ?=
USER_LINK_FLAGS ?=
#LIB_DIR ?= $(abspath ../tuya_sdk_library/ty_gw_zigbee_ext_sdk_gcc-linaro-621-201611_3.0.1-beta.263)
LIB_DIR ?= $(abspath ../tuya_sdk_library/ty_gw_zigbee_ext_sdk_linux-ubuntu-6.2.0_64Bit_3.0.1-beta.266)


-include ./build/build_param

$(info ROOT_DIR=$(ROOT_DIR))
$(info LIB_DIR=$(LIB_DIR))
$(info COMPILE_PREX=$(COMPILE_PREX))

AR = $(COMPILE_PREX)ar
CC = $(COMPILE_PREX)gcc
NM = $(COMPILE_PREX)nm
CPP = $(COMPILE_PREX)g++
OBJCOPY = $(COMPILE_PREX)objcopy
OBJDUMP = $(COMPILE_PREX)objdump

$(info AR=$(AR))
$(info CC=$(CC))
$(info NM=$(NM))
$(info CPP=$(CPP))
$(info OBJCOPY=$(OBJCOPY))
$(info OBJDUMP=$(OBJDUMP))

APP_PACK = ./build/pack.sh

LINKFLAGS = \
        -L$(LIB_DIR)/sdk/lib -ltuya_gw_ext_sdk -pthread -lm
#LINKFLAGS += -L./library/lib -lmodbus -llua

LINKFLAGS += $(USER_LINK_FLAGS)
LINKFLAGS += `pkg-config --libs libmodbus`

CCFLAGS = \
	-g -fPIC -MMD
CCFLAGS += `pkg-config --cflags libmodbus`

CPPFLAGS += -std=c++11

DEFINES = -DAPP_BIN_NAME=\"$(APP_BIN_NAME)\" \
		  -DUSER_SW_VER=\"$(USER_SW_VER)\" \
		  -DLUA_COMPAT_5_3

CFLAGS = $(CCFLAGS) $(DEFINES) $(USER_INCS) $(USER_CFLAGS)

#编译所用变量
OUTPUT_DIR = $(ROOT_DIR)/output/$(APP_BIN_NAME)_$(USER_SW_VER)
OUTPUT_DIR_OBJS = $(OUTPUT_DIR)/.objs

#user app基准路径
USER_SRC_BASE_DIR = ./src
USER_SRC_BASE_DIR += ./3rd-party

USER_INC_BASE_DIR = ./include
USER_INC_BASE_DIR += ./src
USER_INC_BASE_DIR += ./3rd-party

USER_INC_BASE_DIR += ./library/include

USER_INC_BASE_DIR += $(shell find $(LIB_DIR)/sdk -name include -type d)

#user app 编译文件
USER_SRC_DIRS = $(shell find $(USER_SRC_BASE_DIR) -type d)
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.c)) 
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.cpp)) 
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.s)) 
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.S)) 
#user头文件
USER_INCS = $(addprefix -I ,  $(shell find $(USER_INC_BASE_DIR) -type d) )

#user的obj命令
USER_OBJS = $(addsuffix .o, $(USER_SRCS))
#user的实际obj地址
USER_OBJS_OUT =  $(addprefix $(OUTPUT_DIR_OBJS)/, $(USER_OBJS))

DEPENDS := $(addsuffix .d,$(USER_SRCS))

all: pack

pack: build_app
ifeq ($(APP_PACK),$(wildcard $(APP_PACK)))
	sh $(APP_PACK) $(OUTPUT_DIR) $(ROOT_DIR) $(APP_BIN_NAME) $(USER_SW_VER) $(COMPILE_PREX)
endif

build_app: $(USER_OBJS_OUT)
	@$(CPP) $(USER_OBJS_OUT) $(CFLAGS) $(LINKFLAGS) -o $(OUTPUT_DIR)/$(APP_BIN_NAME)
	@echo "Build APP Finish"

$(OUTPUT_DIR_OBJS)/%.c.o: %.c
	@mkdir -p $(dir $@);
	@$(CC) $(CFLAGS) -o $@ -c $< 
	@echo "目标文件..$@"
	@echo "依赖文件..$^"
	
$(OUTPUT_DIR_OBJS)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@);
	@$(CPP) $(CFLAGS) $(CPPFLAGS) -o $@ -c $< 

$(OUTPUT_DIR_OBJS)/%.s.o: %.s
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -o $@ -c $< 

$(OUTPUT_DIR_OBJS)/%.S.o: %.S
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $< 


.PHONY: all clean SHOWARGS build_app pack
clean:
	rm -rf $(OUTPUT_DIR)
	
-include $(DEPENDS)
