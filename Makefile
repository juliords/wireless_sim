CPATH=-I.

# .c, .h, .S
RESOURCES=main.c

SRC=$(filter %.c, ${RESOURCES})
HDR=$(filter %.h, ${RESOURCES})
OBJ=${SRC:.c=.o}

CFLAGS= -O2 -Wall -g $(CPATH)
LDFLAGS=-O2 -lm -lpthread
EXE=main

.PHONY: all test clean
all: $(EXE)
zip: Makefile $(SRC) $(HDR)
	zip -r mypackage.zip $^

test: $(EXE)
	./$<

$(EXE): .depend.mk $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(EXE)

$(OBJ): %.o: %.c

.depend.mk: $(RESOURCES) Makefile
	$(CC) -MM $(CFLAGS) $(SRC) > .depend.mk

clean:
	rm -f $(EXE) $(OBJ)

-include .depend.mk
