CXX = /usr/bin/g++

CXXFLAGS = -g -D_REENTRANT

INCLUDES = -I/home/pai/github/emu/app \
           -I/home/pai/github/emu/app/display \
           -I/home/pai/github/emu/app/display/SDL \
           -I/home/pai/github/emu/core/cpu \
           -I/home/pai/github/emu/core/ppu \
           -I/home/pai/github/emu/core/apu \
           -I/home/pai/github/emu/core/loader \
           -I/home/pai/github/emu/core/memory \
           -I/home/pai/github/emu/demos \
           -I/home/pai/github/emu/include/imgui \
           -I/home/pai/github/emu/include \
           -I/usr/include/SDL2

LIBS = -lSDL2

SRC_DIRS = /home/pai/github/emu/core/cpu/*.cpp \
           /home/pai/github/emu/core/ppu/*.cpp \
           /home/pai/github/emu/core/memory/*.cpp \
           /home/pai/github/emu/core/loader/*.cpp \
           /home/pai/github/emu/app/*.cpp \
		       /home/pai/github/emu/app/display/*.cpp \
           /home/pai/github/emu/app/display/SDL/*.cpp \
           /home/pai/github/emu/demos/*.cpp \
           /home/pai/github/emu/imgui/*.cpp

TARGET = /home/pai/github/emu/bin/emu

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC_DIRS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
