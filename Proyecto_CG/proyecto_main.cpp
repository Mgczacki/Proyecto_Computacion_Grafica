/*---------------------------------------------------------*/
/* ---------------   Proyecto Final  ----------------------*/
/*------------------    2021-1   --------------------------*/
/*-------- Alumno: Mario Horacio Garrido Czacki -----------*/
#include <Windows.h>

#include <iostream>
#include <cmath>
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


//Utilidad para obtener el ángulo absoluto de un vector en un plano 2D.
constexpr float radToDeg(float rad) { return rad * (180.0f / M_PI); }
constexpr float degToRad(float deg) { return deg * (M_PI / 180.0f); }
float vectorAngle(float x, float y) {
	if (x == 0.0f) // special cases
		return (y > 0.0f) ? 90.0f
		: (y == 0.0f) ? 0.0f
		: 270.0f;
	else if (y == 0.0f) // special cases
		return (x >= 0.0f) ? 0.0f
		: 180.0f;
	float ret = radToDeg(atanf((float)y / x));
	if (x < 0.0f && y < 0.0f) // quadrant Ⅲ
		ret = 180.0f + ret;
	else if (x < 0.0f) // quadrant Ⅱ
		ret = 180.0f + ret; // it actually substracts
	else if (y < 0.0f) // quadrant Ⅳ
		ret = 270.0f + (90.0f + ret); // it actually substracts
	return ret;
}
//Código obtenido de https://stackoverflow.com/questions/6247153/angle-from-2d-unit-vector

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
float speedMultiplier = 0.3;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
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

//Variables para animacion
float rotor_rotacion = 0.0f;

// KeyFrame para Pong
float	pong_l_pos = 0.0f,
		pong_r_pos = 0.0f,
		pong_b_xpos = 0.0f,
		pong_b_ypos = 0.0f;
float	pong_l_pos_inc = 0.0f,
		pong_r_pos_inc = 0.0f,
		pong_b_xpos_inc = 0.0f,
		pong_b_ypos_inc = 0.0f;

// KeyFrame para la pelota
float	pel_x = 0.0f,
pel_y = 0.0f,
pel_z = 0.0f,
pel_rot_y = 0.0f;
float	pel_x_inc = 0.0f,
pel_y_inc = 0.0f,
pel_z_inc = 0.0f,
pel_rot_y_inc = 0.0f;

#define MAX_FRAMES_PONG 7

#define MAX_FRAMES_PELOTA 13

int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct pong_frame
{
	//Variables para GUARDAR Key Frames
	float	pong_l_pos = 0.0f,
			pong_r_pos = 0.0f,
			pong_b_xpos = 0.0f,
			pong_b_ypos = 0.0f;

}PONG_FRAME;

PONG_FRAME KeyFramePong[MAX_FRAMES_PONG];
int FrameIndexPong = 7;			//introducir datos
int playIndexPong = 0;

typedef struct pelota_frame
{
	//Variables para GUARDAR Key Frames
	float	pel_x = 0.0f,
		pel_y = 0.0f,
		pel_z = 0.0f,
		pel_rot_y = 0.0f;

}PELOTA_FRAME;

PELOTA_FRAME KeyFramePelota[MAX_FRAMES_PELOTA];
int FrameIndexPelota = 13;			//introducir datos
int playIndexPelota = 0;

