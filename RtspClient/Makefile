#PC��CROSS���ڿ�
CROSS  =
CC     = $(CROSS)g++
AR     = $(CROSS)ar
STRIP  = $(CROSS)strip

EXE_NAME = RtspClient
PROJECT_ROOT = .

CFLAGS =-Wall -Wno-unused-parameter
# -Werror
LDFLAGS = -lm -lpthread

#STATIC_LIB = #$(PROJECT_ROOT)/lib/librtspcli.a
#LDFLAGS+=$(STATIC_IPC_LIB)


INCLUDE = -I$(PROJECT_ROOT)/src/include
INCLUDE += -I$(PROJECT_ROOT)/lib/include/jrtplib3
INCLUDE += -I$(PROJECT_ROOT)/src/base64/include

CFLAGS += $(INCLUDE)



SRC = $(wildcard $(PROJECT_ROOT)/src/*.cpp)
SRC += $(wildcard $(PROJECT_ROOT)/examples/*.cpp)
SRC += $(wildcard $(PROJECT_ROOT)/src/base64/*.cpp)
#��Makefile�����У�ͨ����ᱻ�Զ�չ�������ڱ����Ķ���ͺ�������ʱ��ͨ�����ʧЧ����������������Ҫͨ�����Ч������Ҫʹ�ú�����wildcard��  
#wildcard��ʾͨ�����Ч

#DYNAMIC_LIB_LIST+= ${RESOURCE_PATH}/lib/libsqlite3.so.0
#LDFLAGS+=$(DYNAMIC_LIB_LIST)


OBJ_DIR = $(PROJECT_ROOT)/obj
OBJ_FILE = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRC)))))
#notdirȥ���ļ�������Ŀ¼��Ϣ ����nodir src/123.cpp   ���ʣ��123.cpp
#basename ȥ�ļ���ǰ׺������basename 123.cpp  ����õ� 123
#addsuffix �Ӻ�׺������addsuffix .o 123,����õ� 123.o
#addprefix ��ǰ׺������addprefix ./obj/ 123.o,����õ�./obj/123.o
all: deps  $(EXE_NAME)


deps: $(SRC) Makefile
#���makeִ�е�����ǰ�����@�ַ�������ʾ�������ֻ��ʾ���Ľ��
	@echo "Generating new dependency file...";
	@-rm -f deps;
	@-mkdir $(PROJECT_ROOT)/obj;
#��Ϊ for���� shell ������������target������Ҫ�ټ���һ��$ , ȷ��shell���յ����� $target (makefile���Ȱ� $$target ����Ϊ $target, ����shell).
#��Ϊ��ÿһ�ж�����һ��������subshell��ִ�У����ԣ������Ҫ shell ����ʼ���ǿɼ��Ļ�����Ҫ��һ����б�ߣ���ʾ��Щ��������һ�� subshell��ִ�С�
#$f��ʾȡ����f��ֵ������f����forѭ����ı���
#sed -e s��ʾ�滻���/\.cpp/��ʾƥ��*.cpp��/\.o/��ʾ��ƥ���滻��*.o��
	
	@for f in $(SRC); do \
          OBJ=$(OBJ_DIR)/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.c/\.o/'`; \
          echo $$OBJ: $$f>> deps; \
          echo '	$(CC) $$(CFLAGS) -c -o $$@ $$^'>> deps; \
        done

-include ./deps
#ʹ�á�-include FILENAMES...��������ǣ������������ļ������ڻ��߲�����һ������ȥ��������make��������ִ��
#ͨ��makeִ�е����������������������˳�״̬��0����������ֹ������ִ�к���������������ǰ�����-�ţ���ʹ�������������makeҲ�����ִ�к�������


$(EXE_NAME): $(OBJ_FILE)
	$(CC) -rdynamic ${PROJECT_ROOT}/lib/libjrtp.so.3.11.1 $(LDFLAGS) $(CFLAGS) -o "$@" $(OBJ_FILE)
	$(STRIP) $(EXE_NAME)




clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(EXE_NAME)
	rm -rf deps