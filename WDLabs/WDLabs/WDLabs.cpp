// Nagłówki
//#include "stdafx.h"
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <iostream>

// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core
in vec3 position;
in vec3 color;
out vec3 Color;
void main(){
Color = color;
gl_Position = vec4(position, 1.0);
}
)glsl";

const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color;
out vec4 outColor;
void main()
{
outColor = vec4(Color, 1.0);
}
)glsl";

void ErrorCheck(GLuint& shader, std::string ShaderName = "Unknown shader")
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	std::cout << ShaderName << " compile result: ";
	std::cout << (isCompiled == 0 ? "Fail" : "Ok") << "\n";

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> log(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &log[0]);

		for (auto i : log)
		{
			std::cout << i;
		}
		std::cout << "\n";
	}
}


int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	// Okno renderingu
	sf::Window window(sf::VideoMode(800, 600, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);

	// Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Utworzenie VAO (Vertex Array Object)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Utworzenie VBO (Vertex Buffer Object)
	// i skopiowanie do niego danych wierzchołkowych
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLfloat* vertices = new GLfloat[]{
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,

	-0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 6, vertices, GL_STATIC_DRAW);


	// Utworzenie i skompilowanie shadera wierzchołków
	GLuint vertexShader =
		glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	ErrorCheck(vertexShader, "Vertex Shader");

	// Utworzenie i skompilowanie shadera fragmentów
	GLuint fragmentShader =
		glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	ErrorCheck(fragmentShader, "Fragment Shader");

	// Zlinkowanie obu shaderów w jeden wspólny program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specifikacja formatu danych wierzchołkowych
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	auto primitive = GL_POLYGON;
	int vertcount = 8;

	// Rozpoczęcie pętli zdarzeń
	bool running = true;
	while (running) {
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent)) {
			switch (windowEvent.type) {
			case sf::Event::Closed:
				running = false;
				break;

			case sf::Event::KeyPressed:
				switch (windowEvent.key.code) {
				case sf::Keyboard::Escape: //Jezeli ESC to zamyka okno
					window.close();
					break;
				case sf::Keyboard::Num1:
					primitive = GL_POINTS;
					break;
				case sf::Keyboard::Num2:
					primitive = GL_LINES;
					break;
				case sf::Keyboard::Num3:
					primitive = GL_LINE_STRIP;
					break;
				case sf::Keyboard::Num4:
					primitive = GL_LINE_LOOP;
					break;
				case sf::Keyboard::Num5:
					primitive = GL_TRIANGLES;
					break;
				case sf::Keyboard::Num6:
					primitive = GL_TRIANGLE_STRIP;
					break;
				case sf::Keyboard::Num7:
					primitive = GL_TRIANGLE_FAN;
					break;
				case sf::Keyboard::Num8:
					primitive = GL_QUADS;
					break;
				case sf::Keyboard::Num9:
					primitive = GL_QUAD_STRIP;
					break;
				case sf::Keyboard::Num0:
					primitive = GL_POLYGON;
					break;
				}
			case sf::Event::MouseMoved:
				vertcount = windowEvent.mouseMove.y % 600 / 80;
				break;
			}
		}
		// Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Narysowanie figury
		glDrawArrays(primitive, 0, vertcount);
		// Wymiana buforów tylni/przedni
		window.display();
	}
	// Kasowanie programu i czyszczenie buforów
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	// Zamknięcie okna renderingu
	window.close();
	return 0;
}