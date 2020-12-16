/*---------------------------------------------------------*/
/* ---------------   Proyecto Final  ----------------------*/
/*------------------    2021-1   --------------------------*/
/*-------- Alumno: Mario Horacio Garrido Czacki -----------*/
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include<time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>


//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);
// settings
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// camera
Camera camera(glm::vec3(0.0f, 4.0f, 30.0f));
float MovementSpeed = 0.3;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 120;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

GLuint VBO, VAO, EBO;

//Texture
unsigned int	t_pasto,
                t_acera,
				t_pavimento;

//Mapa
float mapscale = 1.0f;

unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}
float map_max_x = 588.0f;
float map_max_z = 725.0f;
void drawingGrass()
{
	float vertices[] = {
		// positions          // texture coords
		 map_max_x , 0.0f, 0.0f,   map_max_x, 0.0f, // top right
		 map_max_x , 0.0f, map_max_z ,   map_max_x, map_max_z, // bottom right
		0.0f, 0.0f, map_max_z ,   0.0f, map_max_z , // bottom left
		0.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // top left
	};

	unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3,  // second triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

float street_left_left = 364.0f;
float street_left_right = 368.0f;
float street_right_left = 376.0f;
float street_right_right = 380.f;
float street_height = 0.2;
float pavement_height = 0.1;
void drawingStreet()
{
	float vertices[] = {
		// positions          // texture coords
		 street_left_right,  street_height , map_max_z ,   street_left_right- street_left_left, 0.0f, // north right
		 street_left_right,   street_height , 0.0f, street_left_right - street_left_left, map_max_z, // south right
		 street_left_left ,   street_height , 0.0f, 0.0f, map_max_z, // south left
		 street_left_left ,   street_height , map_max_z , 0.0f, 0.0f,  // north left

		 street_left_left ,  street_height , map_max_z , street_height, 0.0f, //LEFT: top north
		 street_left_left ,   0.0f , map_max_z, 0.0f, 0.0f, // bottom north
		 street_left_left ,   0.0f , 0.0f , 0.0f, map_max_z, // bottom south
		 street_left_left ,   street_height , 0.0f , street_height, map_max_z,  // top south

		 street_left_right ,  street_height , map_max_z , street_height, 0.0f, //RIGHT: top north
		 street_left_right ,   0.0f , map_max_z, 0.0f, 0.0f, // bottom north
		 street_left_right ,   0.0f , 0.0f , 0.0f, map_max_z, // bottom south
		 street_left_right ,   street_height , 0.0f , street_height, map_max_z,  // top south

		 street_right_right,  street_height , map_max_z ,  street_right_right - street_right_left, 0.0f, // north right
		 street_right_right,   street_height , 0.0f, street_right_right - street_right_left, map_max_z, // south right
		 street_right_left ,   street_height , 0.0f, 0.0f, map_max_z, // south left
		 street_right_left ,   street_height , map_max_z , 0.0f, 0.0f,  // north left

		 street_right_left ,  street_height , map_max_z , street_height, 0.0f, //LEFT: top north
		 street_right_left ,   0.0f , map_max_z, 0.0f, 0.0f, // bottom north
		 street_right_left ,   0.0f , 0.0f , 0.0f, map_max_z, // bottom south
		 street_right_left ,   street_height , 0.0f , street_height, map_max_z,  // top south

		 street_right_right ,  street_height , map_max_z , street_height, 0.0f, //RIGHT: top north
		 street_right_right ,   0.0f , map_max_z, 0.0f, 0.0f, // bottom north
		 street_right_right ,   0.0f , 0.0f , 0.0f, map_max_z, // bottom south
		 street_right_right ,   street_height , 0.0f , street_height, map_max_z,  // top south

		 street_right_right,  pavement_height , map_max_z ,   street_right_left-street_left_right, 0.0f, //PAVEMENT: north right
		 street_right_right,   pavement_height , 0.0f, street_right_left - street_left_right, map_max_z, // south right
		 street_left_right ,   pavement_height , 0.0f, 0.0f, map_max_z, // south left
		 street_left_right ,   pavement_height , map_max_z , 0.0f, 0.0f,  // north left
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3,  // second triangle

		4, 5, 7, // first triangle
		5, 6, 7,  // second triangle

		8, 9, 11, // first triangle
		9, 10, 11,  // second triangle

		12, 13, 15, // first triangle
		13, 14, 15,  // second triangle

		16, 17, 19, // first triangle
		17, 18, 19,  // second triangle

		20, 21, 23, // first triangle
		21, 22, 23,  // second triangle

		24, 25, 27, // first triangle
		25, 26, 27  // second triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void drawingWall()
{
	float vertices[] = {
			// positions          // texture coords
			 0.5f,  0.5f, 0.0f,   0.5f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f,   0.5f, 0.66666f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.25f, 0.66666f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.25f, 1.0f,  // top left

			 0.5f,  0.5f, 0.0f,   0.25f, 0.66666f, // top right
			 0.5f, -0.5f, 0.0f,   0.25f, 0.33333f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.33333f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.0f, 0.66666f,  // top left

			 0.5f,  0.5f, 0.0f,   0.5f, 0.66666f, // top right
			 0.5f, -0.5f, 0.0f,   0.5f, 0.33333f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.25f, 0.33333f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.25f, 0.66666f,  // top left

			 0.5f,  0.5f, 0.0f,   0.75f, 0.66666f, // top right
			 0.5f, -0.5f, 0.0f,   0.75f, 0.33333f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.5f, 0.33333f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.5f, 0.66666f,  // top left

			 0.5f,  0.5f, 0.0f,   1.0f, 0.66666f, // top right
			 0.5f, -0.5f, 0.0f,   1.0f, 0.33333f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.75f, 0.33333f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.75f, 0.66666f,  // top left

			 0.5f,  0.5f, 0.0f,   0.5f, 0.33333f, // top right
			 0.5f, -0.5f, 0.0f,   0.5f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.25f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.25f, 0.33333f,  // top left
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3,  // second triangle

			4, 5, 7, // first triangle
			5, 6, 7,  // second triangle

			8, 9, 11, // first triangle
			9, 10, 11,  // second triangle

			12, 13, 15, // first triangle
			13, 14, 15,  // second triangle

			16, 17, 19, // first triangle
			17, 18, 19,  // second triangle

			20, 21, 23, // first triangle
			21, 22, 23  // second triangle
		};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
void LoadTextures()
{
	t_pasto = generateTextures("resources/texturas/pasto.jpg", 0);
	t_acera = generateTextures("resources/texturas/acera.jpg", 0);
	t_pavimento = generateTextures("resources/texturas/pavimento.jpg", 0);
}

void animate()
{

}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Final", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	//Shader staticShader("Shaders/lightVertex.vs", "Shaders/lightFragment.fs");
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader projectionShader("Shaders/shader_texture_color.vs", "Shaders/shader_texture_color.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	//Model casa1("resources/objects/casa1/casa1.obj");
	//Model casa2("resources/objects/casa2/casa2.obj");
	//Model casa3("resources/objects/casa3/casa3.obj");
	//Model casa4("resources/objects/casa4/casa4.obj");
	Model barrio("resources/objects/barrio/barrio.obj");
	//Cargando texturas

	LoadTextures();
	//Parámetros del mapa
	float escala_retícula = 1.0f;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);
		
		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.8f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(0.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, pavement_height, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
		
		//--------------------
		// Inicia Casa Real
		//--------------------
		staticShader.setVec3("aColor", 0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 200.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		barrio.Draw(staticShader);
		//casa1.Draw(staticShader);
		/*model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(400.0f, 0.0f, 200.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casa2.Draw(staticShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 400.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casa3.Draw(staticShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(400.0f, 0.0f, 400.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casa4.Draw(staticShader);*/
		// draw skybox as last
		// -------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::mat4(1.0f);
		projectionShader.use();
		projectionShader.setVec3("viewPos", camera.Position);
		projectionShader.setMat4("model", model);
		projectionShader.setMat4("view", view);
		projectionShader.setMat4("projection", projection);
		drawingGrass();
		projectionShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		projectionShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_pasto);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		drawingStreet();
		projectionShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		projectionShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_acera);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, t_pavimento);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(36 * sizeof(float)));
		//FALTAN BARDAS, BOSQUES, CAMINOS, ALBERCA
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}