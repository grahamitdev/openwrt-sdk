CC:=gcc
#CFLAGS:=-Iinclude
CFLAGS:=-c

TARGET:=./helloworld	
DEPAND:=./helloworld.o

$(TARGET):$(DEPAND)
	$(CC) -o $@ $^

./%.o:./%.c
	$(CC) -o $@ $(CFLAGS) $^ -Wall

clean:
	rm *.o helloworld
.PHONY: clean





# helloworld : helloworld.o
# 	$(CC) $(LDFLAGS) helloworld.o -o helloworld
# helloworld.o : helloworld.c
# 	$(CC) $(CFLAGS) -c helloworld.c

# clean :
# 	rm *.o helloworld
