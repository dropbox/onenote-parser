CPP_FILES := $(wildcard *.cpp)
C_FILES := $(wildcard *.c)
HPP_FILES := $(wildcard *.hpp)

one_note_to_json: $(CPP_FILES) $(HPP_FILES) $(C_FILES)
	g++ -o$@ -I. -std=c++11 $(LD_FLAGS) -g  $(CPP_FILES)  -ljansson 

