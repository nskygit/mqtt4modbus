TARGET = mqtt4modbus
CC = gcc

CFLAGS = -Wall -pedantic -std=gnu11 -g -O2 -I /usr/local/include/modbus
LIBS = -L./lib/ -lm -lcJson -lcsv -lmosquitto -lmodbus 

.PHONY: default all clean

default: $(TARGET)
all: default

SUBDIRS = ./cJson ./csv
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(SUBDIRS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

$(SUBDIRS):ECHO
	+$(MAKE) -C $@
 
ECHO:
	@echo $(SUBDIRS)
	@echo begin compile

clean:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir clean||exit 1;\
	done
	-rm -f *.o
	-rm -f $(TARGET)
