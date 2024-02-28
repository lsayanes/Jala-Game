
OUT 	:= Pong
CCFLAGS	:= -std=c++17 -Wall -pedantic --debug -g
CC		:= g++
MKDIR 	:= mkdir -p
SRC		:= ./*.cpp ../*.cpp ../../Framebuffer/*.cpp  ../../Components/*.cpp  ../../Framebuffer/Linux/*.cpp ../../Libs/*.cpp  ../../Util/*.c ../../Libs/tinyPTC/*.c
INC		:= -I. -I../ -I../../Components -I../../Framebuffer -I/../Libs -I../../Util -I../../Sys -I/usr/include/freetype2/ -I../../Libs/tinyPTC/
BIN		:= ../$(OUT)/bin/
OBJ		:= ../$(OUT)/obj/
LIBS	:= -lfreetype -lX11 -lXext -lXrandr

.PHONY : copy

build: $(BIN) $(OBJ) $(patsubst %.cpp, $(OBJ)%.o, $(wildcard $(SRC))) $(patsubst %.c, $(OBJ)%.o, $(wildcard *.c)) $(wildcard *.h)
	$(CC) -o $(BIN)$(OUT) $(patsubst %.cpp, $(OBJ)%.o, $(wildcard $(SRC))) $(patsubst %.c, $(OBJ)%.o, $(wildcard *.c)) $(LIBS)


$(OBJ)%.o: %.cpp
	$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) $(INC) -c -o $@ $<

$(OBJ)%.o: %.c
	$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) $(INC) -c -o $@ $<

$(BIN):
	$(MKDIR) $(BIN)

$(OBJ): copy
	$(MKDIR) $(OBJ)

copy:
	$(shell cp -R Resources ${BIN})

clean:
	$(shell rm -rf ../Framebuffer)
	$(shell rm -rf ../Libs)
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm -f *.o
