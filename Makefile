CXX = g++
CFLAGS = $(shell sdl2-config --cflags) -std=c++11
LFLAGS = -Wall $(shell sdl2-config --libs) -lSDL2_image

ifeq ($(RELEASE), 1)
	LFLAGS+=-O3
endif

target_file  := main
prog_name    := game-of-life
dest_dir     := ./src/
object_files := $(patsubst %.cpp, %.o, $(wildcard $(dest_dir)*.cpp))

all: $(dest_dir)$(target_file)

$(dest_dir)$(target_file): $(object_files)
	$(CXX) $(object_files) -o $(prog_name) $(LFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(prog_name) $(object_files)
