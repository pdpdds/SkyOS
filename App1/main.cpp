#include "UserAPI.h"
#include "string.h"
#include "stl\iostream.h"
#include "stl\functional.h"
#include "stl\tinystl\algo.h"

int main() 
{

	int numbers[] = { 20,40,50,10,30 };
	sort(numbers, numbers + 5, std::greater<int>());
	for (int i = 0; i<5; i++)
		std::cout << numbers[i] << ' ';
	std::cout << '\n';
	return 0;
	
}
	