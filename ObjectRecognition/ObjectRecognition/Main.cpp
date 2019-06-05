

#include "OpenGL/OGLRenderer.h"

#include <vector>
#include <opencv2/core.hpp>

int main(void)
{
	//OPENGL object renderer
	OGLRenderer* systemObject;
	bool result;

	// Create the system object.
	systemObject = new OGLRenderer;
	if (!systemObject)
	{
		return 0;
	}

	result = systemObject->Initialize();
	if (result)
	{
		systemObject->Run();
	}
	//retrieve the vector of cv::Mat screenshots from the renderer
	std::vector<cv::Mat> m_screenShots = systemObject->GetScreenRenders();



	// Shutdown and release the OPENGL object.
	systemObject->Shutdown();
	delete systemObject;
	systemObject = 0;

	return 0;
}

