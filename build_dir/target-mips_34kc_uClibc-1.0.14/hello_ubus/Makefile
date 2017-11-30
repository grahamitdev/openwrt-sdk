# OUTPUT = hello_ubus
# OBJ = hello_ubus.o
# LIBS = -lm -lubus -lubox -lpthread -luci -ljson-c

# all: $(OUTPUT)

# $(OUTPUT): $(OBJ)
# 	@echo "...................................."
# 	@echo "CC = " $(CC)
# 	@echo "CFLAGS = " $(CFLAGS)
# 	@echo "LDFLAGS = " $(LDFLAGS)
# 	@echo "...................................."
# $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS) $(LDLIBS)


# %.o: %.c
# 	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c $^ $(LDLIBS) $(LIBS)


# clean:
# 	-rm $(OUTPUT) *.o




CC:=gcc
#CFLAGS:=-Iinclude
CFLAGS:=-c

TARGET:=./hello_ubus	
DEPAND:=./hello_ubus.o

$(TARGET):$(DEPAND)
	#$(CC) -o $@ $^ -lm -lubus -lubox -lpthread -luci -ljson-c
	$(CC) -o $@ $^ -lmosquitto -lssl -lcrypto -lcyassl -pthread -lubus -lubox -llua -lm  -luci -ljson-c -lsqlite3 -lblobmsg_json -ldl -lpthread

./%.o:./%.c
	$(CC) -o $@ $(CFLAGS) $^ -Wall

clean:
	rm *.o hello_ubus
.PHONY: clean