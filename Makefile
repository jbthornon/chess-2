CC:=clang
CARGS:=-O3
BINNAME=main
SRC:= $(wildcard src/*.c)

OBJ:=$(SRC:.c=.o)
DEPS:=$(SRC:.c=.d)
# $^ = dependancies
# $< = first depandancy
# $@ = target

$(BINNAME):$(OBJ)
	$(CC) $(CARGS) $^ -o $@

%.o:%.c Makefile
	$(CC) -MMD $(CARGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJ)
	rm -f $(DEPS)
	rm -f $(BINNAME)
