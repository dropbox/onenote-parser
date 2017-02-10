CPP_FILES := $(wildcard *.cpp)
HPP_FILES := $(wildcard *.hpp)

one_note_to_json: $(CPP_FILES) $(HPP_FILES)
	g++ -o$@ -std=c++11 $(LD_FLAGS) -g  $(CPP_FILES)  -ljansson -lb64

