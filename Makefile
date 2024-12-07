CXXFLAGS = -g -D_REENTRANT -std=c++17

CUR_DIR = $(abspath .)

ifeq ($(OS),Windows_NT)
    IS_WINDOWS = 1
else
    IS_WINDOWS = 0
endif

INCLUDES = -I$(CUR_DIR)/app \
			-I$(CUR_DIR)/app/display \
			-I$(CUR_DIR)/app/display/SDL \
			-I$(CUR_DIR)/core/cpu \
			-I$(CUR_DIR)/core/ppu \
			-I$(CUR_DIR)/core/ppu/registers \
			-I$(CUR_DIR)/core/apu \
			-I$(CUR_DIR)/core/loader \
			-I$(CUR_DIR)/core/memory \
			-I$(CUR_DIR)/demos \
			-I$(CUR_DIR)/include/imgui \
			-I$(CUR_DIR)/include

SRC_DIRS = $(CUR_DIR)/core/cpu \
			$(CUR_DIR)/core/ppu \
			$(CUR_DIR)/core/memory \
			$(CUR_DIR)/core/loader \
			$(CUR_DIR)/app \
			$(CUR_DIR)/app/display \
			$(CUR_DIR)/app/display/SDL \
			$(CUR_DIR)/demos \
			$(CUR_DIR)/imgui

SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp)) # expand src dirs
OBJ_DIR = $(CUR_DIR)/bin/obj
# map each source to an object
OBJECTS = $(patsubst $(CUR_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

ifeq ($(OS),Windows_NT)
	CXX = g++
	INCLUDES += -I$(CUR_DIR)/include/SDL2/include \
				-I$(CUR_DIR)/include/SDL2/include/SDL2
	LIBS = -L$(CUR_DIR)/include/SDL2/lib -lmingw32 -lSDL2main -lSDL2
	CXXFLAGS += -DWIN32
	TARGET = $(patsubst /,\,$(CUR_DIR))\bin\emu.exe # windows prefers \ over /
	CLEAN_CMD = del /f "$(TARGET)" $(OBJ_DIR)\*.o
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Darwin) # macos	
		CXX = clang++
		INCLUDES += -I/usr/local/include/SDL2
		LIBS = -I/usr/local/lib -lSDL2
	else
		CXX = /usr/bin/g++
		INCLUDES += -I/usr/include/SDL2
		LIBS = -lSDL2
	endif
	TARGET = $(CUR_DIR)/bin/emu
	CLEAN_CMD = rm -f $(TARGET) $(OBJ_DIR)/*.o
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJECTS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: $(CUR_DIR)/%.cpp
ifeq ($(IS_WINDOWS), 1)
	if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
else
	mkdir -p $(dir $@)
endif
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(CLEAN_CMD)

rebuild: clean all
