OBJDIR = build
targets = cache client libnetfiles netfileserver test
properTargets = $ (addsuffix .o, $(addprefix $(OBJDIR)/, $(targets)))

.PHONY: dirs
dirs:
make clean
mkdir $(OBJDIR)
make all

all: $(targets)
       gcc $(properTargets) test.c -o $(OBJDIR)/demo

cache:
gcc -c $@.c -o $(OBJDIR)/$@.o
netfileserver:
gcc -c $@.c -o $(OBJDIR)/$@.o
clean:
rm -rf $(OBJDIR)