/*
Frame Index = 0                                                                                                                                                                                      KeyFramePong[0].pong_l_pos = 2.9;                                                                                                                                                                        KeyFramePong[0].pong_r_pos = 0;                                                                                                                                                                          KeyFramePong[0].pong_b_xpos = -0.5;                                                                                                                                                                      KeyFramePong[0].pong_b_ypos = 0;                                                                                                                                                                         Frame Index = 1                                                                                                                                                                                      KeyFramePong[1].pong_l_pos = 8.7;                                                                                                                                                                        KeyFramePong[1].pong_r_pos = 2.5;                                                                                                                                                                        KeyFramePong[1].pong_b_xpos = 15.5;                                                                                                                                                                      KeyFramePong[1].pong_b_ypos = 8.8;                                                                                                                                                                       Frame Index = 2                                                                                                                                                                                      KeyFramePong[2].pong_l_pos = 5.4;                                                                                                                                                                        KeyFramePong[2].pong_r_pos = 5.7;                                                                                                                                                                        KeyFramePong[2].pong_b_xpos = -0.5;                                                                                                                                                                      KeyFramePong[2].pong_b_ypos = 6.5;                                                                                                                                                                       Frame Index = 3                                                                                                                                                                                      KeyFramePong[3].pong_l_pos = -0.0999999;                                                                                                                                                                 KeyFramePong[3].pong_r_pos = 3.8;                                                                                                                                                                        KeyFramePong[3].pong_b_xpos = 15.5;                                                                                                                                                                      KeyFramePong[3].pong_b_ypos = -0.0999999;
*/

//Animación robot
float robot_pos_x = 204.127f,
robot_pos_z = 290.874f,
robot_rot_y = 0.0f;

//Animación avión
float avion_pos_x = 0.0f,
	avion_pos_y = 0.0f,
	avion_pos_z = 0.0f,
	avion_rot = 0.0f;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndexPelota << std::endl;
	std::cout << "KeyFramePelota[" << FrameIndexPelota << "].pel_x = " << pel_x << ";" << std::endl;
	std::cout << "KeyFramePelota[" << FrameIndexPelota << "].pel_y = " << pel_y << ";" << std::endl;
	std::cout << "KeyFramePelota[" << FrameIndexPelota << "].pel_z = " << pel_z << ";" << std::endl;
	std::cout << "KeyFramePelota[" << FrameIndexPelota << "].pel_rot_y = " << pel_rot_y << ";" << std::endl;
	KeyFramePelota[FrameIndexPong].pel_x = pel_x;
	KeyFramePelota[FrameIndexPong].pel_y = pel_y;
	KeyFramePelota[FrameIndexPong].pel_z = pel_z;
	KeyFramePelota[FrameIndexPong].pel_rot_y = pel_rot_y;
	FrameIndexPelota++;
}

void resetPelotaElements(void)
{
	pel_x = KeyFramePelota[0].pel_x;
	pel_y = KeyFramePelota[0].pel_y;
	pel_z = KeyFramePelota[0].pel_z;
	pel_rot_y = KeyFramePelota[0].pel_rot_y;
}
void resetPongElements(void)
{
	pong_l_pos = KeyFramePong[0].pong_l_pos;
	pong_r_pos = KeyFramePong[0].pong_r_pos;
	pong_b_xpos = KeyFramePong[0].pong_b_xpos;
	pong_b_ypos = KeyFramePong[0].pong_b_ypos;
}

void interpolationPong(void)
{
	pong_l_pos_inc = (KeyFramePong[playIndexPong + 1].pong_l_pos - KeyFramePong[playIndexPong].pong_l_pos) / i_max_steps;
	pong_r_pos_inc = (KeyFramePong[playIndexPong + 1].pong_r_pos - KeyFramePong[playIndexPong].pong_r_pos) / i_max_steps;
	pong_b_xpos_inc = (KeyFramePong[playIndexPong + 1].pong_b_xpos - KeyFramePong[playIndexPong].pong_b_xpos) / i_max_steps;
	pong_b_ypos_inc = (KeyFramePong[playIndexPong + 1].pong_b_ypos - KeyFramePong[playIndexPong].pong_b_ypos) / i_max_steps;
}
void interpolationPelota(void)
{
	pel_x_inc = (KeyFramePelota[playIndexPelota + 1].pel_x - KeyFramePelota[playIndexPelota].pel_x) / i_max_steps;
	pel_y_inc = (KeyFramePelota[playIndexPelota + 1].pel_y - KeyFramePelota[playIndexPelota].pel_y) / i_max_steps;
	pel_z_inc = (KeyFramePelota[playIndexPelota + 1].pel_z - KeyFramePelota[playIndexPelota].pel_z) / i_max_steps;
	pel_rot_y_inc = (KeyFramePelota[playIndexPelota + 1].pel_rot_y - KeyFramePelota[playIndexPelota].pel_rot_y) / i_max_steps;
}
//bool play = false;

