PROG=game
CXX=g++
CFLAGS=
LFLAGS=-Wall -lSDL2
SOURCES=game.cpp

OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPS := $(patsubst %.o, %.d, $(OBJS))

$(PROG): $(OBJS) $(DEPS)
	$(CXX) $(OBJS) -o $@ $(LFLAGS)

%.o: %.cpp %.d
	$(CXX) $(CFLAGS) -c $< -o $@

%.d: %.cpp
	@set -e; $(CC) -M $< | \
		sed -e 's%\($*\)\.o[ :]*%\1.o $@ : %g' > $@; \
		[ -s $@ ] || rm -f $@
	@echo create $@

clean:
	$(RM) $(PROG) $(OBJS) $(DEPS) *~

-include $(DEPS)