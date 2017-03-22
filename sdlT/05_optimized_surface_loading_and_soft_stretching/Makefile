#OBJS specifies which files to compile as part of the project
OBJS = 05_optimized_surface_loading_and_soft_stretching.cpp

##CC specifies which compiler were using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = 05_optimized_surface_loading_and_soft_stretching

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

