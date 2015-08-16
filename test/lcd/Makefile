TARGET = lcd
LIBS = -lwiringPi -lwiringPiDev

.PHONY: lcd clean

all: $(TARGET)

$(TARGET): main.c
	gcc -O2 -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
