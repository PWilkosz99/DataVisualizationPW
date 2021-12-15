// Nagłówki
//#include "stdafx.h"
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/System/Time.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image.h"


#define M_PI 3.14159265358979323846

#define WND_H 600
#define WND_W 800
#define VerticesLen 100


// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core

in vec3 position;
in vec2 aTexCoord;
in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj; 
out vec2 TexCoord;
out vec3 FragPos;  
out vec3 Normal;

void main()
{
	gl_Position = proj * view * model * vec4(position, 1.0);
	TexCoord = aTexCoord;
    Normal = aNormal;
    FragPos = vec3(model * vec4(position, 1.0));

}
)glsl";

const GLchar* fragmentSource = R"glsl(
#version 150 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightPos;
uniform float power;
uniform float lightType;
uniform sampler2D texture1;
out vec4 outColor;

void main()
{
	if(lightType == 0.0)
	{
		vec3 lightColor = vec3(1.0, 1.0, 1.0);
		vec3 ambient = power * lightColor;
		outColor = vec4(ambient, 1.0) * texture(texture1, TexCoord);
	}
	else
	{
		vec3 lightColor = vec3(1.0, 1.0, 1.0);
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos); 
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;
		vec4 ambient = power * vec4(lightColor, 1.0);
		outColor = (ambient + vec4(diffuse, 1.0)) * texture(texture1, TexCoord);
	}
}
)glsl";

GLuint shaderProgram;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraZoom = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
static float lightPower = 0.5f;
static float lightType = 1.0f;


bool firstMouse = true;
int lastX, lastY;

int vertSize;

//kąty do kontroli nachylenia kamery
double yaw = -90;
double pitch = 0;
float rotation = 0;

unsigned int texture1; //Jak do wszystkich obiektów w OpenGL do tekstury można odwołać się poprzez identyfikator ID // texture 1


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

void setCam(GLint _uniView, float _time) {
	float cameraSpeed = 0.000002f * _time; //predkosc kamery

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		cameraPos.y -= cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		cameraPos.y += cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		cameraPos.x += cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		cameraPos.x -= cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraZoom)) * cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraZoom)) * cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Divide)) {
		rotation -= cameraSpeed;
		cameraZoom -= cameraSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply)) {
		rotation += cameraSpeed;
		cameraZoom += cameraSpeed;
	}

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraZoom);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void setCameraMouse(GLint _uniView, float _time, const sf::Window& _window) {
	// Aktualna pozycja kursora
	sf::Vector2i localPos = sf::Mouse::getPosition(_window);
	sf::Vector2i pos;
	bool rel = false;
	// Poziom
	if (localPos.x <= 0) {
		pos.x = _window.getSize().x;
		pos.y = localPos.y;
		rel = true;
	}
	if (localPos.x >= _window.getSize().x - 1) {
		pos.x = 0;
		pos.y = localPos.y;
		rel = true;
	}
	// Pion
	if (localPos.y <= 0) {
		pos.x = localPos.x;
		pos.y = _window.getSize().y - 1;
		rel = true;
	}
	if (localPos.y >= _window.getSize().y - 1) {
		pos.x = localPos.x;
		pos.y = 0;
		rel = true;
	}

	if (rel) {
		sf::Mouse::setPosition(pos, _window);
		firstMouse = true;
	}
	localPos = sf::Mouse::getPosition(_window);
	// Próba uniknięcia błędu związanego z zbyt dużym przeskokiem pozycji kursora
	if (firstMouse) {
		lastX = localPos.x;
		lastY = localPos.y;
		firstMouse = false;
	}
	// Zmiana położenia i zapamiętanie ostatniej pozycji
	double xoff = localPos.x - lastX;
	double yoff = localPos.y - lastY;
	lastX = localPos.x;
	lastY = localPos.y;

	double sensitivity = 0.05f;
	double speed = 0.000002f * _time;

	//aktualizacja kątów ustawienia kamery
	xoff *= sensitivity;
	yoff *= sensitivity;
	yaw += xoff;
	pitch -= yoff;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraZoom);
	glUniformMatrix4fv(_uniView, 0, GL_FALSE, glm::value_ptr(view));
}

void setLight(glm::vec3 pos, float power, float type = 1.0f)
{
	GLint unilightPos = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3fv(unilightPos, 1, &pos[0]);

	lightPower = power;
	lightPos = pos;
	lightType = type;

	GLint PowerPos = glGetUniformLocation(shaderProgram, "power");
	glUniform1f(PowerPos, power);

	GLint lightTypePos = glGetUniformLocation(shaderProgram, "lightType");
	glUniform1f(lightTypePos, type);
}

