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

	// std::string romPath = "tests/nestest2.nes";
	std::string romPath = "default.nes";

	try
	{
		ControllerHandler controllerHandler{};
		// Initiate the GUI.
		std::unique_ptr<EmulatorDisplay> GUI = std::make_unique<EmulatorDisplay>("Emulator", DISPLAY_WIDTH*2, DISPLAY_HEIGHT*2, 2, &controllerHandler);

		while (!romPath.empty())
		{
			// Start the emulator loop.
			Emulator emulator(romPath, *GUI);
			romPath = emulator.Run(); 
		}
	}
	catch (const std::exception& e)
	{
		LOG_FATAL(e.what());
		return 1;
	}

	return 0;
}