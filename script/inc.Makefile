
#工程目录
#PROJECT_DIR_PATH = /home/dengjs/dowork/BicycleProject

RELEASE_DIR_PATH = $(B_PROJECT_DIR_PATH)/application #二进制文件安装目录
LIBRARY_DIR_PATH  = $(B_PROJECT_DIR_PATH)/library
INCLUDE_DIR_PATH = $(B_PROJECT_DIR_PATH)/include

#定义编译选项  -D_GNU_SOURCE  是解决缺少pthread_rwlock_t问题
LDFLAGS = -lpthread -lm -lstdc++ -ldl -lrt   #-lrt: signalfd, timerfd, eventfd #-ldl选项，表示生成的对象模块需要使用共享库
CFLAGS = -Wall -g -O0 -D_GNU_SOURCE #Linux下信号量/读写锁文件进行编译，需要在编译选项中指明-D_GNU_SOURCE

#交叉编译器
CC = arm-none-linux-gnueabi-gcc
AR = arm-none-linux-gnueabi-ar
STRIP = arm-none-linux-gnueabi-strip #被strip后的文件不包含调试信息,用于减小可执行文件的大小


INCLUDE_DIR += -I $(INCLUDE_DIR_PATH)/

#用到的静态库
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_general.a 
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_crypto.a 
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_eventloop.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_zmalloc.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/libmtd.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/libsqlite3.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_blacklist.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_update.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_gps.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_ctos.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_list.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_wireless.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_blacklist.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_threadpool.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_logdb.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_firmware.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_watchdog.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/libcurl.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/libusb-1.0.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/libftdi.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_sn.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_upgrade.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_unity_file.a
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/lib_network_check.a 
STATIC_LIB_DIR += $(LIBRARY_DIR_PATH)/liblnt.a

LDFLAGS += $(STATIC_LIB_DIR) #库查找路径
CFLAGS += $(INCLUDE_DIR) #头文件查找路径






