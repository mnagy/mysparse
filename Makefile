.PHONY: all clean

BISON = bison
OBJS = condate.tab.o main.o tree-check.o tree-match.o

all: mysparse
clean:
	rm -f *.o condate.tab.c

mysparse: $(OBJS)
	$(CC) $(OBJS) -lsparse

condate.tab.o: condate.tab.c

condate.tab.c: condate.y
	$(BISON) $(BISON_FLAGS) $<