bool robot_correct_angle(float x, float z)
{
	float inc = 5.0f;//5 grados de rotacion
	float deltax = x - robot_pos_x;
	float deltaz = z - robot_pos_z;
	float target_angle = vectorAngle(-deltax, deltaz);
	float delta_angle = robot_rot_y - target_angle;
	if (delta_angle == 0.0f)
		return true;
	//No esta en el angulo correcto.
	//Correccion de direccion de giro
	//Generamos la rotacion en si
	//Correccion de direccion de giro
	if (delta_angle > 180.0f)
	{
		delta_angle = delta_angle - 360.0f;
	}
	else if (delta_angle < -180.0f)
	{
		delta_angle = delta_angle + 360.0f;
	}
	if (abs(delta_angle) <= inc)
	{
		robot_rot_y = target_angle;
		//std::cout << "Reached target angle " << target_angle << std::endl;
	}
	else if (delta_angle < 0)
	{
		robot_rot_y += inc;
	}
	else
	{
		robot_rot_y -= inc;
	}
	return false;
}

bool robot_correct_position(float x, float z)
{
	if (robot_pos_x == x && robot_pos_z == z)
		return true;
	float inc = 0.10f;//Incremento de distancia por unidad de tiempo
	float deltax = x - robot_pos_x;
	float deltaz = z - robot_pos_z;
	float target_angle = degToRad(vectorAngle(deltax, deltaz));
	float incx = inc * cos(target_angle);
	float incz = inc * sin(target_angle);
	if (sqrt((deltax * deltax) + (deltaz * deltaz)) <= inc)
	{
		robot_pos_x = x;
		robot_pos_z = z;
	}
	else 
	{
		robot_pos_x += incx;
		robot_pos_z += incz;
	}
	return false;
}
bool angle_verified = false;

int plane_state = 0;
int robot_state = -1;

float radius = 20.0f;

//Para animacion del carro
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 0.0f,
rot_llanta = 0.0f;

