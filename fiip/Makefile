INCLUDES := ./DataFormat ./Link ./lib  #头文件目录
SUBDIRS  := ./DataFormat ./Link ./lib  #子目录

MYFLAGS  += -Wall -g
MYFLAGS  += -lpthread
MYFLAGS  += $(addprefix -I,$(INCLUDES))
CFLAGS   += -std=c11
CPPFLAGS += -std=c++11

CPPSRCS := $(wildcard *.cpp) $(wildcard $(addsuffix /*.cpp, $(SUBDIRS)))
CPPOBJS := $(patsubst %.cpp,%.o,$(CPPSRCS)) 
CSRCS := $(wildcard *.c ) $(wildcard $(addsuffix /*.c, $(SUBDIRS)))
COBJS := $(patsubst %.c,%.o,$(CSRCS))

all:$(CPPOBJS) $(COBJS)

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $^ -o $(OBJS_DIR)/$(notdir $@) $(MYFLAGS)
$(COBJS): %.o: %.c
	$(CC)  $(CFLAGS)   -c $^ -o $(OBJS_DIR)/$(notdir $@) $(MYFLAGS)
