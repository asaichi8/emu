CXX = /usr/bin/g++

CXXFLAGS = -g -D_REENTRANT

CUR_DIR = $(shell pwd)

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
           -I/usr/include/SDL2

LIBS = -lSDL2

SRC_DIRS = $(CUR_DIR)/core/cpu/*.cpp \
           $(CUR_DIR)/core/ppu/*.cpp \
           $(CUR_DIR)/core/memory/*.cpp \
           $(CUR_DIR)/core/loader/*.cpp \
           $(CUR_DIR)/app/*.cpp \
		       $(CUR_DIR)/app/display/*.cpp \
           $(CUR_DIR)/app/display/SDL/*.cpp \
           $(CUR_DIR)/demos/*.cpp \
           $(CUR_DIR)/imgui/*.cpp

TARGET = $(CUR_DIR)/bin/emu

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC_DIRS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
