// Nagłówki
//#include "stdafx.h"
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/System/Time.hpp>

#define M_PI 3.14159265358979323846

#define WND_H 600
#define WND_W 800
#define VerticesLen 100


// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj; 
in vec3 position;
in vec3 color;
out vec3 Color;
void main(){
Color = color;
gl_Position = proj * view * model * vec4(position, 1.0);
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

// Widok
glm::mat4 view;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float sensitivity = 0.75f;

float lastX = 0;
float lastY = 0;

float cameraSpeed = 1.0f / 100;

float rotation = cameraSpeed;

void ustawKamereMysz(GLint uniView, sf::Int64 time, sf::Window& _window) 
{
	double yaw = -90; //obrót względem osi Y
	double pitch = 0; //obrót względem osi X

	sf::Vector2i localPosition = sf::Mouse::getPosition(_window);
	double xoffset = localPosition.x - lastX;
	double yoffset = localPosition.y - lastY;
	lastX = localPosition.x;
	lastY = localPosition.y;

	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch -= yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};



	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);



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

	auto primitive = GL_TRIANGLES;
	//int vertcount = 8;

	// Model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

	// Widok
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// Projekcja
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.06f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	window.setMouseCursorGrabbed(true); //przechwycenie kursora myszy w oknie ------------
	window.setMouseCursorVisible(false); //ukrycie kursora myszy ---------------------

	sf::Clock clock;
	sf::Time time;

	// Rozpoczęcie pętli zdarzeń
	bool running = true;
	while (running) {
		sf::Event windowEvent;
		time = clock.getElapsedTime();
		clock.restart();
		float cameraSpeed = 0.000002f * time.asMicroseconds();

		float fps = 1000000 / time.asMicroseconds();
		window.setTitle(std::to_string(fps));
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
				case sf::Keyboard::Left:
					cameraPos.x += cameraSpeed;
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;

				case sf::Keyboard::Right:
					cameraPos.x -= cameraSpeed;
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Keyboard::Up:
					cameraPos.y -= cameraSpeed;
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Keyboard::Down:
					cameraPos.y += cameraSpeed;
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Keyboard::Add:
					cameraPos += cameraSpeed * cameraFront;
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Keyboard::Subtract:
					cameraPos -= cameraSpeed * cameraFront;
					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Keyboard::Divide:
					rotation -= cameraSpeed;
					cameraUp -= cameraSpeed;

					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Keyboard::Multiply:
					rotation  += cameraSpeed;
					cameraUp += cameraSpeed;

					view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

					uniView = glGetUniformLocation(shaderProgram, "view");
					glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
					break;
				case sf::Event::MouseMoved:
					ustawKamereMysz(uniView, time.asMicroseconds(), window);
					break;
				}
			}
		}
		// Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Narysowanie figury
		glDrawArrays(primitive, 0, 36);
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