#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

int main() {
	// Inicjalizacja GLFW
	glfwInit();

	// Przekazanie GLFW informacji o uzywanej wersji OpenGL
	// W tym wypadku OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Przekazanie GLFW wersji u�ywanego profilu - CORE profile
	// B�dziemy u�ywa� tylko nowej OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Utworzenie obiektu okna GLFW 800 na 800 px o nazwie "Okno OpenGL"
	GLFWwindow* window = glfwCreateWindow(800, 800, "Okno OpenGL", NULL, NULL);
	// Sprawdzanie b��d�w tworzenia okna
	if (window == NULL) 
	{
		std::cout << "B��d tworznia okna GLFW";
		glfwTerminate();
		return -1;
	}
	// Wstawienie okna do bie��cego kontekstu
	glfwMakeContextCurrent(window);

	// G��wna p�tla zdarze�
	while(!glfwWindowShouldClose(window))
	{
		// Zajmij si� wszystkimi zdarzeniami GLFW
		glfwPollEvents();
	}

	// Zamkniecie okna GLFW przed zako�czeniem dia�ania programu
	glfwDestroyWindow(window);

	//Zako�cz GLFW przed zako�czeniem programu
	glfwTerminate();
	return 0;
}