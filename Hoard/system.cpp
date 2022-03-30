

#include "system.h"

double prevTime = 0.0;
double currTime = 0.0;


// ---------------------------------------------------------------------------
// Initialize GLFW, GLEW, Input, Create window

int SystemInit(int width, int height, const char* title){

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.2 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// vsync (0 = off, 1 = on) try changing this and see the frame rate
	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	printf("System succesfully initialize\n");

}

void SystemShutdown(){
	
	printf("Bye Bye\n");
	glfwTerminate();

}



// ---------------------------------------------------------------------------
// Get frame duration between each frame

void FrameInit(){
	prevTime = glfwGetTime();
}

double FrameStart(){
	currTime = glfwGetTime();
	double result = currTime - prevTime;
	return result;
}

void FrameEnd(){
	prevTime = currTime;
}