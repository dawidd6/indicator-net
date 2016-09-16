#Assembled by dawidd6
CXX=g++
CXXFLAGS=-std=c++11 `pkg-config --libs appindicator3-0.1 --cflags appindicator3-0.1`
PROGRAM=indicator-net
SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:.cpp=.o)
START_COLOR=\033[0;33m
CLOSE_COLOR=\033[m

src/%.o: src/%.cpp
	@echo "$(START_COLOR)[CXX]$(CLOSE_COLOR)   $<"
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

bin/$(PROGRAM): $(OBJ)
	@echo "$(START_COLOR)[CXX]$(CLOSE_COLOR)   $@"
	@mkdir -p bin
	@$(CXX) -o $@ $^ $(CXXFLAGS)

install:
	@echo "$(START_COLOR)[IN]$(CLOSE_COLOR)   /usr/bin/$(PROGRAM)"
	@echo "$(START_COLOR)[IN]$(CLOSE_COLOR)   /usr/share/applications/$(PROGRAM).desktop"
	@echo "$(START_COLOR)[IN]$(CLOSE_COLOR)   /etc/xdg/autostart/$(PROGRAM).desktop"
	@install bin/$(PROGRAM) /usr/bin
	@install $(PROGRAM).desktop /usr/share/applications
	@install $(PROGRAM).desktop /etc/xdg/autostart

uninstall:
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   /usr/bin/$(PROGRAM)"
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   /usr/share/applications/$(PROGRAM).desktop"
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   /etc/xdg/autostart/$(PROGRAM).desktop"
	@rm -rf /usr/bin/$(PROGRAM)
	@rm -rf /usr/share/applications/$(PROGRAM).desktop
	@rm -rf /etc/xdg/autostart/$(PROGRAM).desktop

clean:
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   $(OBJ)"
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   bin"
	@rm -rf $(OBJ)
	@rm -rf bin

.PHONY: install uninstall clean
