#包含头文件
#include ../script/inc.Makefile
include  $(B_PROJECT_DIR_PATH)/script/inc.Makefile #指定交叉编译工具链和编译选项LDFLAGS和CFLAGS

#目标自动依赖
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

#可执行文件
BINS = $(TARGET)

all: $(OBJS) $(BINS)
	@$(STRIP) $(BINS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(BINS): $(OBJS)
	echo $(LDFLAGS)
#	@$(CC) -o $@ $^ $(LDFLAGS)
#	@$(STRIP) $(BINS)

	@$(CC) -o $@ $^ -Xlinker --start-group $(LDFLAGS) -Xlinker --end-group #-Xlinker --start-group $(LDFLAGS) -Xlinker --end-group:解决循环依赖项
	@$(STRIP) $(BINS)
	
	@cp $(BINS) ../application/

clean:
	@rm -f $(BINS) $(OBJS)

install:
	@cp -rf $(BINS) $(RELEASE_DIR_PATH)


.PHONY: all clean install


