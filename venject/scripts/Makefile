CC		:= cc
CFLAGS 	:= -O2 -Wall -Wextra

TARGET 	:= asar_pack
SRC 	:= asar_pack.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
