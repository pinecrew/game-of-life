PROG=game
CXX=g++
CFLAGS=$(shell sdl2-config --cflags) -std=c++11
LFLAGS=-Wall $(shell sdl2-config --libs) -lSDL2_image
SOURCES=src/game.cpp src/draw.cpp src/logics.cpp src/font.cpp

OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPS := $(patsubst %.o, %.d, $(OBJS))

$(PROG): $(OBJS) $(DEPS)
	$(CXX) $(OBJS) -o $@ $(LFLAGS)

%.o: %.cpp %.d
	$(CXX) $(CFLAGS) -c $< -o $@

%.d: %.cpp
	@set -e; $(CXX) -M $< | \
		sed -e 's%\($*\)\.o[ :]*%\1.o $@ : %g' > $@; \
		[ -s $@ ] || rm -f $@
	@echo create $@

clean:
	$(RM) $(PROG) $(OBJS) $(DEPS) *~

-include $(DEPS)
