cc=gcc
rm=rm
cp=cp

OBJ=$(wildcard *.o)
FLAGS=-lcurl 
debug=-g

target=portal

$(target):$(target).o urlcode.o dumpToStrFromFILE.o inforecord.o regexmatch.o
	$(cc) -g -o  $@-`uname -m` $+ $(FLAGS) 
%.o:%.c
	$(cc) -g -c  $<
clean:
	$(rm) -f $(OBJ) $(target) 
install :
	$(cp) -f ./$(target)-`uname -m` /usr/local/bin/$(target)
uninstall:
	$(rm) -f /usr/local/bin/$(target) 
