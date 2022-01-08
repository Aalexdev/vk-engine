CXX = g++.exe
STD_VERSION = c++17
EXEC = prog
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)
OBJ = $(addprefix obj/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
LIB = libs/
INCLUDE = include/
CFLAGS = -l mingw32 -l glfw3 -lgdi32 -lvulkan-1
DEFINES = -D DEBUG

all : $(EXEC)

obj/%.o : src/%.cpp
	@echo compile $@ from $<
	@$(CXX) -std=$(STD_VERSION) -o $@ -c $< -I $(INCLUDE) $(DEFINES)

obj/%.o : src/*/%.cpp
	@echo compile $@ from $<
	@$(CXX) -std=$(STD_VERSION) -o $@ -c $< -I $(INCLUDE) $(DEFINES)

obj/%.o : src/*/*/%.cpp
	@echo compile $@ from $<
	@$(CXX) -std=$(STD_VERSION) -o $@ -c $< -I $(INCLUDE) $(DEFINES)

$(EXEC) : $(OBJ)
	@ $(CXX) -time -std=$(STD_VERSION) obj/*.o -I $(INCLUDE) -L $(LIB) -o $(EXEC) $(CFLAGS) $(DEFINES)
	@echo compiling finish with success

dbg : $(OBJ)
	$(CXX) -std=$(STD_VERSION) -g3 $(SRC) -I $(INCLUDE) -L $(LIB) -o $(EXEC) $(CFLAGS) $(DEFINES)

clear:
	@del obj\*.o

info:
	@echo -----------------------------------------------------
	@echo compiling info :                
	@echo 	compiler                     : $(CXX)
	@echo 	compiler standart version    : $(STD_VERSION)
	@echo 	include path                 : $(INCLUDE)
	@echo 	flags                        : $(FLAGS)
	@echo 	libs directory               : $(LIB)
	@echo 	executable output            : $(EXEC)
	@echo -----------------------------------------------------
