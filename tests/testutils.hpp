#pragma once

#include <iostream>

static int testsFailed = 0;

# define EXPECT(cond,description)										\
do {																	\
	if (!(cond)) {														\
	  std::cerr << "FAILED "											\
				<< description << std::endl <<"       " << #cond		\
				<< "\n"													\
				<< "       " << __FILE__ << ':' << __LINE__				\
				<< std::endl;											\
	  ++testsFailed;													\
	}																	\
} while (false)