#����ͷ�ļ�
#include ../script/inc.Makefile
include  $(B_PROJECT_DIR_PATH)/script/inc.Makefile #ָ��������빤�����ͱ���ѡ��LDFLAGS��CFLAGS

#Ŀ���Զ�����
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

#��ִ���ļ�
BINS = $(TARGET)

all: $(OBJS) $(BINS)
	@$(STRIP) $(BINS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $< 

$(BINS): $(OBJS)
	echo $(LDFLAGS)
#	@$(CC) -o $@ $^ $(LDFLAGS)
#	@$(STRIP) $(BINS)

	@$(CC) -o $@ $^ -Xlinker --start-group $(LDFLAGS) -Xlinker --end-group #-Xlinker --start-group $(LDFLAGS) -Xlinker --end-group:���ѭ��������
	@$(STRIP) $(BINS)
	
	@cp $(BINS) ../application/

clean:
	@rm -f $(BINS) $(OBJS)

install:
	@cp -rf $(BINS) $(RELEASE_DIR_PATH)


.PHONY: all clean install


