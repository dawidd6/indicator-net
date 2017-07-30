#Assembled by dawidd6
COMPILER=gcc
CFLAGS=-Wall -fPIC -std=c11 $(shell pkg-config --libs --cflags appindicator3-0.1)
PROGRAM=indicator-net
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
START_COLOR=\033[0;33m
CLOSE_COLOR=\033[m
DESTDIR=

src/%.o: src/%.c
	@echo "$(START_COLOR)[CC]$(CLOSE_COLOR)   $<"
	@$(COMPILER) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJ)
	@echo "$(START_COLOR)[LD]$(CLOSE_COLOR)   $@"
	@$(COMPILER) -o $@ $^ $(CFLAGS)

install:
	@echo "$(START_COLOR)[INSTALL]$(CLOSE_COLOR)   /usr/bin/$(PROGRAM)"
	@echo "$(START_COLOR)[INSTALL]$(CLOSE_COLOR)   /usr/share/applications/$(PROGRAM).desktop"
	@echo "$(START_COLOR)[INSTALL]$(CLOSE_COLOR)   /etc/xdg/autostart/$(PROGRAM).desktop"
	@install -d $(DESTDIR)/usr/bin
	@install -d $(DESTDIR)/usr/share/applications
	@install -d $(DESTDIR)/etc/xdg/autostart
	@install $(PROGRAM) $(DESTDIR)/usr/bin
	@install -m 644 $(PROGRAM).desktop $(DESTDIR)/usr/share/applications
	@install -m 644 $(PROGRAM).desktop $(DESTDIR)/etc/xdg/autostart

uninstall:
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   /usr/bin/$(PROGRAM)"
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   /usr/share/applications/$(PROGRAM).desktop"
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   /etc/xdg/autostart/$(PROGRAM).desktop"
	@rm -rf /usr/bin/$(PROGRAM)
	@rm -rf /usr/share/applications/$(PROGRAM).desktop
	@rm -rf /etc/xdg/autostart/$(PROGRAM).desktop

clean:
	@echo "$(START_COLOR)[RM]$(CLOSE_COLOR)   $(OBJ) $(PROGRAM)"
	@rm -rf $(OBJ) $(PROGRAM)

debian:
	debuild --no-tgz-check
	dh clean

.PHONY: install uninstall clean debian
