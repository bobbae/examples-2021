default: dumqtt
CC=gcc
ifeq ($(OS),Windows_NT)
DEBUG=-g
INC=-I C:/WpdPack/Include -I.
CFLAGS=-std=c99 -DWIN32 $(INC) $(DEBUG)
LIBS=-L C:/Win10pcap/x64 -lwpcap -liphlpapi -lws2_32 
EXE=.exe
else
ifeq ($(shell uname),Linux)
DEBUG=-g
EXE=
LIBS=-lpcap
INC=-I /usr/local/include -I.
CFLAGS=-std=c99 -DLINUX $(INC) $(DEBUG)
endif
endif

OBJ1=dumqtt.o mqtt.o mqtt_pal.o

dumqtt: $(OBJ1)
	gcc  $(CFLAGS) -o dumqtt$(EXE) $(OBJ1)  $(LIBS)