void animate(void)
{		//Para Keyframes (Pong y Pelota)
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndexPong++;
			playIndexPelota++;
			if (playIndexPong > FrameIndexPong - 2)	//end of total animation?
			{
				playIndexPong = 0;
				resetPongElements();
				interpolationPong();
			}
			else //Next frame interpolations
			{  
				interpolationPong();
			}
			if (playIndexPelota > FrameIndexPelota - 2)	//end of total animation?
			{
				playIndexPelota = 0;
				resetPelotaElements();
				interpolationPelota();
			}
			else //Next frame interpolations
			{
				interpolationPelota();
			}
			i_curr_steps = 0; //Reset counter
		}
		else
		{
			//Draw animation -PONG
			pong_l_pos += pong_l_pos_inc;
			pong_r_pos += pong_r_pos_inc;
			pong_b_xpos += pong_b_xpos_inc;
			pong_b_ypos += pong_b_ypos_inc;
			//Draw animation -Pelota
			pel_x += pel_x_inc;
			pel_y += pel_y_inc;
			pel_z += pel_z_inc;
			pel_rot_y += pel_rot_y_inc;
			//Contador++
			i_curr_steps++;
		}
		//Para el robot
		rotor_rotacion += 30.0f;
		if (rotor_rotacion >= 360.0f)
		{
			rotor_rotacion = 0.0f;
		}
		//Máquina de estados del robot
		switch(robot_state)
		{
		case -1:
			if (angle_verified)
			{
				if (robot_correct_position(189.384, 291.558))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(189.384, 291.558);
			}
			break;
		case 0:
			if (angle_verified)
			{
				if (robot_correct_position(189.285, 313.769))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(189.285, 313.769);
			}
			break;
		case 1:
			if (angle_verified)
			{
				if (robot_correct_position(189.384, 291.558))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(189.384, 291.558);
			}
			break;
		case 2:
			if (angle_verified)
			{
				if (robot_correct_position(151.971, 290.928))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(151.971, 290.928);
			}
			break;
		case 3:
			if (angle_verified)
			{
				if (robot_correct_position(152.217, 309.121))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(152.217, 309.121);
			}
			break;
		case 4:
			if (angle_verified)
			{
				if (robot_correct_position(153.0, 312.229))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(153.0, 312.229);
			}
			break;
		case 5:
			if (angle_verified)
			{
				if (robot_correct_position(157.629, 312.106))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(157.629, 312.106);
			}
			break;
		case 6:
			if (angle_verified)
			{
				if (robot_correct_position(157.898, 314.92))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(157.898, 314.92);
			}
			break;
		case 7:
			if (angle_verified)
			{
				if (robot_correct_position(165.451, 314.293))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(165.451, 314.293);
			}
			break;
		case 8:
			if (angle_verified)
			{
				if (robot_correct_position(165.812, 291.674))
				{
					robot_state++;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(165.812, 291.674);
			}
			break;
		default:
			if (angle_verified)
			{
				if (robot_correct_position(204.127f, 290.874f))
				{
					robot_state = -1;
					angle_verified = false;
				}
			}
			else
			{
				angle_verified = robot_correct_angle(204.127f, 290.874f);
			}
			break;
		}
		//Para avion
		switch (plane_state) 
		{
		case 0:
			if (avion_pos_z >= 100.0f)
			{
				plane_state++;
				avion_rot += 15.0f;
				radius = 15.0f;
			}
			else
			{
				avion_pos_x -= 0.157f;
				avion_pos_y -= 0.3;
				avion_pos_z += 4.0f;
			}
			break;
		case 1:
			if (avion_rot == 360.0f)
			{
				avion_rot = 0.0f;
				plane_state++;
			}
			else
			{
				avion_rot += 15.0f;
			}
			break;
		case 2:
			if (avion_pos_z >= 230.0f)
			{
				plane_state++;
				avion_rot += 15.0f;
				radius = 10.0f;
			}
			else
			{
				avion_pos_x -= 0.157f;
				avion_pos_y -= 0.2;
				avion_pos_z += 3.0f;
			}
			break;
		case 3:
			if (avion_rot == 360.0f)
			{
				avion_rot = 0.0f;
				plane_state++;
			}
			else
			{
				avion_rot += 15.0f;
			}
			break;
		case 4:
			if (avion_pos_z == 400.0f)
			{
				plane_state++;
				avion_rot += 20.0f;
				radius = 5.0f;
			}
			else
			{
				avion_pos_z += 2.0f;
				avion_pos_y -= 0.0203f;
			}
			break;
		case 5:
			if (avion_rot == 360.0f)
			{
				avion_rot = 0.0f;
				plane_state++;
			}
			else
			{
				avion_rot += 20.0f;
			}
			break;
		default:
			if (avion_pos_z == 720.0f)
			{
				plane_state = 0;
				avion_pos_x = 0.0f;
				avion_pos_y = 0.0f;
				avion_pos_z = 0.0f;
			}
			else
			{
				avion_pos_z += 2.0f;
				avion_pos_y -= 0.0218f;
			}
			break;
		}
		//Para el carro
		rot_llanta += 60.0f;
		if (rot_llanta >= 360.0f)
		{
			rot_llanta = 0.0f;
		}
}

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
float map_max_z = 800.0f;
void drawingGrass()
{
	float vertices[] = {
		// positions          // texture coords
		 map_max_x , 0.0f, 0.0f,   map_max_x/3, 0.0f, // top right
		 map_max_x , 0.0f, map_max_z ,   map_max_x/3, map_max_z/3, // bottom right
		0.0f, 0.0f, map_max_z ,   0.0f, map_max_z/3, // bottom left
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

float street_left_left = 290.0f;
float street_left_right = 302.0f;
float street_right_left = 334.0f;
float street_right_right = 346.f;
float street_height = 0.3;
float pavement_height = 0.2;
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
	Model casa1("resources/objects/casa1/casa1.obj");
	Model casa2("resources/objects/casa2/casa2.obj");
	Model casa3("resources/objects/casa3/casa3.obj");
	Model casa4("resources/objects/casa4/casa4.obj");
	Model bardas("resources/objects/bardas/bardas.obj");
	Model bosque("resources/objects/bosque/bosque.obj");
	Model caminos("resources/objects/caminos/caminos.obj");
	Model postes("resources/objects/postes/postes.obj");
	//Para animacion de PONG.
	Model pong_l("resources/objects/pong/pong_izq.obj");
	Model pong_r("resources/objects/pong/pong_der.obj");
	Model pong_b("resources/objects/pong/pong_ball.obj");
	//Para animacion de la pelota en la alberca.
	Model pelota("resources/objects/pelota/pelota.obj");
	//Para animación del avión.
	Model avion("resources/objects/avion/avion.obj");
	//Para animación del robot.
	Model robot("resources/objects/robot/robot.obj");
	Model robot_rotor_izq("resources/objects/robot/rotor_left.obj");
	Model robot_rotor_der("resources/objects/robot/rotor_right.obj");
	//Para animación del carro.
	Model carro("resources/objects/lambo/carroceria.obj");
	Model llanta("resources/objects/lambo/Wheel.obj");
	//Cargando texturas

	LoadTextures();
	//Inicializo keyframes para Pong
	for (int i = 0; i < MAX_FRAMES_PONG; i++)
	{
		KeyFramePong[i].pong_l_pos = 0;
		KeyFramePong[i].pong_r_pos = 0;
		KeyFramePong[i].pong_b_xpos = 0;
		KeyFramePong[i].pong_b_ypos = 0;

	}
	KeyFramePong[0].pong_l_pos = 1.3;
	KeyFramePong[0].pong_r_pos = 0;
	KeyFramePong[0].pong_b_xpos = -0.5;
	KeyFramePong[0].pong_b_ypos = 0.1;

	KeyFramePong[1].pong_l_pos = 4.7;
	KeyFramePong[1].pong_r_pos = 1.7;
	KeyFramePong[1].pong_b_xpos = 15.5;
	KeyFramePong[1].pong_b_ypos = 4.7;

	KeyFramePong[2].pong_l_pos = 2.3;
	KeyFramePong[2].pong_r_pos = 7.79999;
	KeyFramePong[2].pong_b_xpos = -0.6;
	KeyFramePong[2].pong_b_ypos = 8.09999;

	KeyFramePong[3].pong_l_pos = 6.3;
	KeyFramePong[3].pong_r_pos = 3.6;
	KeyFramePong[3].pong_b_xpos = 15.5;
	KeyFramePong[3].pong_b_ypos = 5.8;

	KeyFramePong[4].pong_l_pos = 4.1;
	KeyFramePong[4].pong_r_pos = 8.2;
	KeyFramePong[4].pong_b_xpos = -0.6;
	KeyFramePong[4].pong_b_ypos = 8.3;

	KeyFramePong[5].pong_l_pos = 7.1;
	KeyFramePong[5].pong_r_pos = 6.2;
	KeyFramePong[5].pong_b_xpos = 15.5;
	KeyFramePong[5].pong_b_ypos = 6.8;

	KeyFramePong[6].pong_l_pos = 1.3;
	KeyFramePong[6].pong_r_pos = 0;
	KeyFramePong[6].pong_b_xpos = -0.5;
	KeyFramePong[6].pong_b_ypos = 0.1;
	resetPongElements();
	interpolationPong();
	playIndexPong = 0;
	//Inicializo frames para Pelota
	for (int i = 0; i < MAX_FRAMES_PELOTA; i++)
	{
		KeyFramePelota[i].pel_x = 0;
		KeyFramePelota[i].pel_y = 0;
		KeyFramePelota[i].pel_z = 0;
		KeyFramePelota[i].pel_rot_y = 0;
	}
	KeyFramePelota[0].pel_x = -42.2999;
	KeyFramePelota[0].pel_y = 2.2;
	KeyFramePelota[0].pel_z = -231.105;
	KeyFramePelota[0].pel_rot_y = 26.6001;

	KeyFramePelota[1].pel_x = -44.4999;
	KeyFramePelota[1].pel_y = 2.2;
	KeyFramePelota[1].pel_z = -234.305;
	KeyFramePelota[1].pel_rot_y = 3.3;

	KeyFramePelota[2].pel_x = -47.6998;
	KeyFramePelota[2].pel_y = 2.4;
	KeyFramePelota[2].pel_z = -230.905;
	KeyFramePelota[2].pel_rot_y = -35.3;

	KeyFramePelota[3].pel_x = -52.4998;
	KeyFramePelota[3].pel_y = 1.5;
	KeyFramePelota[3].pel_z = -234.705;
	KeyFramePelota[3].pel_rot_y = -58.6997;

	KeyFramePelota[4].pel_x = -50.3998;
	KeyFramePelota[4].pel_y = 2.6;
	KeyFramePelota[4].pel_z = -239.905;
	KeyFramePelota[4].pel_rot_y = -96.5991;

	KeyFramePelota[5].pel_x = -54.3998;
	KeyFramePelota[5].pel_y = 1.8;
	KeyFramePelota[5].pel_z = -232.405;
	KeyFramePelota[5].pel_rot_y = -130.5991;

	KeyFramePelota[6].pel_x = -51.7;
	KeyFramePelota[6].pel_y = 1.3;
	KeyFramePelota[6].pel_z = -230.405;
	KeyFramePelota[6].pel_rot_y = -159.001;

	KeyFramePelota[7].pel_x = -47.9001;
	KeyFramePelota[7].pel_y = 1;
	KeyFramePelota[7].pel_z = -228.605;
	KeyFramePelota[7].pel_rot_y = -120.901;

	KeyFramePelota[8].pel_x = -45;
	KeyFramePelota[8].pel_y = 1.8;
	KeyFramePelota[8].pel_z = -232.405;
	KeyFramePelota[8].pel_rot_y = -159.001;

	KeyFramePelota[9].pel_x = -43.8;
	KeyFramePelota[9].pel_y = 1.6;
	KeyFramePelota[9].pel_z = -228.905;
	KeyFramePelota[9].pel_rot_y = -200.603;

	KeyFramePelota[10].pel_x = -40.2001;
	KeyFramePelota[10].pel_y = 1.1;
	KeyFramePelota[10].pel_z = -228.105;
	KeyFramePelota[10].pel_rot_y = -229.605;

	KeyFramePelota[11].pel_x = -38.3001;
	KeyFramePelota[11].pel_y = -0.199999;
	KeyFramePelota[11].pel_z = -233.905;
	KeyFramePelota[11].pel_rot_y = -229.905;

	KeyFramePelota[12].pel_x = -42.2999;
	KeyFramePelota[12].pel_y = 2.2;
	KeyFramePelota[12].pel_z = -231.105;
	KeyFramePelota[12].pel_rot_y = 26.6001;
	resetPelotaElements();
	interpolationPelota();
	playIndexPelota = 0;
	//Inicializo timer de keyframes
	i_curr_steps = 0;
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
		glm::mat4 tmp2 = glm::mat4(1.0f);
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
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 300.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bardas.Draw(staticShader);
		bosque.Draw(staticShader);
		caminos.Draw(staticShader);
		casa1.Draw(staticShader);
		casa2.Draw(staticShader);
		casa3.Draw(staticShader);
		casa4.Draw(staticShader);
		postes.Draw(staticShader);
		//Animacion 1: PONG
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(0.0f, pong_l_pos, 0.0f));
		staticShader.setMat4("model", tmp);
		pong_l.Draw(staticShader);
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(0.0f, pong_r_pos, 0.0f));
		staticShader.setMat4("model", tmp);
		pong_r.Draw(staticShader);
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(pong_b_xpos, pong_b_ypos, 0.0f));
		staticShader.setMat4("model", tmp);
		pong_b.Draw(staticShader);
		//Animacion 2: Pelota
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(pel_x, pel_y, pel_z));
		tmp = glm::rotate(tmp, glm::radians(pel_rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", tmp);
		pelota.Draw(staticShader);
		//Animacion 3: Avión de papel
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(336.0f, 25.0f + radius, 45.0f));
		model = glm::translate(model, glm::vec3(avion_pos_x, avion_pos_y, avion_pos_z));
		model = glm::rotate(model, glm::radians(-avion_rot), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -radius, 0.0f));
		staticShader.setMat4("model", model);
		avion.Draw(staticShader);
		//Animacion 4: Robot aspiradora
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(robot_pos_x, 0.3f, robot_pos_z));
		model = glm::scale(model, glm::vec3(0.333f));
		model = glm::rotate(model, glm::radians(robot_rot_y - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		robot.Draw(staticShader);
		tmp = glm::translate(model, glm::vec3(1.348f, -0.592f, 0.839f));
		tmp = glm::rotate(tmp, glm::radians(-rotor_rotacion), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", tmp);
		robot_rotor_izq.Draw(staticShader);
		tmp = glm::translate(model, glm::vec3(-1.351f, -0.592f, 0.839f));
		tmp = glm::rotate(tmp, glm::radians(rotor_rotacion), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", tmp);
		robot_rotor_der.Draw(staticShader);
		//Animacion 5: Carro
		model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(15.0f + movAuto_x, -1.0f, movAuto_z));
		tmp = model = glm::rotate(model, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		carro.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(8.5f, 3.5f, 12.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(-rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.5f, 3.5f, 12.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Der delantera

		model = glm::translate(tmp, glm::vec3(-8.5f, 4.0f, -14.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Der trasera

		model = glm::translate(tmp, glm::vec3(8.5f, 4.0f, -14.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(-rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);	//Izq trase
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
		camera.ProcessKeyboard(FORWARD, (float)deltaTime * speedMultiplier);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime * speedMultiplier);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime * speedMultiplier);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime * speedMultiplier);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		speedMultiplier *= 2;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		speedMultiplier /= 2;
	//PARA PONG
	/*if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		pel_x += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		pel_x -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		pel_y += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		pel_y -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		pel_z += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		pel_z -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		pel_rot_y += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		pel_rot_y -= 0.1f;*/
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		/*std::cout << "POSX: " << camera.Position.x << std::endl;
		std::cout << "POSY: " << camera.Position.y << std::endl;
		std::cout << "POSZ: " << camera.Position.z << std::endl;*/
		std::cout << "(" << camera.Position.x << ", " << camera.Position.z << ");" << std::endl;
	}
	//if (key == GLFW_KEY_P && action == GLFW_PRESS)
	//{
	//	if (play == false && (FrameIndexPelota > 1))
	//	{
	//		std::cout << "Play animation" << std::endl;
	//		resetPelotaElements();
	//		//First Interpolation				
	//		interpolationPelota();

	//		play = true;
	//		playIndexPelota = 0;
	//		i_curr_steps = 0;
	//	}
	//	else
	//	{
	//		play = false;
	//		std::cout << "Not enough Key Frames" << std::endl;
	//	}
	//}

	////To Save a KeyFramePong
	//if (key == GLFW_KEY_L && action == GLFW_PRESS)
	//{
	//	if (FrameIndexPelota < MAX_FRAMES_PELOTA)
	//	{
	//		saveFrame();
	//	}
	//}
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