CC = gcc
AR = ar
ARFLAG = -rcs

LIB_DIR = ./../lib/
TOP_DIR = ./..
 
SOURCES = $(wildcard ./*.c)
INCLUDE_DIRS = -I$(TOP_DIR)/include  -I$(TOP_DIR)/json -I./ 
 
TARGET = libcsv.a
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
 
$(TARGET) : $(OBJECTS)
	$(AR) $(ARFLAG) $@ $^
	cp $@ $(LIB_DIR)
	
$(OBJECTS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE_DIRS)
 
.PHONY : clean
clean:
	rm -rf *.o
	rm -rf $(TARGET)

