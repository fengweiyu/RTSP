#PC上CROSS等于空
CROSS  =
CC     = $(CROSS)g++
AR     = $(CROSS)ar
STRIP  = $(CROSS)strip

EXE_NAME = RtspServer
PROJECT_ROOT = .

CFLAGS =-g -Wall -Wno-unused-parameter
# -Werror -lrt time相关函数
LDFLAGS = -lm -lpthread -lrt

#STATIC_LIB = #$(PROJECT_ROOT)/lib/librtspcli.a
#LDFLAGS+=$(STATIC_IPC_LIB)


INCLUDE = -I$(PROJECT_ROOT)/src/include
#INCLUDE += -I$(PROJECT_ROOT)/lib/include/jrtplib3
INCLUDE += -I$(PROJECT_ROOT)/src/base64/include

CFLAGS += $(INCLUDE)



SRC = $(wildcard $(PROJECT_ROOT)/src/*.cpp)
SRC += $(wildcard $(PROJECT_ROOT)/examples/*.cpp)
SRC += $(wildcard $(PROJECT_ROOT)/src/base64/*.cpp)
#在Makefile规则中，通配符会被自动展开。但在变量的定义和函数引用时，通配符将失效。这种情况下如果需要通配符有效，就需要使用函数“wildcard”  
#wildcard表示通配符有效

#DYNAMIC_LIB_LIST+= ${RESOURCE_PATH}/lib/libsqlite3.so.0
#LDFLAGS+=$(DYNAMIC_LIB_LIST)


OBJ_DIR = $(PROJECT_ROOT)/obj
OBJ_FILE = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRC)))))
#notdir去除文件的所有目录信息 例：nodir src/123.cpp   结果剩下123.cpp
#basename 去文件名前缀，例：basename 123.cpp  结果得到 123
#addsuffix 加后缀，例：addsuffix .o 123,结果得到 123.o
#addprefix 加前缀，例：addprefix ./obj/ 123.o,结果得到./obj/123.o
all: deps  $(EXE_NAME)


deps: $(SRC) Makefile
#如果make执行的命令前面加了@字符，则不显示命令本身而只显示它的结果
	@echo "Generating new dependency file...";
	@-rm -f deps;
	@-mkdir $(PROJECT_ROOT)/obj;
#因为 for属于 shell 命令，所以这里的target变量需要再加上一个$ , 确保shell接收到的是 $target (makefile会先把 $$target 处理为 $target, 传给shell).
#因为，每一行都是在一个单独的subshell里执行，所以，如果想要 shell 变量始终是可见的话，需要加一个反斜线，表示这些命令是在一个 subshell里执行。
#$f表示取变量f的值搜索，f是在for循环里的变量
#sed -e s表示替换命令，/\.cpp/表示匹配*.cpp，/\.o/表示把匹配替换成*.o，
	
	@for f in $(SRC); do \
          OBJ=$(OBJ_DIR)/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.c/\.o/'`; \
          echo $$OBJ: $$f>> deps; \
          echo '	$(CC) $$(CFLAGS) -c -o $$@ $$^'>> deps; \
        done

-include ./deps
#使用“-include FILENAMES...”的情况是，当所包含的文件不存在或者不存在一个规则去创建它，make程序会继续执行
#通常make执行的命令如果出错（该命令的退出状态非0）就立刻终止，不再执行后续命令，但如果命令前面加了-号，即使这条命令出错，make也会继续执行后续命令

#$(LDFLAGS) 必须放到后面不然编译不过
$(EXE_NAME): $(OBJ_FILE)
	$(CC) $(CFLAGS) -o "$@" $(OBJ_FILE) $(LDFLAGS)
	$(STRIP) $(EXE_NAME)




clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(EXE_NAME)
	rm -rf deps