void loadTexture() {
	glGenTextures(1, &texture1); // Generuje tekstury
	glBindTexture(GL_TEXTURE_2D, texture1); //Ustawienie tekstury jako bieżącej (powiązanie)
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("metal.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

bool loadOBJ(const char* path, std::vector < glm::vec3 >& out_vertices, std::vector < glm::vec2 >& out_uvs, std::vector < glm::vec3 >& out_normals)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}
}

int main()
{
	srand((unsigned)time(NULL));

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
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};

	int points_ = 6;

	// Załadowanie tekstury
	loadTexture();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Read our .obj file
	std::vector< glm::vec3 > verticesOBJ;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.
	bool res = loadOBJ("ChairAndTable.obj", verticesOBJ, uvs, normals);
	vertSize = verticesOBJ.size();

	glBufferData(GL_ARRAY_BUFFER, verticesOBJ.size() * sizeof(glm::vec3), &verticesOBJ[0], GL_STATIC_DRAW);

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
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specifikacja formatu danych wierzchołkowych
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	auto primitive = GL_TRIANGLES;

	// Specyfikacja informacji o wektorach normalnych określając specyfikację formatu danych wierzchołkowych
	GLint NorAttrib = glGetAttribLocation(shaderProgram, "aNormal");
	glEnableVertexAttribArray(NorAttrib);
	glVertexAttribPointer(NorAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// Tekstura
	GLint TexCoord = glGetAttribLocation(shaderProgram, "aTexCoord");
	glEnableVertexAttribArray(TexCoord);
	glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	// Model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

	// Widok
	GLint uniView = glGetUniformLocation(shaderProgram, "view");

	// Projekcja
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.06f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	// Swiatlo
	GLint unilightPos = glGetUniformLocation(shaderProgram, "lightPos");

	// Przechwycenie kursora myszy w oknie
	window.setMouseCursorGrabbed(true);
	// Ukrycie kursora myszy
	window.setMouseCursorVisible(false);

	//FPS limit
	window.setFramerateLimit(144);
	sf::Clock clock;
	sf::Time time;

	// Ustawienie tekstury na bierzaca
	//glBindTexture(GL_TEXTURE_2D, texture1);

	glm::vec3 NewPosition;

	// Rozpoczęcie pętli zdarzeń
	bool running = true;
	while (running) {
		sf::Event windowEvent;
		time = clock.getElapsedTime();
		clock.restart();

		int ms = time.asMicroseconds();
		float fps = 0;
		if (ms > 0.001) {
			fps = 1000000 / time.asMicroseconds();
		}
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
				case sf::Keyboard::P:
					setLight(lightPos, lightPower + 0.1, lightType);
					break;
				case sf::Keyboard::L:
					setLight(lightPos, lightPower - 0.1, lightType);
					break;
				case sf::Keyboard::O:
					setLight(lightPos, lightPower, 1.0);
					break;
				case sf::Keyboard::K:
					setLight(lightPos, lightPower, 0.0);
					break;
				case sf::Keyboard::T:
					NewPosition = lightPos;
					NewPosition.y += 0.5f;
					setLight(NewPosition, lightPower, lightType);
					break;
				case sf::Keyboard::G:
					NewPosition = lightPos;
					NewPosition.y -= 0.5f;
					setLight(NewPosition, lightPower, lightType);
					break;
				case sf::Keyboard::H:
					NewPosition = lightPos;
					NewPosition.x += 0.5f;
					setLight(NewPosition, lightPower, lightType);
					break;
				case sf::Keyboard::F:
					NewPosition = lightPos;
					NewPosition.x -= 0.5f;
					setLight(NewPosition, lightPower, lightType);
					break;
				case sf::Keyboard::U:
					NewPosition = lightPos;
					NewPosition.z += 0.5f;
					setLight(NewPosition, lightPower, lightType);
					break;
				case sf::Keyboard::J:
					NewPosition = lightPos;
					NewPosition.z -= 0.5f;
					setLight(NewPosition, lightPower, lightType);
					break;
				}
			case sf::Event::MouseMoved:
				setCameraMouse(uniView, time.asMicroseconds(), window);
				break;
			}
		}
		//Wywolanie funkcji do kamery
		setCam(uniView, time.asMicroseconds());

		// Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Narysowanie figury
		glDrawArrays(primitive, 0, vertSize);
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