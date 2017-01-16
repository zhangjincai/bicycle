
#����Ŀ¼
#PROJECT_DIR_PATH = /home/dengjs/dowork/BicycleProject

RELEASE_DIR_PATH = $(B_PROJECT_DIR_PATH)/application #�������ļ���װĿ¼
LIBRARY_DIR_PATH  = $(B_PROJECT_DIR_PATH)/library
INCLUDE_DIR_PATH = $(B_PROJECT_DIR_PATH)/include

#�������ѡ��  -D_GNU_SOURCE  �ǽ��ȱ��pthread_rwlock_t����
LDFLAGS = -lpthread -lm -lstdc++ -ldl -lrt   #-lrt: signalfd, timerfd, eventfd #-ldlѡ���ʾ���ɵĶ���ģ����Ҫʹ�ù����
CFLAGS = -Wall -g -O0 -D_GNU_SOURCE #Linux���ź���/��д���ļ����б��룬��Ҫ�ڱ���ѡ����ָ��-D_GNU_SOURCE

#���������
CC = arm-none-linux-gnueabi-gcc
AR = arm-none-linux-gnueabi-ar
STRIP = arm-none-linux-gnueabi-strip #��strip����ļ�������������Ϣ,���ڼ�С��ִ���ļ��Ĵ�С


INCLUDE_DIR += -I $(INCLUDE_DIR_PATH)/

#�õ��ľ�̬��
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

LDFLAGS += $(STATIC_LIB_DIR) #�����·��
CFLAGS += $(INCLUDE_DIR) #ͷ�ļ�����·��






