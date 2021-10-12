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
	// Przekazanie GLFW wersji u¿ywanego profilu - CORE profile
	// Bêdziemy u¿ywaæ tylko nowej OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Utworzenie obiektu okna GLFW 800 na 800 px o nazwie "Okno OpenGL"
	GLFWwindow* window = glfwCreateWindow(800, 800, "Okno OpenGL", NULL, NULL);
	// Sprawdzanie b³êdów tworzenia okna
	if (window == NULL) 
	{
		std::cout << "B³¹d tworznia okna GLFW";
		glfwTerminate();
		return -1;
	}
	// Wstawienie okna do bie¿¹cego kontekstu
	glfwMakeContextCurrent(window);

	// G³ówna pêtla zdarzeñ
	while(!glfwWindowShouldClose(window))
	{
		// Zajmij siê wszystkimi zdarzeniami GLFW
		glfwPollEvents();
	}

	// Zamkniecie okna GLFW przed zakoñczeniem dia³ania programu
	glfwDestroyWindow(window);

	//Zakoñcz GLFW przed zakoñczeniem programu
	glfwTerminate();
	return 0;
}