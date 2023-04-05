
OUT 	:= fbApp
CCFLAGS	:= -std=c++17 -Wall -pedantic --debug
CC		:= g++
MKDIR 	:= mkdir -p
SRC		:= ./*.cpp ../Framebuffer/*.cpp  ../Framebuffer/Linux/*.cpp ../Libs/*.cpp ../Util/*.c  
INC		:= -I. -I../Components -I../Framebuffer -I../Libs -I../Util -I/home/parallels/freetype2/include
BIN		:= ./bin/
OBJ		:= ./obj/
LIBS	:= -lfreetype -lX11 -lXext 


#build: $(BIN) $(OBJ) $(patsubst %.cpp, $(OBJ)%.o, $(wildcard $(SRC))) $(wildcard *.h)
#	$(CC) -o $(BIN)$(OUT) $(patsubst %.cpp, $(OBJ)%.o, $(wildcard $(SRC))) $(LIBS)

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


$(OBJ):
	$(MKDIR) $(OBJ)

	
clean: 
	$(shell rm -rf ./Framebuffer)
	$(shell rm -rf ./Libs)
	rm -rf $(BIN)
	rm -rf $(OBJ)

