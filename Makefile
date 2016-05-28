SRC=indicator-net
DESTDIR=

build:
	g++ $(SRC).cpp -o $(SRC) -std=c++11 -Wall \
	`pkg-config --libs appindicator3-0.1 --cflags appindicator3-0.1`

install:
	install $(SRC) $(DESTDIR)/usr/bin
	install $(SRC).desktop $(DESTDIR)/usr/share/applications
	install $(SRC).desktop $(DESTDIR)/etc/xdg/autostart

uninstall:
	rm -f /usr/bin/$(SRC)
	rm -f /usr/share/applications/$(SRC).desktop
	rm -f /etc/xdg/autostart/$(SRC).desktop

.PHONY: install uninstall
