PROJECT_ROOT:=$(shell pwd)

###设置编译器##
#cross=arm-hisiv300-linux-
CC=$(cross)gcc
CPP=$(cross)g++
AR=$(cross)ar

#####设置编译参数######
CFLAGS = -g -Wall -O2 
ifdef cross
CFLAGS += -fPIC -ffunction-sections -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -mno-unaligned-access -fno-aggressive-loop-optimizations
endif
######设置库参数######
#LDFLAGS =  -lpthread
LDFLAGS =  

#静态库名称#
LIB_NAME :=libzbase.a
#动态库名称#
LIB_SO := libzbase.so
#库和头文件所在目录#
LIB_DIR :=./lib

#中间文件所在目录#
OBJECT_DIR:=./objects
DEPFILE:=./deps

#头文件 #
INCLUDES = -I$(PROJECT_ROOT)/include

#源文件#
SRCS:=$(wildcard *.cpp)
SRCS+=$(wildcard *.c)

OBJECT_FILE:=$(addprefix $(OBJECT_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRCS)))))

.PHONY:lib clean

lib:$(DEPFILE) libs
$(DEPFILE): $(SRCS) 
	@echo "Generating new dependency file...";
	@-rm -f $(DEPFILE)
	@for f in $(SRCS); do \
	OBJ=$(OBJECT_DIR)/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.c/\.o/'`; \
          echo $$OBJ: $$f>> $(DEPFILE); \
          echo '	$(CC) $(CFLAGS) $(INCLUDES)  -c -o $$@ $$^ $(LDFLAGS)'>> $(DEPFILE); \
        done
-include $(DEPFILE)

libs:$(OBJECT_FILE)
	@[ -e $(LIB_DIR) ] || mkdir $(LIB_DIR)
	@[ -e $(LIB_DIR)/include ] || mkdir -p $(LIB_DIR)/include
	@cp -f $(PROJECT_ROOT)/include/* $(LIB_DIR)/include/
	$(AR) -rcu $(LIB_DIR)/$(LIB_NAME) $(OBJECT_FILE)
	$(CC) -fPIC -shared -o $(LIB_DIR)/$(LIB_SO) $(OBJECT_FILE)
	
clean:
	$(RM) -f $(DEPFILE)
	$(RM) -f $(OBJECT_DIR)/*
	$(RM) -rf $(LIB_DIR)/*
	
