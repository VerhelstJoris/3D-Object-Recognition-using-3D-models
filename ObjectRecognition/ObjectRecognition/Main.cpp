

#include "System.h"


int main(void)
{

	System* systemObject;
	bool result;

	// Create the system object.
	systemObject = new System;
	if (!systemObject)
	{
		return 0;
	}

	result = systemObject->Initialize();
	if (result)
	{
		systemObject->Run();
	}

	// Shutdown and release the system object.
	systemObject->Shutdown();
	delete systemObject;
	systemObject = 0;

	return 0;
}

