#include "UserAPI.h"
#include "string.h"
#include "iostream.h"
#include "functional.h"
#include "algo.h"

int main() 
{

	int numbers[] = { 20,40,50,10,30 };
	sort(numbers, numbers + 5, std::greater<int>());
	char* space = " \n";	
	for (int i = 0; i<5; i++)
		std::cout << numbers[i] << space;
	//std::cout << std::endl;

	while (1);
	return 0;
	
}
	