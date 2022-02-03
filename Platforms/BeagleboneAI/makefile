TARGET = test

CXX = g++
CFLAGS = -g

OUTDIR = ./bin
DATADIR = ./data
SUBDIR = vm vm/vmfunc tinyxml2
DIR_OBJ = ./obj

INCS = $(wildcard *.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRCS = $(wildcard *.cpp $(foreach fd, $(SUBDIR), $(fd)/*.cpp))
NODIR_SRC = $(notdir $(SRCS))
OBJS = $(addprefix $(DIR_OBJ)/, $(SRCS:cpp=o)) # obj/xxx.o obj/folder/xxx .o
INC_DIRS = -I./ $(addprefix -I, $(SUBDIR))
#LIBS = -largp
#LIB_DIRS = -L/usr/local/Cellar/argp-standalone/1.3/lib

PHONY := $(TARGET)
$(TARGET): $(OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $@ $(OBJS) $(LIB_DIRS) $(LIBS)

$(DIR_OBJ)/%.o: %.cpp $(INCS)
	mkdir -p $(@D)
	$(CXX) -o $@ $(CFLAGS) -c $< $(INC_DIRS)

PHONY += clean
clean:
	rm -rf $(OUTDIR)/* $(DATADIR)/* $(DIR_OBJ)/*

PHONY += echoes
echoes:
    @echo "INC files: $(INCS)"
    @echo "SRC files: $(SRCS)"
    @echo "OBJ files: $(OBJS)"
    @echo "LIB files: $(LIBS)"
    @echo "INC DIR: $(INC_DIRS)"
    @echo "LIB DIR: $(LIB_DIRS)"

.PHONY = $(PHONY)