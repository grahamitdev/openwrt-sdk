

MY_LIBS = -L./ -lshuncomzigbee

MY_INCLUDES =  -I$(CURDIR)/include/  -I$(CURDIR)/

export MY_CFLAGS += -lmosquitto -lssl -lcrypto -lcyassl -pthread -lubus -lubox -llua -lm  -luci -ljson-c -lsqlite3 -lblobmsg_json -ldl ${CFLAGS} -fPIC ${MY_INCLUDES} -g

APP_NAME=zigbeeservice


#=================================================================================
#								Main entrance
#=================================================================================
all: $(APP_NAME)



APP_OBJECTS =  zigbeeservice.o ubus.o zha_strategy.o cJSON.o mqtt.o aes.o

$(APP_OBJECTS) : %.o : %.c
	$(CC) -c $(MY_CFLAGS) $(MY_INCLUDES) $(DEFS) $< -o $@ 

$(APP_NAME): $(APP_OBJECTS) 
	#$(CC) -o $(APP_NAME)  $(MY_CFLAGS) *.o $(MY_LIBS)    
	$(CC) -o $@  $(MY_CFLAGS) $^ $(MY_LIBS)             

clean:
	@rm -f *.o
.PHONY: clean

