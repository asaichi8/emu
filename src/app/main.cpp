#include "Emulator.h"

int main(int argc, char* argv[]) 
{
	if (argc < 2 || argv[1] == 0)
	{
		std::cerr << "Usage: <rom_path>" << std::endl;
		return 1;
	}

	std::string romPath = argv[1];

	try
	{
		// Initiate the GUI.
		std::unique_ptr<EmulatorDisplay> GUI = std::make_unique<EmulatorDisplay>("Emulator", DISPLAY_WIDTH, DISPLAY_HEIGHT, 4);

		while (!romPath.empty())
		{
			// Start the emulator loop.
			Emulator emulator(romPath, *GUI);
			romPath = emulator.Run(); 
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}