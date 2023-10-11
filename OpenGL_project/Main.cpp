#include <Windows.h>
#include <vector>
#include <string>
#include "Game.h"

int main() 
{
	Game game("TEST",
		1280, 720,
		4, 4,
		false);

	//game.startCollision();

	/*std::vector<int> arr = std::vector<int>();

	arr.push_back(34);

	std::string st = std::string("pidor");*/

	//MAIN LOOP
	while(!game.getWindowShouldClose())
	{
		//std::cout << "gdfg\n";
		game.update();
		game.render();
		/*SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (char*)"D:/testImg.bmp",
			SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);*/
		
	}

	return 0;
}