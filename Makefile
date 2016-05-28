SRC=indicator-net
all:
	g++ $(SRC).cpp -o $(SRC) -std=gnu++11 \
	`pkg-config --libs gtk+-3.0 --cflags gtk+-3.0` \
	`pkg-config --libs appindicator3-0.1 --cflags appindicator3-0.1`

install:
	install $(SRC) /usr/bin
	install $(SRC).desktop /usr/share/applications
	install $(SRC).desktop /etc/xdg/autostart

uninstall:
	rm -f /usr/bin/$(SRC)
	rm -f /usr/share/applications/$(SRC).desktop
	rm -f /etc/xdg/autostart/$(SRC).desktop

.PHONY: install uninstall
