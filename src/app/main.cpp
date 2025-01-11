#include "pch.h"
#include "Emulator.h"

int main(int argc, char* argv[]) 
{
	/*if (argc < 2 || argv[1] == 0)
	{
		std::cerr << "Usage: <rom_path>" << std::endl;
		return 1;
	}

	std::string romPath = argv[1];*/

	/* // TODO: safe implementation of verbose arguments
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i] == "--verbose" || argv[i] == "-v")
			g_verboseLevel = (VerboseLevel)(std::stoi(argv[++i]));
	}*/
	g_verboseLevel = VerboseLevel::DEBUG;

	std::string romPath = "default.nes";

	try
	{
		ControllerHandler controllerHandler{};
		// Initiate the GUI.
		std::unique_ptr<EmulatorDisplay> GUI = std::make_unique<EmulatorDisplay>("Emulator", DISPLAY_WIDTH, DISPLAY_HEIGHT, 4, &controllerHandler);

		while (!romPath.empty())
		{
			// Start the emulator loop.
			Emulator emulator(romPath, *GUI);
			romPath = emulator.Run(); 
		}
	}
	catch (const std::exception& e)
	{
		LOG_ERROR(e.what());
		return 1;
	}

	return 0;
}