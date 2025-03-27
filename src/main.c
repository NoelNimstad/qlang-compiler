/**
 * @name Qlang
*/

#include "qlang.h"

int main(int argc, char const *argv[])
{	
	ASSERT(argc >= 3, "Not enough arguments supplied.\n\tProper usage: qlang <IN.Q> <OUT>\n");

	unsigned char debugTokens, debugNodes;
	int i = 0;
	while(argv[i])
	{
        if(strcmp(argv[i], "--debugTokens") == 0)
		{
			debugTokens = 1;
        } else if(strcmp(argv[i], "--debugNodes") == 0)
		{
			debugNodes = 1;
		} else if(strcmp(argv[i], "--debug") == 0)
		{
			debugTokens = 1;
			debugNodes = 1;
		}
        i++;
    }

	processFile(argv[1], debugTokens, debugNodes); // Process main supplied file

	return EXIT_SUCCESS;
}