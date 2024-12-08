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
		Emulator emulator(romPath);
		emulator.Run(); // Start the emulator.
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}