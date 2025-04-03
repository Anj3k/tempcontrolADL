CC = gcc
CFLAGS = -Wall -Wextra -O3 -flto
TARGET = tempcontrol
SRC = main.c
PREFIX = /usr
SBIN = $(PREFIX)/sbin
SYSTEMD = /lib/systemd/system

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

install: $(TARGET)
	install -m 755 $(TARGET) $(SBIN)/$(TARGET)
	install -m 644 tempcontrol.service $(SYSTEMD)/tempcontrol.service
	systemctl daemon-reload
	systemctl enable tempcontrol

uninstall:
	systemctl stop tempcontrol || true
	systemctl disable tempcontrol || true
	rm -f $(SBIN)/$(TARGET)
	rm -f $(SYSTEMD)/tempcontrol.service
	systemctl daemon-reload

clean:
	rm -f $(TARGET)
