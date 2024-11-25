CXXFLAGS = -g -D_REENTRANT

CUR_DIR = $(abspath .)

INCLUDES = -I$(CUR_DIR)/app \
           -I$(CUR_DIR)/app/display \
           -I$(CUR_DIR)/app/display/SDL \
           -I$(CUR_DIR)/core/cpu \
           -I$(CUR_DIR)/core/ppu \
           -I$(CUR_DIR)/core/apu \
           -I$(CUR_DIR)/core/loader \
           -I$(CUR_DIR)/core/memory \
           -I$(CUR_DIR)/demos \
           -I$(CUR_DIR)/include/imgui \
           -I$(CUR_DIR)/include \


SRC_DIRS = $(CUR_DIR)/core/cpu/*.cpp \
           $(CUR_DIR)/core/ppu/*.cpp \
           $(CUR_DIR)/core/memory/*.cpp \
           $(CUR_DIR)/core/loader/*.cpp \
           $(CUR_DIR)/app/*.cpp \
		       $(CUR_DIR)/app/display/*.cpp \
           $(CUR_DIR)/app/display/SDL/*.cpp \
           $(CUR_DIR)/demos/*.cpp \
           $(CUR_DIR)/imgui/*.cpp


ifeq ($(OS),Windows_NT)
    CXX = g++
    INCLUDES += -I$(CUR_DIR)/include/SDL2/include \
                -I$(CUR_DIR)/include/SDL2/include/SDL2
    LIBS = -L$(CUR_DIR)/include/SDL2/lib -lmingw32 -lSDL2main -lSDL2
    CXXFLAGS += -DWIN32
    TARGET = $(patsubst /,\,$(CUR_DIR))\bin\emu.exe # windows prefers \ over /
    CLEAN_CMD = del /f "$(TARGET)"
else
    CXX = /usr/bin/g++
    LIBS = -lSDL2
    INCLUDES += -I/usr/include/SDL2
    TARGET = $(CUR_DIR)/bin/emu
    CLEAN_CMD = rm -f $(TARGET)
endif

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC_DIRS) $(LIBS) -o $(TARGET)

clean:
	$(CLEAN_CMD)
