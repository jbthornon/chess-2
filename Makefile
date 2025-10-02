CC:=clang
CARGS:=
BINNAME=main
SRC:= $(wildcard ./*.c)

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
	rm -f $(BINNAME)
