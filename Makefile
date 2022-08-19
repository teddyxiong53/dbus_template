.PHONY: all clean myservice_server myservice_client myservice_cmd libcjson prepare libmyservice

all: prepare libcjson myservice_server myservice_client myservice_cmd libcjson
CFLAGS := -I./cjson `pkg-config --cflags   glib-2.0` -I/usr/include/gio-unix-2.0
LDFLAGS := -L./ -lcjson -lmyservice `pkg-config --libs glib-2.0` \
	-lgobject-2.0 -lgio-2.0

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
prepare:
	if [ ! -f myservice.c ]; then \
		gdbus-codegen  --interface-prefix=myipc --generate-c-code=myservice_gdbus myservice.xml; \
	fi
libmyservice: myservice_gdbus.o
	ar -crv  libmyservice.a $<
libcjson: cjson/cJSON_port.o cjson/cJSON_util.o cjson/cJSON.o
	ar -crv  libcjson.a $<

myservice_server:myservice_server.o 
	gcc -o $@ $< $(LDFLAGS) 
myservice_client:myservice_client.o 
	gcc -o $@ $< $(LDFLAGS) 
myservice_cmd:myservice_cmd.o 
	gcc -o $@ $< $(LDFLAGS) 
clean:
	-rm -rf *.o myservice_server myservice_client myservice_cmd
	-rm -rf cjson/*.o
	-rm -rf *.a