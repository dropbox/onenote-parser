CPP_FILES := $(wildcard *.cpp)
C_FILES := $(wildcard *.c)
HPP_FILES := $(wildcard *.hpp)

OBJS := $(C_FILES:%.c=%.o)
%.o: %.c
	gcc  -I. -c $< -o $@

one_note_to_json: $(CPP_FILES) $(HPP_FILES) $(OBJS)
	g++ -o$@ -I. -std=c++11 $(LD_FLAGS) -g  $(CPP_FILES)  -ljansson $(OBJS)

