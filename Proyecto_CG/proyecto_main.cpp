﻿/*---------------------------------------------------------*/
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

#include <irrklang/irrKlang.h>
using namespace irrklang;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>

//Para reproducir sonidos.
ISoundEngine* SoundEngine = createIrrKlangDevice();

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

unsigned int SCR_WIDTH;
unsigned int SCR_HEIGHT;

// camera
Camera camera(glm::vec3(308.603, 15.5462, 304.647));
float speedMultiplier = 0.3;
float lastX;
float lastY;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, 1.0f);
glm::vec3 insideLightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 insideLightDirection(0.0f, -1.0f, 0.1f);

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

// KeyFrame para Recorrido
float	rec_pos_x = 0.0f,
rec_pos_y = 0.0f,
rec_pos_z = 0.0f,
rec_pitch = 0.0f,
rec_yaw = 0.0f;
float	rec_pos_x_inc = 0.0f,
rec_pos_y_inc = 0.0f,
rec_pos_z_inc = 0.0f,
rec_pitch_inc = 0.0f,
rec_yaw_inc = 0.0f;


#define MAX_FRAMES_PONG 7

#define MAX_FRAMES_PELOTA 13

#define MAX_FRAMES_RECORRIDO 110//Guardo unos extra, pero realmente son 106

int i_max_steps = 120;
int i_curr_steps = 0;
int i_curr_steps_p = 0;
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
int playIndexPelotaRand = 1;
//RECORRIDO

typedef struct recorrido_frame
{
	//Variables para GUARDAR Key Frames
	float	rec_pos_x = 0.0f,
		rec_pos_y = 0.0f,
		rec_pos_z = 0.0f,
		rec_pitch = 0.0f,
		rec_yaw = 0.0f;

}REC_FRAME;

REC_FRAME KeyFrameRec[MAX_FRAMES_RECORRIDO];
int FrameIndexRec = 0;			//introducir datos
int playIndexRec = 0;


//Animación robot
float robot_pos_x = 204.127f,
robot_pos_z = 290.874f,
robot_rot_y = 0.0f;

//Animación avión
float
avion_delta_despl_inicial = 2.0,
avion_delta_despl = avion_delta_despl_inicial, //Cuanto se desplaza el avion en la animación, función.
	avion_last_pos_y = 0.0f,
    avion_pos_x = 0.0f,
	avion_pos_y = 0.0f,
	avion_pos_z = 0.0f,
	avion_rot = 0.0f;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	rec_pos_x = camera.Position.x;
	rec_pos_y = camera.Position.y;
	rec_pos_z = camera.Position.z;
	rec_yaw = camera.Yaw;
	rec_pitch = camera.Pitch;
	std::cout << "KeyFrameRec[" << FrameIndexRec << "].rec_pos_x = " << rec_pos_x << ";" << std::endl;
	std::cout << "KeyFrameRec[" << FrameIndexRec << "].rec_pos_y = " << rec_pos_y << ";" << std::endl;
	std::cout << "KeyFrameRec[" << FrameIndexRec << "].rec_pos_z = " << rec_pos_z << ";" << std::endl;
	std::cout << "KeyFrameRec[" << FrameIndexRec << "].rec_yaw = " << rec_yaw << ";" << std::endl;
	std::cout << "KeyFrameRec[" << FrameIndexRec << "].rec_pitch = " << rec_pitch << ";" << std::endl;
	KeyFrameRec[FrameIndexRec].rec_pos_x = rec_pos_x;
	KeyFrameRec[FrameIndexRec].rec_pos_y = rec_pos_y;
	KeyFrameRec[FrameIndexRec].rec_pos_z = rec_pos_z;
	KeyFrameRec[FrameIndexRec].rec_yaw = rec_yaw;
	KeyFrameRec[FrameIndexRec].rec_pitch = rec_pitch;
	FrameIndexRec++;
}

void resetRecorridoElements(void)
{
	rec_pos_x = KeyFrameRec[0].rec_pos_x;
	rec_pos_y = KeyFrameRec[0].rec_pos_y;
	rec_pos_z = KeyFrameRec[0].rec_pos_z;
	rec_pitch = KeyFrameRec[0].rec_pitch;
	rec_yaw = KeyFrameRec[0].rec_yaw;
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

void interpolationRecorrido(void)
{
	rec_pos_x_inc = (KeyFrameRec[playIndexRec + 1].rec_pos_x - KeyFrameRec[playIndexRec].rec_pos_x) / i_max_steps;
	rec_pos_y_inc = (KeyFrameRec[playIndexRec + 1].rec_pos_y - KeyFrameRec[playIndexRec].rec_pos_y) / i_max_steps;
	rec_pos_z_inc = (KeyFrameRec[playIndexRec + 1].rec_pos_z - KeyFrameRec[playIndexRec].rec_pos_z) / i_max_steps;
	float curPitch = KeyFrameRec[playIndexRec].rec_pitch;
	float nextPitch = KeyFrameRec[playIndexRec + 1].rec_pitch;
	float deltaPitch = nextPitch - curPitch;
	float curYaw = KeyFrameRec[playIndexRec].rec_yaw;
	float nextYaw = KeyFrameRec[playIndexRec + 1].rec_yaw;
	float deltaYaw = nextYaw - curYaw;
	if (deltaPitch > 180.0f)
	{
		deltaPitch = deltaPitch - 360.0f;
	}
	else if (deltaPitch < -180.0f)
	{
		deltaPitch = deltaPitch + 360.0f;
	}
	
	if (deltaYaw > 180.0f)
	{
		deltaYaw = deltaYaw - 360.0f;
	}
	else if (deltaYaw < -180.0f)
	{
		deltaYaw = deltaYaw + 360.0f;
	}

	rec_yaw_inc = deltaYaw / i_max_steps;
	rec_pitch_inc = deltaPitch / i_max_steps;
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
	pel_x_inc = (KeyFramePelota[playIndexPelotaRand].pel_x - KeyFramePelota[playIndexPelota].pel_x) / i_max_steps;
	pel_y_inc = (KeyFramePelota[playIndexPelotaRand].pel_y - KeyFramePelota[playIndexPelota].pel_y) / i_max_steps;
	pel_z_inc = (KeyFramePelota[playIndexPelotaRand].pel_z - KeyFramePelota[playIndexPelota].pel_z) / i_max_steps;
	pel_rot_y_inc = (KeyFramePelota[playIndexPelotaRand].pel_rot_y - KeyFramePelota[playIndexPelota].pel_rot_y) / i_max_steps;
}

bool robot_correct_angle(float x, float z)
{
	float inc = 2.5f;//5 grados de rotacion
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
	float inc = 0.05f;//Incremento de distancia por unidad de tiempo
	float deltax = x - robot_pos_x;
	float deltaz = z - robot_pos_z;
	float target_angle = degToRad(vectorAngle(deltax, deltaz));
	float incx = inc * cos(target_angle);
	float incz = inc * sin(target_angle);
	if (sqrt((deltax * deltax) + (deltaz * deltaz)) <= inc)//Si quedamos a menos distancia que un delta
	{//Solo nos ponemos en el waypoint deseado
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

//Para animacion del carro
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 0.0f,
rot_llanta = 0.0f;
//Para la pseudo máquina de estados
int steps_carro = 32;
//La animación del carro en forma de dos arreglos
float anim_x[] = { 118.702, 118.702, 113.514, 109.169, 106.169, 104.347, 103.534, 102.903, 102.279, 101.657, 101.682, 101.238, 101.742,
102.474, 103.141, 103.988, 104.777, 106.243, 109.572, 177.793, 286.741, 291.249, 294.54, 296.299, 298.214, 300.326, 301.416,
304.111, 307.582, 308.837, 310.46, 309.892};
float anim_z[] = { 277.351, 278.351, 278.816, 277.328, 275.966, 273.054, 270.882, 268.724, 266.517, 264.397, 261.528, 257.828, 255.598,
253.415, 251.334, 249.832, 248.256, 245.477, 243.75, 242.395, 246.957, 246.919, 246.361, 245.567, 244.3, 242.34, 240.412,
238.009, 233.838, 229.598, 159.368, 2.2559};
//La aceleracion del carro en cada punto
float accel_carro[] = {1.0, 0.8, 0.4, 1.0, 0.8, 1.2, 1.2, 1.0, 1.0, 1.2,
					   1.2, 1.2, 1.5, 1.5, 0.7, 1.0, 1.0, 0.8, 0.8, 1.0,
                       1.2, 1.0, 0.7, 0.9, 1.0, 0.8, 0.9, 1.0, 1.3, 1.8,
                       1.8, 1.6, 1.0};//1 extra por indices
int state_carro = 32;

//Auto ajusta su movimiento con su aceleracion
float car_inc_start = 0.5f;
float car_inc = car_inc_start;
float car_inc_ang_start = 4.0f;
float car_inc_ang = car_inc_ang_start;
bool car_correct_position(float x, float z)
{
	if (movAuto_x == x && movAuto_z == z)
		return true;
	float deltax = x - movAuto_x;
	float deltaz = z - movAuto_z;
	float target_angle = degToRad(vectorAngle(deltax, deltaz));
	float incx = car_inc * cos(target_angle);
	float incz = car_inc * sin(target_angle);
	if (sqrt((deltax * deltax) + (deltaz * deltaz)) <= car_inc)
	{
		movAuto_x = x;
		movAuto_z = z;
	}
	else
	{
		movAuto_x += incx;
		movAuto_z += incz;
	}
	//Volteamos el carro con cierto smoothing.
	float angulo = vectorAngle(-deltax, deltaz);
	//Tomar en cuenta que el carro modifica su posicion MIENTRAS se mueve, a diferencia del robot
	float delta_angle = orienta - angulo;
	if (delta_angle != 0.0f)
	{
		if (delta_angle > 180.0f)
		{
			delta_angle = delta_angle - 360.0f;
		}
		else if (delta_angle < -180.0f)
		{
			delta_angle = delta_angle + 360.0f;
		}
		if (abs(delta_angle) <= car_inc_ang)//Si esta mas cerca que el delta angulo, solo hacemos la asignacion para no pasarnos
		{
			orienta = angulo;
		}
		else if (delta_angle < 0)
		{
			orienta += car_inc_ang;
		}
		else
		{
			orienta -= car_inc_ang;
		}
	}
	return false;
}
bool play = false;
void animate(void)
{
		//Keyframes de recorrdio
		if(play)
		{
			if (i_curr_steps_p >= i_max_steps)
			{
				std::cout << playIndexRec << std::endl;
				playIndexRec++;
				if (playIndexRec > FrameIndexRec - 2)	//end of total animation?
				{
					playIndexRec = 0;
					resetRecorridoElements();
					interpolationRecorrido();
					play = false;
				}
				else //Next frame interpolations
				{
					interpolationRecorrido();
				}
				i_curr_steps_p = 0;
			}
			else
			{
				//Draw animation - Recorrido
				rec_pos_x += rec_pos_x_inc;
				rec_pos_y += rec_pos_y_inc;
				rec_pos_z += rec_pos_z_inc;
				rec_yaw = fmod(rec_yaw + rec_yaw_inc, 360.0f);
				if (rec_yaw < 0.0f)
				{
					rec_yaw += 360.0f;
				}
				rec_pitch += rec_pitch_inc;
				camera.Position = glm::vec3(rec_pos_x, rec_pos_y, rec_pos_z);
				camera.Yaw = rec_yaw;
				camera.Pitch = rec_pitch;
				camera.updateCameraVectors();
				i_curr_steps_p++;
			}
		}
		//Para Keyframes (Pong y Pelota - Aunque pelota esta aleatorizada)
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndexPong++;
			playIndexPelota = playIndexPelotaRand;
			while (playIndexPelotaRand == playIndexPelota)
			{
				playIndexPelotaRand = rand() % 13;
			}
			float dx = camera.Position.x - 167.82f;
			float dy = camera.Position.y - 10.0f;
			float dz = camera.Position.z - 308.71f;
			if( ((dx * dx) + (dy * dy) + (dz * dz)) <= 200)
				SoundEngine->play2D("resources/audio/pong.wav");
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
			interpolationPelota();
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
		//Para el avion
		//El avion avanza en el eje Z
		//Tiene la trayectoria de una onda sinusoidal amortiguada
		if (avion_pos_z >= 720.0f)
		{//Reiniciar animacion
			avion_pos_z = 0.0f;
			avion_pos_x = 0.0f;
			avion_pos_y = 0.0f;
			avion_rot = 0.0f;
			avion_last_pos_y = 0.0f;
			avion_delta_despl = avion_delta_despl_inicial;
		}else
		{
			float a = 4000.0f;
			float b = 74.0f;
			float c = 210.0f;
			avion_delta_despl *= 0.9976; //Drag/Friccion del aire/Perdida de momento
			avion_pos_z += avion_delta_despl;
			avion_last_pos_y = avion_pos_y;
			avion_pos_y = (a / (avion_pos_z + b)) * sin(avion_pos_z * 2.0f * 3.1415926f / c);
			float avion_delta_y = (avion_pos_y - avion_last_pos_y);
			avion_rot = vectorAngle(avion_delta_despl, avion_delta_y);
			if(avion_delta_y < 0.0f)//Efecto de la gravedad
			{
				avion_delta_despl *= 1.0047; //Esto es mayor al efecto del drag, por lo que aumenta la velocidad
			}
		}
		//Para el carro
		rot_llanta += 10.0f * (car_inc/car_inc_start);//Ajustamos velocidad de giro de ruedas con velocidad del carro
		if (rot_llanta >= 360.0f)
		{
			rot_llanta = 0.0f;
		}
		if (state_carro == steps_carro)
		{
			state_carro = 1;
			car_inc = car_inc_start;//Reiniciamos la velocidad de giro
			car_inc_ang = car_inc_ang_start;//Reiniciamos la velocidad de giro
			movAuto_x = anim_x[0];
			movAuto_z = anim_z[0];
			float dx = anim_x[2] - anim_x[1];
			float dz = anim_z[2] - anim_z[1];
			orienta = vectorAngle(-dx, dz);
		}
		if(car_correct_position(anim_x[state_carro], anim_z[state_carro]))
		{
			state_carro++;
			car_inc *= accel_carro[state_carro];
			car_inc_ang *= accel_carro[state_carro];
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
		// positions
// texture coords
		 map_max_x , 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  map_max_x/3, 0.0f, // top right
		 map_max_x , 0.0f, map_max_z , 0.0f, 1.0f, 0.0f,  map_max_x/3, map_max_z/3, // bottom right
		0.0f, 0.0f, map_max_z , 0.0f, 1.0f, 0.0f,  0.0f, map_max_z/3, // bottom left
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f  // top left
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

float street_left_left = 290.0f;
float street_left_right = 302.0f;
float street_right_left = 334.0f;
float street_right_right = 346.f;
float street_height = 0.3;
float pavement_height = 0.2;

bool noche = false;

void drawingStreet()
{
	float vertices[] = {
		// positions
		// texture coords
		 street_left_right,  street_height , map_max_z ,  0.0f, 1.0f, 0.0f,  street_left_right- street_left_left, 0.0f, // north right
		 street_left_right,   street_height , 0.0f, 0.0f, 1.0f, 0.0f, street_left_right - street_left_left, map_max_z, // south right
		 street_left_left ,   street_height , 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, map_max_z, // south left
		 street_left_left ,   street_height , map_max_z , 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // north left

		 street_left_left ,  street_height , map_max_z , -1.0f, 0.0f, 0.0f, street_height, 0.0f, //LEFT: top north
		 street_left_left ,   0.0f , map_max_z, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom north
		 street_left_left ,   0.0f , 0.0f , -1.0f, 0.0f, 0.0f, 0.0f, map_max_z, // bottom south
		 street_left_left ,   street_height , 0.0f , -1.0f, 0.0f, 0.0f, street_height, map_max_z,  // top south

		 street_left_right ,  street_height , map_max_z , 1.0f, 0.0f, 0.0f, street_height, 0.0f, //RIGHT: top north
		 street_left_right ,   0.0f , map_max_z, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom north
		 street_left_right ,   0.0f , 0.0f , 1.0f, 0.0f, 0.0f, 0.0f, map_max_z, // bottom south
		 street_left_right ,   street_height , 0.0f , 1.0f, 0.0f, 0.0f, street_height, map_max_z,  // top south

		 street_right_right,  street_height , map_max_z , 0.0f, 1.0f, 0.0f,  street_right_right - street_right_left, 0.0f, // north right
		 street_right_right,   street_height , 0.0f, 0.0f, 1.0f, 0.0f, street_right_right - street_right_left, map_max_z, // south right
		 street_right_left ,   street_height , 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, map_max_z, // south left
		 street_right_left ,   street_height , map_max_z , 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // north left

		 street_right_left ,  street_height , map_max_z , -1.0f, 0.0f, 0.0f, street_height, 0.0f, //LEFT: top north
		 street_right_left ,   0.0f , map_max_z, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom north
		 street_right_left ,   0.0f , 0.0f , -1.0f, 0.0f, 0.0f, 0.0f, map_max_z, // bottom south
		 street_right_left ,   street_height , 0.0f , -1.0f, 0.0f, 0.0f, street_height, map_max_z,  // top south

		 street_right_right ,  street_height , map_max_z , 1.0f, 0.0f, 0.0f, street_height, 0.0f, //RIGHT: top north
		 street_right_right ,   0.0f , map_max_z, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom north
		 street_right_right ,   0.0f , 0.0f , 1.0f, 0.0f, 0.0f, 0.0f, map_max_z, // bottom south
		 street_right_right ,   street_height , 0.0f , 1.0f, 0.0f, 0.0f, street_height, map_max_z,  // top south

		 street_right_right,  pavement_height , map_max_z , 0.0f, 1.0f, 0.0f,   street_right_left-street_left_right, 0.0f, //PAVEMENT: north right
		 street_right_right,   pavement_height , 0.0f, 0.0f, 1.0f, 0.0f, street_right_left - street_left_right, map_max_z, // south right
		 street_left_right ,   pavement_height , 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, map_max_z, // south left
		 street_left_right ,   pavement_height , map_max_z , 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // north left
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void LoadTextures()
{
	t_pasto = generateTextures("resources/texturas/pasto.jpg", 0);
	t_acera = generateTextures("resources/texturas/acera.jpg", 0);
	t_pavimento = generateTextures("resources/texturas/pavimento.jpg", 0);
}

void inicializaRecorrido()
{
	KeyFrameRec[0].rec_pos_x = 308.603;
	KeyFrameRec[0].rec_pos_y = 15.5462;
	KeyFrameRec[0].rec_pos_z = 304.647;
	KeyFrameRec[0].rec_yaw = 182.3;
	KeyFrameRec[0].rec_pitch = -14;
	KeyFrameRec[1].rec_pos_x = 308.603;
	KeyFrameRec[1].rec_pos_y = 15.5462;
	KeyFrameRec[1].rec_pos_z = 304.647;
	KeyFrameRec[1].rec_yaw = 125.6;
	KeyFrameRec[1].rec_pitch = -9.8;
	KeyFrameRec[2].rec_pos_x = 308.603;
	KeyFrameRec[2].rec_pos_y = 15.5462;
	KeyFrameRec[2].rec_pos_z = 304.647;
	KeyFrameRec[2].rec_yaw = 75.2;
	KeyFrameRec[2].rec_pitch = -8.4;
	KeyFrameRec[3].rec_pos_x = 308.603;
	KeyFrameRec[3].rec_pos_y = 15.5462;
	KeyFrameRec[3].rec_pos_z = 304.647;
	KeyFrameRec[3].rec_yaw = 6.59995;
	KeyFrameRec[3].rec_pitch = -8.4;
	KeyFrameRec[4].rec_pos_x = 308.603;
	KeyFrameRec[4].rec_pos_y = 15.5462;
	KeyFrameRec[4].rec_pos_z = 304.647;
	KeyFrameRec[4].rec_yaw = 295.2;
	KeyFrameRec[4].rec_pitch = -4.9;
	KeyFrameRec[5].rec_pos_x = 308.603;
	KeyFrameRec[5].rec_pos_y = 15.5462;
	KeyFrameRec[5].rec_pos_z = 304.647;
	KeyFrameRec[5].rec_yaw = 188.8;
	KeyFrameRec[5].rec_pitch = -9.1;
	KeyFrameRec[6].rec_pos_x = 293.03;
	KeyFrameRec[6].rec_pos_y = 6.51898;
	KeyFrameRec[6].rec_pos_z = 304.539;
	KeyFrameRec[6].rec_yaw = 188.799;
	KeyFrameRec[6].rec_pitch = -8.4;
	KeyFrameRec[7].rec_pos_x = 283.791;
	KeyFrameRec[7].rec_pos_y = 5.63759;
	KeyFrameRec[7].rec_pos_z = 306.786;
	KeyFrameRec[7].rec_yaw = 217.499;
	KeyFrameRec[7].rec_pitch = -8.4;
	KeyFrameRec[8].rec_pos_x = 269.396;
	KeyFrameRec[8].rec_pos_y = 4.84616;
	KeyFrameRec[8].rec_pos_z = 297.055;
	KeyFrameRec[8].rec_yaw = 206.299;
	KeyFrameRec[8].rec_pitch = -4.52995e-06;
	KeyFrameRec[9].rec_pos_x = 260.024;
	KeyFrameRec[9].rec_pos_y = 4.88293;
	KeyFrameRec[9].rec_pos_z = 295.026;
	KeyFrameRec[9].rec_yaw = 175.499;
	KeyFrameRec[9].rec_pitch = 0.699995;
	KeyFrameRec[10].rec_pos_x = 251.942;
	KeyFrameRec[10].rec_pos_y = 4.98378;
	KeyFrameRec[10].rec_pos_z = 296.947;
	KeyFrameRec[10].rec_yaw = 195.099;
	KeyFrameRec[10].rec_pitch = 2.09999;
	KeyFrameRec[11].rec_pos_x = 235.827;
	KeyFrameRec[11].rec_pos_y = 4.25665;
	KeyFrameRec[11].rec_pos_z = 293.598;
	KeyFrameRec[11].rec_yaw = 189.499;
	KeyFrameRec[11].rec_pitch = -3.50001;
	KeyFrameRec[12].rec_pos_x = 218.364;
	KeyFrameRec[12].rec_pos_y = 3.70406;
	KeyFrameRec[12].rec_pos_z = 290.55;
	KeyFrameRec[12].rec_yaw = 181.799;
	KeyFrameRec[12].rec_pitch = -3.50001;
	KeyFrameRec[13].rec_pos_x = 205.266;
	KeyFrameRec[13].rec_pos_y = 3.81;
	KeyFrameRec[13].rec_pos_z = 290.952;
	KeyFrameRec[13].rec_yaw = 181.8;
	KeyFrameRec[13].rec_pitch = 1.39999;
	KeyFrameRec[14].rec_pos_x = 205.266;
	KeyFrameRec[14].rec_pos_y = 3.81;
	KeyFrameRec[14].rec_pos_z = 290.952;
	KeyFrameRec[14].rec_yaw = 254.599;
	KeyFrameRec[14].rec_pitch = -0.700005;
	KeyFrameRec[15].rec_pos_x = 203.782;
	KeyFrameRec[15].rec_pos_y = 3.74861;
	KeyFrameRec[15].rec_pos_z = 286.735;
	KeyFrameRec[15].rec_yaw = 253.899;
	KeyFrameRec[15].rec_pitch = -1.4;
	KeyFrameRec[16].rec_pos_x = 203.782;
	KeyFrameRec[16].rec_pos_y = 3.74861;
	KeyFrameRec[16].rec_pos_z = 286.735;
	KeyFrameRec[16].rec_yaw = 225.199;
	KeyFrameRec[16].rec_pitch = -4.2;
	KeyFrameRec[17].rec_pos_x = 203.782;
	KeyFrameRec[17].rec_pos_y = 3.74861;
	KeyFrameRec[17].rec_pos_z = 286.735;
	KeyFrameRec[17].rec_yaw = 225.199;
	KeyFrameRec[17].rec_pitch = -4.2;
	KeyFrameRec[18].rec_pos_x = 203.905;
	KeyFrameRec[18].rec_pos_y = 3.60206;
	KeyFrameRec[18].rec_pos_z = 284.512;
	KeyFrameRec[18].rec_yaw = 344.199;
	KeyFrameRec[18].rec_pitch = 7;
	KeyFrameRec[19].rec_pos_x = 203.905;
	KeyFrameRec[19].rec_pos_y = 3.60206;
	KeyFrameRec[19].rec_pos_z = 284.512;
	KeyFrameRec[19].rec_yaw = 344.199;
	KeyFrameRec[19].rec_pitch = 7;
	KeyFrameRec[20].rec_pos_x = 203.905;
	KeyFrameRec[20].rec_pos_y = 3.60206;
	KeyFrameRec[20].rec_pos_z = 284.512;
	KeyFrameRec[20].rec_yaw = 269.999;
	KeyFrameRec[20].rec_pitch = -3.5;
	KeyFrameRec[21].rec_pos_x = 204.131;
	KeyFrameRec[21].rec_pos_y = 3.15803;
	KeyFrameRec[21].rec_pos_z = 276.057;
	KeyFrameRec[21].rec_yaw = 302.199;
	KeyFrameRec[21].rec_pitch = -3.5;
	KeyFrameRec[22].rec_pos_x = 204.131;
	KeyFrameRec[22].rec_pos_y = 3.15803;
	KeyFrameRec[22].rec_pos_z = 276.057;
	KeyFrameRec[22].rec_yaw = 232.199;
	KeyFrameRec[22].rec_pitch = -4.2;
	KeyFrameRec[23].rec_pos_x = 202.11;
	KeyFrameRec[23].rec_pos_y = 3.0739;
	KeyFrameRec[23].rec_pos_z = 273.465;
	KeyFrameRec[23].rec_yaw = 185.999;
	KeyFrameRec[23].rec_pitch = -4.2;
	KeyFrameRec[24].rec_pos_x = 202.11;
	KeyFrameRec[24].rec_pos_y = 3.0739;
	KeyFrameRec[24].rec_pos_z = 273.465;
	KeyFrameRec[24].rec_yaw = 191.599;
	KeyFrameRec[24].rec_pitch = -2.1;
	KeyFrameRec[25].rec_pos_x = 202.11;
	KeyFrameRec[25].rec_pos_y = 3.0739;
	KeyFrameRec[25].rec_pos_z = 273.465;
	KeyFrameRec[25].rec_yaw = 59.9995;
	KeyFrameRec[25].rec_pitch = 1.4;
	KeyFrameRec[26].rec_pos_x = 206.473;
	KeyFrameRec[26].rec_pos_y = 3.28366;
	KeyFrameRec[26].rec_pos_z = 281.641;
	KeyFrameRec[26].rec_yaw = 190.199;
	KeyFrameRec[26].rec_pitch = -3.33786e-06;
	KeyFrameRec[27].rec_pos_x = 198.465;
	KeyFrameRec[27].rec_pos_y = 2.93813;
	KeyFrameRec[27].rec_pos_z = 285.747;
	KeyFrameRec[27].rec_yaw = 249.699;
	KeyFrameRec[27].rec_pitch = 6.3;
	KeyFrameRec[28].rec_pos_x = 190.894;
	KeyFrameRec[28].rec_pos_y = 3.55188;
	KeyFrameRec[28].rec_pos_z = 281.024;
	KeyFrameRec[28].rec_yaw = 98.4994;
	KeyFrameRec[28].rec_pitch = 11.2;
	KeyFrameRec[29].rec_pos_x = 190.894;
	KeyFrameRec[29].rec_pos_y = 3.55188;
	KeyFrameRec[29].rec_pos_z = 281.024;
	KeyFrameRec[29].rec_yaw = 167.799;
	KeyFrameRec[29].rec_pitch = 1.4;
	KeyFrameRec[30].rec_pos_x = 184.84;
	KeyFrameRec[30].rec_pos_y = 3.70409;
	KeyFrameRec[30].rec_pos_z = 282.488;
	KeyFrameRec[30].rec_yaw = 190.199;
	KeyFrameRec[30].rec_pitch = 1.4;
	KeyFrameRec[31].rec_pos_x = 184.84;
	KeyFrameRec[31].rec_pos_y = 3.70409;
	KeyFrameRec[31].rec_pos_z = 282.488;
	KeyFrameRec[31].rec_yaw = 190.199;
	KeyFrameRec[31].rec_pitch = 1.4;
	KeyFrameRec[32].rec_pos_x = 184.84;
	KeyFrameRec[32].rec_pos_y = 3.70409;
	KeyFrameRec[32].rec_pos_z = 282.488;
	KeyFrameRec[32].rec_yaw = 267.199;
	KeyFrameRec[32].rec_pitch = -1.4;
	KeyFrameRec[33].rec_pos_x = 184.84;
	KeyFrameRec[33].rec_pos_y = 3.70409;
	KeyFrameRec[33].rec_pos_z = 282.488;
	KeyFrameRec[33].rec_yaw = 267.199;
	KeyFrameRec[33].rec_pitch = -1.4;
	KeyFrameRec[34].rec_pos_x = 182.195;
	KeyFrameRec[34].rec_pos_y = 3.70483;
	KeyFrameRec[34].rec_pos_z = 269.286;
	KeyFrameRec[34].rec_yaw = 142.599;
	KeyFrameRec[34].rec_pitch = -1.4;
	KeyFrameRec[35].rec_pos_x = 182.195;
	KeyFrameRec[35].rec_pos_y = 3.70483;
	KeyFrameRec[35].rec_pos_z = 269.286;
	KeyFrameRec[35].rec_yaw = 59.9993;
	KeyFrameRec[35].rec_pitch = -2.8;
	KeyFrameRec[36].rec_pos_x = 184.731;
	KeyFrameRec[36].rec_pos_y = 3.41612;
	KeyFrameRec[36].rec_pos_z = 280.831;
	KeyFrameRec[36].rec_yaw = 148.199;
	KeyFrameRec[36].rec_pitch = -0.700001;
	KeyFrameRec[37].rec_pos_x = 173.522;
	KeyFrameRec[37].rec_pos_y = 3.36991;
	KeyFrameRec[37].rec_pos_z = 289.442;
	KeyFrameRec[37].rec_yaw = 148.199;
	KeyFrameRec[37].rec_pitch = -3.5;
	KeyFrameRec[38].rec_pos_x = 188.811;
	KeyFrameRec[38].rec_pos_y = 3.60214;
	KeyFrameRec[38].rec_pos_z = 290.981;
	KeyFrameRec[38].rec_yaw = 92.1993;
	KeyFrameRec[38].rec_pitch = 1.4;
	KeyFrameRec[39].rec_pos_x = 190.739;
	KeyFrameRec[39].rec_pos_y = 3.43471;
	KeyFrameRec[39].rec_pos_z = 296.979;
	KeyFrameRec[39].rec_yaw = 13.7993;
	KeyFrameRec[39].rec_pitch = -3.5;
	KeyFrameRec[40].rec_pos_x = 190.739;
	KeyFrameRec[40].rec_pos_y = 3.43471;
	KeyFrameRec[40].rec_pos_z = 296.979;
	KeyFrameRec[40].rec_yaw = 13.7993;
	KeyFrameRec[40].rec_pitch = -3.5;
	KeyFrameRec[41].rec_pos_x = 201.788;
	KeyFrameRec[41].rec_pos_y = 3.66151;
	KeyFrameRec[41].rec_pos_z = 303.089;
	KeyFrameRec[41].rec_yaw = 275.099;
	KeyFrameRec[41].rec_pitch = -16.8;
	KeyFrameRec[42].rec_pos_x = 201.788;
	KeyFrameRec[42].rec_pos_y = 3.66151;
	KeyFrameRec[42].rec_pos_z = 303.089;
	KeyFrameRec[42].rec_yaw = 329.699;
	KeyFrameRec[42].rec_pitch = -14.7;
	KeyFrameRec[43].rec_pos_x = 201.788;
	KeyFrameRec[43].rec_pos_y = 3.66151;
	KeyFrameRec[43].rec_pos_z = 303.089;
	KeyFrameRec[43].rec_yaw = 164.999;
	KeyFrameRec[43].rec_pitch = -21;
	KeyFrameRec[44].rec_pos_x = 201.788;
	KeyFrameRec[44].rec_pos_y = 3.66151;
	KeyFrameRec[44].rec_pos_z = 303.089;
	KeyFrameRec[44].rec_yaw = 100.599;
	KeyFrameRec[44].rec_pitch = -33.6;
	KeyFrameRec[45].rec_pos_x = 201.788;
	KeyFrameRec[45].rec_pos_y = 3.66151;
	KeyFrameRec[45].rec_pos_z = 303.089;
	KeyFrameRec[45].rec_yaw = 354.199;
	KeyFrameRec[45].rec_pitch = -1.4;
	KeyFrameRec[46].rec_pos_x = 205.363;
	KeyFrameRec[46].rec_pos_y = 3.69759;
	KeyFrameRec[46].rec_pos_z = 304.404;
	KeyFrameRec[46].rec_yaw = 89.3994;
	KeyFrameRec[46].rec_pitch = 1.4;
	KeyFrameRec[47].rec_pos_x = 205.316;
	KeyFrameRec[47].rec_pos_y = 3.75275;
	KeyFrameRec[47].rec_pos_z = 309.244;
	KeyFrameRec[47].rec_yaw = 132.099;
	KeyFrameRec[47].rec_pitch = -17.5;
	KeyFrameRec[48].rec_pos_x = 205.316;
	KeyFrameRec[48].rec_pos_y = 3.75275;
	KeyFrameRec[48].rec_pos_z = 309.244;
	KeyFrameRec[48].rec_yaw = 131.399;
	KeyFrameRec[48].rec_pitch = -16.8;
	KeyFrameRec[49].rec_pos_x = 205.316;
	KeyFrameRec[49].rec_pos_y = 3.75275;
	KeyFrameRec[49].rec_pos_z = 309.244;
	KeyFrameRec[49].rec_yaw = 174.099;
	KeyFrameRec[49].rec_pitch = 2.1;
	KeyFrameRec[50].rec_pos_x = 195.308;
	KeyFrameRec[50].rec_pos_y = 4.05677;
	KeyFrameRec[50].rec_pos_z = 309.753;
	KeyFrameRec[50].rec_yaw = 110.399;
	KeyFrameRec[50].rec_pitch = 2.1;
	KeyFrameRec[51].rec_pos_x = 195.308;
	KeyFrameRec[51].rec_pos_y = 4.05677;
	KeyFrameRec[51].rec_pos_z = 309.753;
	KeyFrameRec[51].rec_yaw = 112.499;
	KeyFrameRec[51].rec_pitch = 34.3;
	KeyFrameRec[52].rec_pos_x = 195.308;
	KeyFrameRec[52].rec_pos_y = 4.05677;
	KeyFrameRec[52].rec_pos_z = 309.753;
	KeyFrameRec[52].rec_yaw = 108.299;
	KeyFrameRec[52].rec_pitch = 0.699997;
	KeyFrameRec[53].rec_pos_x = 195.308;
	KeyFrameRec[53].rec_pos_y = 4.05677;
	KeyFrameRec[53].rec_pos_z = 309.753;
	KeyFrameRec[53].rec_yaw = 5.39932;
	KeyFrameRec[53].rec_pitch = -7;
	KeyFrameRec[54].rec_pos_x = 205.133;
	KeyFrameRec[54].rec_pos_y = 3.17572;
	KeyFrameRec[54].rec_pos_z = 309.503;
	KeyFrameRec[54].rec_yaw = 275.099;
	KeyFrameRec[54].rec_pitch = -9.8;
	KeyFrameRec[55].rec_pos_x = 204.977;
	KeyFrameRec[55].rec_pos_y = 3.35947;
	KeyFrameRec[55].rec_pos_z = 301.832;
	KeyFrameRec[55].rec_yaw = 353.499;
	KeyFrameRec[55].rec_pitch = 3.5;
	KeyFrameRec[56].rec_pos_x = 193.57;
	KeyFrameRec[56].rec_pos_y = 3.38388;
	KeyFrameRec[56].rec_pos_z = 297.811;
	KeyFrameRec[56].rec_yaw = 184.799;
	KeyFrameRec[56].rec_pitch = -4.29153e-06;
	KeyFrameRec[57].rec_pos_x = 188.468;
	KeyFrameRec[57].rec_pos_y = 3.13406;
	KeyFrameRec[57].rec_pos_z = 297.568;
	KeyFrameRec[57].rec_yaw = 186.899;
	KeyFrameRec[57].rec_pitch = -2.79988;
	KeyFrameRec[58].rec_pos_x = 188.468;
	KeyFrameRec[58].rec_pos_y = 3.13406;
	KeyFrameRec[58].rec_pos_z = 297.568;
	KeyFrameRec[58].rec_yaw = 186.899;
	KeyFrameRec[58].rec_pitch = -2.79988;
	KeyFrameRec[59].rec_pos_x = 179.621;
	KeyFrameRec[59].rec_pos_y = 2.66527;
	KeyFrameRec[59].rec_pos_z = 298.126;
	KeyFrameRec[59].rec_yaw = 188.999;
	KeyFrameRec[59].rec_pitch = -14.6999;
	KeyFrameRec[60].rec_pos_x = 177.417;
	KeyFrameRec[60].rec_pos_y = 3.17863;
	KeyFrameRec[60].rec_pos_z = 299.107;
	KeyFrameRec[60].rec_yaw = 312.899;
	KeyFrameRec[60].rec_pitch = -15.3999;
	KeyFrameRec[61].rec_pos_x = 182.509;
	KeyFrameRec[61].rec_pos_y = 3.0809;
	KeyFrameRec[61].rec_pos_z = 296.509;
	KeyFrameRec[61].rec_yaw = 20.7994;
	KeyFrameRec[61].rec_pitch = -4.89988;
	KeyFrameRec[62].rec_pos_x = 189.191;
	KeyFrameRec[62].rec_pos_y = 3.16561;
	KeyFrameRec[62].rec_pos_z = 298.349;
	KeyFrameRec[62].rec_yaw = 97.0994;
	KeyFrameRec[62].rec_pitch = -4.89988;
	KeyFrameRec[63].rec_pos_x = 188.963;
	KeyFrameRec[63].rec_pos_y = 3.1836;
	KeyFrameRec[63].rec_pos_z = 304.063;
	KeyFrameRec[63].rec_yaw = 182.499;
	KeyFrameRec[63].rec_pitch = -9.79988;
	KeyFrameRec[64].rec_pos_x = 185.127;
	KeyFrameRec[64].rec_pos_y = 2.99206;
	KeyFrameRec[64].rec_pos_z = 304.157;
	KeyFrameRec[64].rec_yaw = 225.199;
	KeyFrameRec[64].rec_pitch = -37.7999;
	KeyFrameRec[65].rec_pos_x = 182.693;
	KeyFrameRec[65].rec_pos_y = 3.19221;
	KeyFrameRec[65].rec_pos_z = 302.694;
	KeyFrameRec[65].rec_yaw = 102.699;
	KeyFrameRec[65].rec_pitch = -19.5999;
	KeyFrameRec[66].rec_pos_x = 180.624;
	KeyFrameRec[66].rec_pos_y = 3.25358;
	KeyFrameRec[66].rec_pos_z = 302.308;
	KeyFrameRec[66].rec_yaw = 186.699;
	KeyFrameRec[66].rec_pitch = 47.6001;
	KeyFrameRec[67].rec_pos_x = 178.503;
	KeyFrameRec[67].rec_pos_y = 3.16709;
	KeyFrameRec[67].rec_pos_z = 302.213;
	KeyFrameRec[67].rec_yaw = 98.4995;
	KeyFrameRec[67].rec_pitch = 4.90012;
	KeyFrameRec[68].rec_pos_x = 178.503;
	KeyFrameRec[68].rec_pos_y = 3.16709;
	KeyFrameRec[68].rec_pos_z = 302.213;
	KeyFrameRec[68].rec_yaw = 14.4995;
	KeyFrameRec[68].rec_pitch = -18.8999;
	KeyFrameRec[69].rec_pos_x = 188.24;
	KeyFrameRec[69].rec_pos_y = 3.36987;
	KeyFrameRec[69].rec_pos_z = 304.515;
	KeyFrameRec[69].rec_yaw = 85.8995;
	KeyFrameRec[69].rec_pitch = 1.40013;
	KeyFrameRec[70].rec_pos_x = 189.033;
	KeyFrameRec[70].rec_pos_y = 3.59676;
	KeyFrameRec[70].rec_pos_z = 313.068;
	KeyFrameRec[70].rec_yaw = 85.1995;
	KeyFrameRec[70].rec_pitch = 1.40013;
	KeyFrameRec[71].rec_pos_x = 187.699;
	KeyFrameRec[71].rec_pos_y = 3.49774;
	KeyFrameRec[71].rec_pos_z = 312.672;
	KeyFrameRec[71].rec_yaw = 209.099;
	KeyFrameRec[71].rec_pitch = -2.79988;
	KeyFrameRec[72].rec_pos_x = 184.904;
	KeyFrameRec[72].rec_pos_y = 3.35188;
	KeyFrameRec[72].rec_pos_z = 311.584;
	KeyFrameRec[72].rec_yaw = 180.4;
	KeyFrameRec[72].rec_pitch = -5.59988;
	KeyFrameRec[73].rec_pos_x = 184.904;
	KeyFrameRec[73].rec_pos_y = 3.35188;
	KeyFrameRec[73].rec_pos_z = 311.584;
	KeyFrameRec[73].rec_yaw = 142.6;
	KeyFrameRec[73].rec_pitch = 14.0001;
	KeyFrameRec[74].rec_pos_x = 184.904;
	KeyFrameRec[74].rec_pos_y = 3.35188;
	KeyFrameRec[74].rec_pos_z = 311.584;
	KeyFrameRec[74].rec_yaw = 17.9995;
	KeyFrameRec[74].rec_pitch = -2.79987;
	KeyFrameRec[75].rec_pos_x = 188.779;
	KeyFrameRec[75].rec_pos_y = 3.43;
	KeyFrameRec[75].rec_pos_z = 312.722;
	KeyFrameRec[75].rec_yaw = 92.1995;
	KeyFrameRec[75].rec_pitch = -0.699875;
	KeyFrameRec[76].rec_pos_x = 188.779;
	KeyFrameRec[76].rec_pos_y = 3.43;
	KeyFrameRec[76].rec_pos_z = 312.722;
	KeyFrameRec[76].rec_yaw = 92.1995;
	KeyFrameRec[76].rec_pitch = -0.699875;
	KeyFrameRec[77].rec_pos_x = 189.258;
	KeyFrameRec[77].rec_pos_y = 3.53116;
	KeyFrameRec[77].rec_pos_z = 291.794;
	KeyFrameRec[77].rec_yaw = 184.8;
	KeyFrameRec[77].rec_pitch = -2.09987;
	KeyFrameRec[78].rec_pos_x = 163.205;
	KeyFrameRec[78].rec_pos_y = 3.82612;
	KeyFrameRec[78].rec_pos_z = 290.736;
	KeyFrameRec[78].rec_yaw = 116.2;
	KeyFrameRec[78].rec_pitch = -0.699875;
	KeyFrameRec[79].rec_pos_x = 153.418;
	KeyFrameRec[79].rec_pos_y = 3.70826;
	KeyFrameRec[79].rec_pos_z = 297.689;
	KeyFrameRec[79].rec_yaw = 86.0996;
	KeyFrameRec[79].rec_pitch = -4.19987;
	KeyFrameRec[80].rec_pos_x = 153.418;
	KeyFrameRec[80].rec_pos_y = 3.70826;
	KeyFrameRec[80].rec_pos_z = 297.689;
	KeyFrameRec[80].rec_yaw = 86.0996;
	KeyFrameRec[80].rec_pitch = -4.19987;
	KeyFrameRec[81].rec_pos_x = 153.418;
	KeyFrameRec[81].rec_pos_y = 3.70826;
	KeyFrameRec[81].rec_pos_z = 297.689;
	KeyFrameRec[81].rec_yaw = 35.6996;
	KeyFrameRec[81].rec_pitch = 3.50013;
	KeyFrameRec[82].rec_pos_x = 164.463;
	KeyFrameRec[82].rec_pos_y = 3.7834;
	KeyFrameRec[82].rec_pos_z = 303.705;
	KeyFrameRec[82].rec_yaw = 63.6996;
	KeyFrameRec[82].rec_pitch = 7.00013;
	KeyFrameRec[83].rec_pos_x = 169.95;
	KeyFrameRec[83].rec_pos_y = 3.77711;
	KeyFrameRec[83].rec_pos_z = 304.73;
	KeyFrameRec[83].rec_yaw = 45.4996;
	KeyFrameRec[83].rec_pitch = -25.1999;
	KeyFrameRec[84].rec_pos_x = 171.517;
	KeyFrameRec[84].rec_pos_y = 3.39549;
	KeyFrameRec[84].rec_pos_z = 304.808;
	KeyFrameRec[84].rec_yaw = 315.2;
	KeyFrameRec[84].rec_pitch = -27.2999;
	KeyFrameRec[85].rec_pos_x = 171.517;
	KeyFrameRec[85].rec_pos_y = 3.39549;
	KeyFrameRec[85].rec_pos_z = 304.808;
	KeyFrameRec[85].rec_yaw = 198.3;
	KeyFrameRec[85].rec_pitch = 2.10013;
	KeyFrameRec[86].rec_pos_x = 163.603;
	KeyFrameRec[86].rec_pos_y = 3.44037;
	KeyFrameRec[86].rec_pos_z = 304.047;
	KeyFrameRec[86].rec_yaw = 243.1;
	KeyFrameRec[86].rec_pitch = -1.39987;
	KeyFrameRec[87].rec_pos_x = 155.388;
	KeyFrameRec[87].rec_pos_y = 3.4837;
	KeyFrameRec[87].rec_pos_z = 288.574;
	KeyFrameRec[87].rec_yaw = 295.6;
	KeyFrameRec[87].rec_pitch = 1.40013;
	KeyFrameRec[88].rec_pos_x = 158.336;
	KeyFrameRec[88].rec_pos_y = 3.60168;
	KeyFrameRec[88].rec_pos_z = 282.319;
	KeyFrameRec[88].rec_yaw = 243.1;
	KeyFrameRec[88].rec_pitch = -0.699875;
	KeyFrameRec[89].rec_pos_x = 158.336;
	KeyFrameRec[89].rec_pos_y = 3.60168;
	KeyFrameRec[89].rec_pos_z = 282.319;
	KeyFrameRec[89].rec_yaw = 202.5;
	KeyFrameRec[89].rec_pitch = -0.699875;
	KeyFrameRec[90].rec_pos_x = 158.336;
	KeyFrameRec[90].rec_pos_y = 3.60168;
	KeyFrameRec[90].rec_pos_z = 282.319;
	KeyFrameRec[90].rec_yaw = 202.5;
	KeyFrameRec[90].rec_pitch = -2.09987;
	KeyFrameRec[91].rec_pos_x = 158.336;
	KeyFrameRec[91].rec_pos_y = 3.60168;
	KeyFrameRec[91].rec_pos_z = 282.319;
	KeyFrameRec[91].rec_yaw = 124.1;
	KeyFrameRec[91].rec_pitch = -1.39987;
	KeyFrameRec[92].rec_pos_x = 153.084;
	KeyFrameRec[92].rec_pos_y = 3.6513;
	KeyFrameRec[92].rec_pos_z = 293.926;
	KeyFrameRec[92].rec_yaw = 138.1;
	KeyFrameRec[92].rec_pitch = -2.09987;
	KeyFrameRec[93].rec_pos_x = 147.066;
	KeyFrameRec[93].rec_pos_y = 3.58375;
	KeyFrameRec[93].rec_pos_z = 298.479;
	KeyFrameRec[93].rec_yaw = 162.6;
	KeyFrameRec[93].rec_pitch = -0.699875;
	KeyFrameRec[94].rec_pos_x = 135.291;
	KeyFrameRec[94].rec_pos_y = 4.2884;
	KeyFrameRec[94].rec_pos_z = 303.133;
	KeyFrameRec[94].rec_yaw = 232.6;
	KeyFrameRec[94].rec_pitch = -0.699874;
	KeyFrameRec[95].rec_pos_x = 135.291;
	KeyFrameRec[95].rec_pos_y = 4.2884;
	KeyFrameRec[95].rec_pos_z = 303.133;
	KeyFrameRec[95].rec_yaw = 232.6;
	KeyFrameRec[95].rec_pitch = -0.699874;
	KeyFrameRec[96].rec_pos_x = 127.395;
	KeyFrameRec[96].rec_pos_y = 6.21994;
	KeyFrameRec[96].rec_pos_z = 307.834;
	KeyFrameRec[96].rec_yaw = 146.5;
	KeyFrameRec[96].rec_pitch = -20.2999;
	KeyFrameRec[97].rec_pos_x = 127.395;
	KeyFrameRec[97].rec_pos_y = 6.21994;
	KeyFrameRec[97].rec_pos_z = 307.834;
	KeyFrameRec[97].rec_yaw = 146.5;
	KeyFrameRec[97].rec_pitch = -20.2999;
	KeyFrameRec[98].rec_pos_x = 127.395;
	KeyFrameRec[98].rec_pos_y = 6.21994;
	KeyFrameRec[98].rec_pos_z = 307.834;
	KeyFrameRec[98].rec_yaw = 54.0995;
	KeyFrameRec[98].rec_pitch = -2.09987;
	KeyFrameRec[99].rec_pos_x = 148.514;
	KeyFrameRec[99].rec_pos_y = 5.42969;
	KeyFrameRec[99].rec_pos_z = 323.169;
	KeyFrameRec[99].rec_yaw = 41.4995;
	KeyFrameRec[99].rec_pitch = -3.49987;
	KeyFrameRec[100].rec_pos_x = 155.361;
	KeyFrameRec[100].rec_pos_y = 5.5376;
	KeyFrameRec[100].rec_pos_z = 323.732;
	KeyFrameRec[100].rec_yaw = 284.6;
	KeyFrameRec[100].rec_pitch = -2.09987;
	KeyFrameRec[101].rec_pos_x = 176.518;
	KeyFrameRec[101].rec_pos_y = 5.18564;
	KeyFrameRec[101].rec_pos_z = 321.891;
	KeyFrameRec[101].rec_yaw = 288.799;
	KeyFrameRec[101].rec_pitch = -5.59987;
	KeyFrameRec[102].rec_pos_x = 189.168;
	KeyFrameRec[102].rec_pos_y = 5.26016;
	KeyFrameRec[102].rec_pos_z = 320.787;
	KeyFrameRec[102].rec_yaw = 275.5;
	KeyFrameRec[102].rec_pitch = -8.39987;
	KeyFrameRec[103].rec_pos_x = 205.93;
	KeyFrameRec[103].rec_pos_y = 3.768;
	KeyFrameRec[103].rec_pos_z = 318.372;
	KeyFrameRec[103].rec_yaw = 354.802;
	KeyFrameRec[103].rec_pitch = 2;
	KeyFrameRec[104].rec_pos_x = 229.826;
	KeyFrameRec[104].rec_pos_y = 8.14363;
	KeyFrameRec[104].rec_pos_z = 310.252;
	KeyFrameRec[104].rec_yaw = 339.402;
	KeyFrameRec[104].rec_pitch = 9.7;
	KeyFrameRec[105].rec_pos_x = 288.484;
	KeyFrameRec[105].rec_pos_y = 2.91798;
	KeyFrameRec[105].rec_pos_z = 305.052;
	KeyFrameRec[105].rec_yaw = 186.602;
	KeyFrameRec[105].rec_pitch = 0.599999;
	FrameIndexRec = 106;
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
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = mode->height;
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;
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
	Shader outsideShader("Shaders/shader_MLights.vs", "Shaders/shader_MLights.fs");
	Shader insideShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader lampShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	vector<std::string> faces2
	{
		"resources/skybox/posx.png",
		"resources/skybox/negx.png",
		"resources/skybox/posy.png",
		"resources/skybox/negy.png",
		"resources/skybox/posz.png",
		"resources/skybox/negz.png"
	};

	Skybox skybox = Skybox(faces);
	Skybox skyboxNight = Skybox(faces2);

	// Shader configuration
	// --------------------
	skyboxShader.use();

	// load models
	// -----------
	//La casa
	//Shader externo
	Model paredes_externas("resources/objects/casa1/paredes_externas2.obj");
	Model garage_externo("resources/objects/casa1/garage_externo.obj");
	Model muebles_externos("resources/objects/casa1/muebles_externos.obj");
	Model techo("resources/objects/casa1/techo.obj");
	//Shader interno
	Model paredes_internas("resources/objects/casa1/paredes_internas2.obj");
	Model shading_interno("resources/objects/casa1/shading_interno.obj");
	//Lámparas
	Model lamparas_dia("resources/objects/lamparas/lamparas.obj");
	Model lamparas_noche("resources/objects/lamparas/lamparas_noche.obj");
	//El resto de la cuadra
	Model casa2("resources/objects/casa2/casa2.obj");
	Model casa3("resources/objects/casa3/casa3.obj");
	Model casa4("resources/objects/casa4/casa4.obj");
	Model bardas("resources/objects/bardas/bardas.obj");
	Model bosque("resources/objects/bosque/bosque.obj");
	Model caminos("resources/objects/caminos/caminos.obj");
	//Postes
	Model postes_noche("resources/objects/postes/postes_noche.obj");
	Model postes_focos("resources/objects/postes/postes_focos.obj");
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
	//Inicializo timer de keyframes activables
	i_curr_steps_p = 0;
	//Inicializo frames para Recorrido
	inicializaRecorrido();

	//Inicializo la musica de fondo.
	SoundEngine->play2D("resources/audio/viv.mp3", true);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

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
		outsideShader.use();
		//Setup luces
		outsideShader.setVec3("viewPos", camera.Position);
		outsideShader.setVec3("dirLight.direction", lightDirection);
		outsideShader.setFloat("material_shininess", 32.0f);
		if(!noche)
		{//Shading de día
			outsideShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
			outsideShader.setVec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

			outsideShader.setVec3("pointLight[0].position", glm::vec3(293.042f, 15.6692f, 4.26191f));
			outsideShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[0].constant", 0.7f);
			outsideShader.setFloat("pointLight[0].linear", 0.04f);
			outsideShader.setFloat("pointLight[0].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[1].position", glm::vec3(342.68f, 16.7556f, 4.31773f));
			outsideShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[1].constant", 0.7f);
			outsideShader.setFloat("pointLight[1].linear", 0.04f);
			outsideShader.setFloat("pointLight[1].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[2].position", glm::vec3(342.37f, 16.1238f, 79.9482f));
			outsideShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[2].constant", 0.7f);
			outsideShader.setFloat("pointLight[2].linear", 0.04f);
			outsideShader.setFloat("pointLight[2].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[3].position", glm::vec3(342.5f, 16.2667f, 157.065f));
			outsideShader.setVec3("pointLight[3].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[3].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[3].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[3].constant", 0.7f);
			outsideShader.setFloat("pointLight[3].linear", 0.04f);
			outsideShader.setFloat("pointLight[3].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[4].position", glm::vec3(342.744f, 16.0782f, 232.342f));
			outsideShader.setVec3("pointLight[4].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[4].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[4].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[4].constant", 0.7f);
			outsideShader.setFloat("pointLight[4].linear", 0.04f);
			outsideShader.setFloat("pointLight[4].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[5].position", glm::vec3(342.618f, 15.9671f, 292.37f));
			outsideShader.setVec3("pointLight[5].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[5].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[5].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[5].constant", 0.7f);
			outsideShader.setFloat("pointLight[5].linear", 0.04f);
			outsideShader.setFloat("pointLight[5].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[6].position", glm::vec3(342.534f, 16.0145f, 317.269f));
			outsideShader.setVec3("pointLight[6].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[6].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[6].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[6].constant", 0.7f);
			outsideShader.setFloat("pointLight[6].linear", 0.04f);
			outsideShader.setFloat("pointLight[6].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[7].position", glm::vec3(342.559f, 16.1613f, 390.896f));
			outsideShader.setVec3("pointLight[7].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[7].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[7].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[7].constant", 0.7f);
			outsideShader.setFloat("pointLight[7].linear", 0.04f);
			outsideShader.setFloat("pointLight[7].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[8].position", glm::vec3(342.663f, 16.5252f, 454.696f));
			outsideShader.setVec3("pointLight[8].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[8].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[8].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[8].constant", 0.7f);
			outsideShader.setFloat("pointLight[8].linear", 0.04f);
			outsideShader.setFloat("pointLight[8].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[9].position", glm::vec3(342.621f, 17.2002f, 479.991f));
			outsideShader.setVec3("pointLight[9].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[9].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[9].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[9].constant", 0.7f);
			outsideShader.setFloat("pointLight[9].linear", 0.04f);
			outsideShader.setFloat("pointLight[9].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[10].position", glm::vec3(342.809f, 16.0347f, 537.113f));
			outsideShader.setVec3("pointLight[10].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[10].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[10].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[10].constant", 0.7f);
			outsideShader.setFloat("pointLight[10].linear", 0.04f);
			outsideShader.setFloat("pointLight[10].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[11].position", glm::vec3(342.596f, 16.8832f, 593.279f));
			outsideShader.setVec3("pointLight[11].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[11].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[11].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[11].constant", 0.7f);
			outsideShader.setFloat("pointLight[11].linear", 0.04f);
			outsideShader.setFloat("pointLight[11].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[12].position", glm::vec3(342.565f, 15.8552f, 649.711f));
			outsideShader.setVec3("pointLight[12].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[12].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[12].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[12].constant", 0.7f);
			outsideShader.setFloat("pointLight[12].linear", 0.04f);
			outsideShader.setFloat("pointLight[12].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[13].position", glm::vec3(342.565f, 15.7905f, 705.593f));
			outsideShader.setVec3("pointLight[13].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[13].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[13].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[13].constant", 0.7f);
			outsideShader.setFloat("pointLight[13].linear", 0.04f);
			outsideShader.setFloat("pointLight[13].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[14].position", glm::vec3(293.062f, 15.676f, 705.468f));
			outsideShader.setVec3("pointLight[14].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[14].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[14].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[14].constant", 0.7f);
			outsideShader.setFloat("pointLight[14].linear", 0.04f);
			outsideShader.setFloat("pointLight[14].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[15].position", glm::vec3(293.107f, 16.2884f, 649.48f));
			outsideShader.setVec3("pointLight[15].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[15].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[15].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[15].constant", 0.7f);
			outsideShader.setFloat("pointLight[15].linear", 0.04f);
			outsideShader.setFloat("pointLight[15].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[16].position", glm::vec3(293.174f, 16.3224f, 593.974f));
			outsideShader.setVec3("pointLight[16].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[16].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[16].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[16].constant", 0.7f);
			outsideShader.setFloat("pointLight[16].linear", 0.04f);
			outsideShader.setFloat("pointLight[16].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[17].position", glm::vec3(293.23f, 16.2697f, 537.775f));
			outsideShader.setVec3("pointLight[17].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[17].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[17].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[17].constant", 0.7f);
			outsideShader.setFloat("pointLight[17].linear", 0.04f);
			outsideShader.setFloat("pointLight[17].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[18].position", glm::vec3(293.094f, 16.276f, 480.55f));
			outsideShader.setVec3("pointLight[18].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[18].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[18].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[18].constant", 0.7f);
			outsideShader.setFloat("pointLight[18].linear", 0.04f);
			outsideShader.setFloat("pointLight[18].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[19].position", glm::vec3(293.086f, 16.048f, 455.163f));
			outsideShader.setVec3("pointLight[19].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[19].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[19].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[19].constant", 0.7f);
			outsideShader.setFloat("pointLight[19].linear", 0.04f);
			outsideShader.setFloat("pointLight[19].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[20].position", glm::vec3(293.095f, 15.9682f, 391.252f));
			outsideShader.setVec3("pointLight[20].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[20].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[20].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[20].constant", 0.7f);
			outsideShader.setFloat("pointLight[20].linear", 0.04f);
			outsideShader.setFloat("pointLight[20].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[21].position", glm::vec3(293.077f, 16.0079f, 317.017f));
			outsideShader.setVec3("pointLight[21].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[21].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[21].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[21].constant", 0.7f);
			outsideShader.setFloat("pointLight[21].linear", 0.04f);
			outsideShader.setFloat("pointLight[21].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[22].position", glm::vec3(293.063f, 15.9916f, 291.686f));
			outsideShader.setVec3("pointLight[22].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[22].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[22].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[22].constant", 0.7f);
			outsideShader.setFloat("pointLight[22].linear", 0.04f);
			outsideShader.setFloat("pointLight[22].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[23].position", glm::vec3(293.064f, 15.8211f, 232.57f));
			outsideShader.setVec3("pointLight[23].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[23].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[23].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[23].constant", 0.7f);
			outsideShader.setFloat("pointLight[23].linear", 0.04f);
			outsideShader.setFloat("pointLight[23].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[24].position", glm::vec3(293.1f, 16.187f, 156.697f));
			outsideShader.setVec3("pointLight[24].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[25].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[24].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[24].constant", 0.7f);
			outsideShader.setFloat("pointLight[24].linear", 0.04f);
			outsideShader.setFloat("pointLight[24].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[25].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[25].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[25].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[25].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[25].constant", 0.7f);
			outsideShader.setFloat("pointLight[25].linear", 0.04f);
			outsideShader.setFloat("pointLight[25].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[26].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[26].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[26].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[26].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[26].constant", 0.7f);
			outsideShader.setFloat("pointLight[26].linear", 0.04f);
			outsideShader.setFloat("pointLight[26].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[27].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[27].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[27].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[27].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[27].constant", 0.7f);
			outsideShader.setFloat("pointLight[27].linear", 0.04f);
			outsideShader.setFloat("pointLight[27].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[28].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[28].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[28].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[28].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[28].constant", 0.7f);
			outsideShader.setFloat("pointLight[28].linear", 0.04f);
			outsideShader.setFloat("pointLight[28].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[29].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[29].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[29].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[29].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[29].constant", 0.7f);
			outsideShader.setFloat("pointLight[29].linear", 0.04f);
			outsideShader.setFloat("pointLight[29].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[30].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[30].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[30].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[30].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[30].constant", 0.7f);
			outsideShader.setFloat("pointLight[30].linear", 0.04f);
			outsideShader.setFloat("pointLight[30].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[31].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[31].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[31].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[31].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[31].constant", 0.7f);
			outsideShader.setFloat("pointLight[31].linear", 0.04f);
			outsideShader.setFloat("pointLight[31].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[32].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[32].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[32].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[32].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[32].constant", 0.7f);
			outsideShader.setFloat("pointLight[32].linear", 0.04f);
			outsideShader.setFloat("pointLight[32].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[33].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[33].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[33].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[33].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[33].constant", 0.7f);
			outsideShader.setFloat("pointLight[33].linear", 0.04f);
			outsideShader.setFloat("pointLight[33].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[34].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[34].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[34].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[34].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[34].constant", 0.7f);
			outsideShader.setFloat("pointLight[34].linear", 0.04f);
			outsideShader.setFloat("pointLight[34].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[35].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[35].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[35].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[35].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[35].constant", 0.7f);
			outsideShader.setFloat("pointLight[35].linear", 0.04f);
			outsideShader.setFloat("pointLight[35].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[36].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[36].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[36].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[36].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[36].constant", 0.7f);
			outsideShader.setFloat("pointLight[36].linear", 0.04f);
			outsideShader.setFloat("pointLight[36].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[37].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[37].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[37].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setVec3("pointLight[37].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[37].constant", 0.7f);
			outsideShader.setFloat("pointLight[37].linear", 0.04f);
			outsideShader.setFloat("pointLight[37].quadratic", 0.01f);

		}
		else
		{//Shading de noche, faltan light sources aparte
			outsideShader.setVec3("dirLight.ambient", glm::vec3(0.02f, 0.02f, 0.02f));
			outsideShader.setVec3("dirLight.diffuse", glm::vec3(0.05f, 0.05f, 0.05f));
			outsideShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

			outsideShader.setVec3("pointLight[0].position", glm::vec3(293.042f, 15.6692f, 4.26191f));
			outsideShader.setVec3("pointLight[0].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[0].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[0].constant", 0.7f);
			outsideShader.setFloat("pointLight[0].linear", 0.04f);
			outsideShader.setFloat("pointLight[0].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[1].position", glm::vec3(342.68f, 16.7556f, 4.31773f));
			outsideShader.setVec3("pointLight[1].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[1].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[1].constant", 0.7f);
			outsideShader.setFloat("pointLight[1].linear", 0.04f);
			outsideShader.setFloat("pointLight[1].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[2].position", glm::vec3(342.37f, 16.1238f, 79.9482f));
			outsideShader.setVec3("pointLight[2].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[2].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[2].constant", 0.7f);
			outsideShader.setFloat("pointLight[2].linear", 0.04f);
			outsideShader.setFloat("pointLight[2].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[3].position", glm::vec3(342.5f, 16.2667f, 157.065f));
			outsideShader.setVec3("pointLight[3].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[3].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[3].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[3].constant", 0.7f);
			outsideShader.setFloat("pointLight[3].linear", 0.04f);
			outsideShader.setFloat("pointLight[3].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[4].position", glm::vec3(342.744f, 16.0782f, 232.342f));
			outsideShader.setVec3("pointLight[4].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[4].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[4].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[4].constant", 0.7f);
			outsideShader.setFloat("pointLight[4].linear", 0.04f);
			outsideShader.setFloat("pointLight[4].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[5].position", glm::vec3(342.618f, 15.9671f, 292.37f));
			outsideShader.setVec3("pointLight[5].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[5].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[5].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[5].constant", 0.7f);
			outsideShader.setFloat("pointLight[5].linear", 0.04f);
			outsideShader.setFloat("pointLight[5].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[6].position", glm::vec3(342.534f, 16.0145f, 317.269f));
			outsideShader.setVec3("pointLight[6].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[6].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[6].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[6].constant", 0.7f);
			outsideShader.setFloat("pointLight[6].linear", 0.04f);
			outsideShader.setFloat("pointLight[6].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[7].position", glm::vec3(342.559f, 16.1613f, 390.896f));
			outsideShader.setVec3("pointLight[7].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[7].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[7].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[7].constant", 0.7f);
			outsideShader.setFloat("pointLight[7].linear", 0.04f);
			outsideShader.setFloat("pointLight[7].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[8].position", glm::vec3(342.663f, 16.5252f, 454.696f));
			outsideShader.setVec3("pointLight[8].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[8].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[8].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[8].constant", 0.7f);
			outsideShader.setFloat("pointLight[8].linear", 0.04f);
			outsideShader.setFloat("pointLight[8].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[9].position", glm::vec3(342.621f, 17.2002f, 479.991f));
			outsideShader.setVec3("pointLight[9].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[9].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[9].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[9].constant", 0.7f);
			outsideShader.setFloat("pointLight[9].linear", 0.04f);
			outsideShader.setFloat("pointLight[9].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[10].position", glm::vec3(342.809f, 16.0347f, 537.113f));
			outsideShader.setVec3("pointLight[10].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[10].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[10].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[10].constant", 0.7f);
			outsideShader.setFloat("pointLight[10].linear", 0.04f);
			outsideShader.setFloat("pointLight[10].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[11].position", glm::vec3(342.596f, 16.8832f, 593.279f));
			outsideShader.setVec3("pointLight[11].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[11].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[11].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[11].constant", 0.7f);
			outsideShader.setFloat("pointLight[11].linear", 0.04f);
			outsideShader.setFloat("pointLight[11].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[12].position", glm::vec3(342.565f, 15.8552f, 649.711f));
			outsideShader.setVec3("pointLight[12].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[12].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[12].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[12].constant", 0.7f);
			outsideShader.setFloat("pointLight[12].linear", 0.04f);
			outsideShader.setFloat("pointLight[12].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[13].position", glm::vec3(342.565f, 15.7905f, 705.593f));
			outsideShader.setVec3("pointLight[13].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[13].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[13].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[13].constant", 0.7f);
			outsideShader.setFloat("pointLight[13].linear", 0.04f);
			outsideShader.setFloat("pointLight[13].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[14].position", glm::vec3(293.062f, 15.676f, 705.468f));
			outsideShader.setVec3("pointLight[14].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[14].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[14].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[14].constant", 0.7f);
			outsideShader.setFloat("pointLight[14].linear", 0.04f);
			outsideShader.setFloat("pointLight[14].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[15].position", glm::vec3(293.107f, 16.2884f, 649.48f));
			outsideShader.setVec3("pointLight[15].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[15].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[15].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[15].constant", 0.7f);
			outsideShader.setFloat("pointLight[15].linear", 0.04f);
			outsideShader.setFloat("pointLight[15].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[16].position", glm::vec3(293.174f, 16.3224f, 593.974f));
			outsideShader.setVec3("pointLight[16].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[16].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[16].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[16].constant", 0.7f);
			outsideShader.setFloat("pointLight[16].linear", 0.04f);
			outsideShader.setFloat("pointLight[16].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[17].position", glm::vec3(293.23f, 16.2697f, 537.775f));
			outsideShader.setVec3("pointLight[17].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[17].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[17].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[17].constant", 0.7f);
			outsideShader.setFloat("pointLight[17].linear", 0.04f);
			outsideShader.setFloat("pointLight[17].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[18].position", glm::vec3(293.094f, 16.276f, 480.55f));
			outsideShader.setVec3("pointLight[18].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[18].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[18].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[18].constant", 0.7f);
			outsideShader.setFloat("pointLight[18].linear", 0.04f);
			outsideShader.setFloat("pointLight[18].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[19].position", glm::vec3(293.086f, 16.048f, 455.163f));
			outsideShader.setVec3("pointLight[19].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[19].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[19].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[19].constant", 0.7f);
			outsideShader.setFloat("pointLight[19].linear", 0.04f);
			outsideShader.setFloat("pointLight[19].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[20].position", glm::vec3(293.095f, 15.9682f, 391.252f));
			outsideShader.setVec3("pointLight[20].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[20].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[20].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[20].constant", 0.7f);
			outsideShader.setFloat("pointLight[20].linear", 0.04f);
			outsideShader.setFloat("pointLight[20].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[21].position", glm::vec3(293.077f, 16.0079f, 317.017f));
			outsideShader.setVec3("pointLight[21].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[21].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[21].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[21].constant", 0.7f);
			outsideShader.setFloat("pointLight[21].linear", 0.04f);
			outsideShader.setFloat("pointLight[21].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[22].position", glm::vec3(293.063f, 15.9916f, 291.686f));
			outsideShader.setVec3("pointLight[22].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[22].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[22].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[22].constant", 0.7f);
			outsideShader.setFloat("pointLight[22].linear", 0.04f);
			outsideShader.setFloat("pointLight[22].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[23].position", glm::vec3(293.064f, 15.8211f, 232.57f));
			outsideShader.setVec3("pointLight[23].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[23].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[23].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[23].constant", 0.7f);
			outsideShader.setFloat("pointLight[23].linear", 0.04f);
			outsideShader.setFloat("pointLight[23].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[24].position", glm::vec3(293.1f, 16.187f, 156.697f));
			outsideShader.setVec3("pointLight[24].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[25].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[24].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[24].constant", 0.7f);
			outsideShader.setFloat("pointLight[24].linear", 0.04f);
			outsideShader.setFloat("pointLight[24].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[25].position", glm::vec3(293.12f, 15.8822f, 80.4954f));
			outsideShader.setVec3("pointLight[25].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
			outsideShader.setVec3("pointLight[25].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			outsideShader.setVec3("pointLight[25].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[25].constant", 0.7f);
			outsideShader.setFloat("pointLight[25].linear", 0.04f);
			outsideShader.setFloat("pointLight[25].quadratic", 0.01f);

			//Casa

			outsideShader.setVec3("pointLight[26].position", glm::vec3(211.121f, 5.94826f, 282.423f));
			outsideShader.setVec3("pointLight[26].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[26].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[26].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[26].constant", 0.95f);
			outsideShader.setFloat("pointLight[26].linear", 0.04f);
			outsideShader.setFloat("pointLight[26].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[27].position", glm::vec3(208.612f, 3.02237f, 290.348f));
			outsideShader.setVec3("pointLight[27].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[27].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[27].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[27].constant", 0.95f);
			outsideShader.setFloat("pointLight[27].linear", 0.04f);
			outsideShader.setFloat("pointLight[27].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[28].position", glm::vec3(210.852f, 4.31604f, 300.805f));
			outsideShader.setVec3("pointLight[28].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[28].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[28].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[28].constant", 0.95f);
			outsideShader.setFloat("pointLight[28].linear", 0.04f);
			outsideShader.setFloat("pointLight[28].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[29].position", glm::vec3(189.741f, 4.73011f, 315.835f));
			outsideShader.setVec3("pointLight[29].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[29].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[29].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[29].constant", 0.95f);
			outsideShader.setFloat("pointLight[29].linear", 0.04f);
			outsideShader.setFloat("pointLight[29].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[30].position", glm::vec3(178.32f, 4.12308f, 315.864f));
			outsideShader.setVec3("pointLight[30].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[30].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[30].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[30].constant", 0.95f);
			outsideShader.setFloat("pointLight[30].linear", 0.04f);
			outsideShader.setFloat("pointLight[30].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[31].position", glm::vec3(162.546f, 4.49479f, 312.609f));
			outsideShader.setVec3("pointLight[31].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[31].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[31].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[31].constant", 0.95f);
			outsideShader.setFloat("pointLight[31].linear", 0.04f);
			outsideShader.setFloat("pointLight[31].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[32].position", glm::vec3(158.357f, 5.26351f, 314.634f));
			outsideShader.setVec3("pointLight[32].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[32].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[32].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[32].constant", 0.95f);
			outsideShader.setFloat("pointLight[32].linear", 0.04f);
			outsideShader.setFloat("pointLight[32].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[33].position", glm::vec3(153.934f, 4.822f, 314.155f));
			outsideShader.setVec3("pointLight[33].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[33].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[33].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[33].constant", 0.95f);
			outsideShader.setFloat("pointLight[33].linear", 0.04f);
			outsideShader.setFloat("pointLight[33].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[34].position", glm::vec3(149.119f, 2.82754f, 299.724f));
			outsideShader.setVec3("pointLight[34].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[34].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[34].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[34].constant", 0.95f);
			outsideShader.setFloat("pointLight[34].linear", 0.04f);
			outsideShader.setFloat("pointLight[35].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[35].position", glm::vec3(169.745f, 5.57316f, 269.044f));
			outsideShader.setVec3("pointLight[35].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[35].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[35].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[35].constant", 0.95f);
			outsideShader.setFloat("pointLight[35].linear", 0.04f);
			outsideShader.setFloat("pointLight[35].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[36].position", glm::vec3(173.893f, 5.3118f, 268.885f));
			outsideShader.setVec3("pointLight[36].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[36].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[36].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[36].constant", 0.95f);
			outsideShader.setFloat("pointLight[36].linear", 0.04f);
			outsideShader.setFloat("pointLight[36].quadratic", 0.01f);

			outsideShader.setVec3("pointLight[37].position", glm::vec3(178.113f, 5.26151f, 268.561f));
			outsideShader.setVec3("pointLight[37].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
			outsideShader.setVec3("pointLight[37].diffuse", glm::vec3(0.14f, 0.14f, 0.14f));
			outsideShader.setVec3("pointLight[37].specular", glm::vec3(0.0f, 0.0f, 0.0f));
			outsideShader.setFloat("pointLight[37].constant", 0.95f);
			outsideShader.setFloat("pointLight[37].linear", 0.04f);
			outsideShader.setFloat("pointLight[37].quadratic", 0.01f);
		}
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		glm::mat4 tmp2 = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, pavement_height, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		outsideShader.setMat4("projection", projection);
		outsideShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

		//--------------------
		// SHADING EXTERNO
		//--------------------

		outsideShader.setVec3("aColor", 0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 300.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		outsideShader.setMat4("model", model);
		bardas.Draw(outsideShader);
		bosque.Draw(outsideShader);
		caminos.Draw(outsideShader);
		//De casa 1
		garage_externo.Draw(outsideShader);
		techo.Draw(outsideShader);
		muebles_externos.Draw(outsideShader);
		paredes_externas.Draw(outsideShader);
		//Terminan cosas de casa 1
		casa2.Draw(outsideShader);
		casa3.Draw(outsideShader);
		casa4.Draw(outsideShader);
		postes_noche.Draw(outsideShader);
		if (!noche)
		{
			postes_focos.Draw(outsideShader);
		}


		//Animacion 2: Pelota
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(pel_x, pel_y, pel_z));
		tmp = glm::rotate(tmp, glm::radians(pel_rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		outsideShader.setMat4("model", tmp);
		pelota.Draw(outsideShader);
		//Animacion 3: Avión de papel
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(336.0f, 25.0f, 45.0f));//pos inicial
		model = glm::translate(model, glm::vec3(avion_pos_x, avion_pos_y, avion_pos_z));
		model = glm::rotate(model, glm::radians(-avion_rot), glm::vec3(1.0f, 0.0f, 0.0f));
		outsideShader.setMat4("model", model);
		avion.Draw(outsideShader);
		//Animacion 5: Carro
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(movAuto_x, 0.0f, movAuto_z));
		tmp = model = glm::rotate(model, glm::radians(270.0f + orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		outsideShader.setMat4("model", model);
		carro.Draw(outsideShader);

		model = glm::translate(tmp, glm::vec3(8.5f * 0.4f, 3.5f * 0.4f, 12.9f * 0.4f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		model = glm::rotate(model, glm::radians(-rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		outsideShader.setMat4("model", model);
		llanta.Draw(outsideShader);	//Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.5f * 0.4f, 3.5f * 0.4f, 12.9f * 0.4f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		outsideShader.setMat4("model", model);
		llanta.Draw(outsideShader);	//Der delantera

		model = glm::translate(tmp, glm::vec3(-8.5f * 0.4f, 4.0f * 0.4f, -14.5f * 0.4f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		outsideShader.setMat4("model", model);
		llanta.Draw(outsideShader);	//Der trasera

		model = glm::translate(tmp, glm::vec3(8.5f * 0.4f, 4.0f * 0.4f, -14.5f * 0.4f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		model = glm::rotate(model, glm::radians(-rot_llanta), glm::vec3(1.0f, 0.0f, 0.0f));
		outsideShader.setMat4("model", model);
		llanta.Draw(outsideShader);	//Izq trase

		//ESCENARIO
		model = glm::mat4(1.0f);
		outsideShader.setVec3("viewPos", camera.Position);
		outsideShader.setMat4("model", model);
		outsideShader.setMat4("view", view);
		outsideShader.setMat4("projection", projection);
		drawingGrass();
		outsideShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		outsideShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_pasto);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		drawingStreet();
		outsideShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		outsideShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_acera);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, t_pavimento);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(36 * sizeof(float)));
		//--------------------
		// SHADING INTERNO
		//--------------------

		insideShader.use();
		//Setup shader
		insideShader.setMat4("projection", projection);
		insideShader.setMat4("view", view);
		insideShader.setVec3("viewPos", camera.Position);
		insideShader.setVec3("dirLight.direction", insideLightDirection);
		insideShader.setFloat("material_shininess", 32.0f);
		if(noche)
		{
			
			insideShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
			insideShader.setVec3("dirLight.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
			insideShader.setVec3("dirLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));
		}
		else
		{
			insideShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
			insideShader.setVec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
			insideShader.setVec3("dirLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));
		}
		insideShader.setVec3("pointLight[0].position", lightPosition);
		insideShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		insideShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		insideShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		insideShader.setFloat("pointLight[0].constant", 0.8f);
		insideShader.setFloat("pointLight[0].linear", 0.009f);
		insideShader.setFloat("pointLight[0].quadratic", 0.032f);
		//Finaliza setup
		insideShader.setVec3("aColor", 0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 300.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		insideShader.setMat4("model", model);
		//Casa 1, interno
		paredes_internas.Draw(insideShader);
		if(!noche)
		{
			lamparas_dia.Draw(insideShader);
		}
		shading_interno.Draw(insideShader);

		//Animacion 4: Robot aspiradora
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(robot_pos_x, 0.3f, robot_pos_z));
		model = glm::scale(model, glm::vec3(0.333f));
		model = glm::rotate(model, glm::radians(robot_rot_y - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		insideShader.setMat4("model", model);
		robot.Draw(insideShader);
		tmp = glm::translate(model, glm::vec3(1.348f, -0.592f, 0.839f));
		tmp = glm::rotate(tmp, glm::radians(-rotor_rotacion), glm::vec3(0.0f, 1.0f, 0.0f));
		insideShader.setMat4("model", tmp);
		robot_rotor_izq.Draw(insideShader);
		tmp = glm::translate(model, glm::vec3(-1.351f, -0.592f, 0.839f));
		tmp = glm::rotate(tmp, glm::radians(rotor_rotacion), glm::vec3(0.0f, 1.0f, 0.0f));
		insideShader.setMat4("model", tmp);
		robot_rotor_der.Draw(insideShader);

		/// Shader de light sources

		lampShader.use();
		//Setup luces
		lampShader.setVec3("viewPos", camera.Position);
		lampShader.setVec3("dirLight.direction", insideLightDirection);
		lampShader.setFloat("material_shininess", 32.0f);
		//Setup shader
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		//Finaliza setup
		lampShader.setVec3("aColor", 0.0f, 0.0f, 0.0f);
		lampShader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		lampShader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		lampShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		lampShader.setVec3("pointLight[0].position", glm::vec3(-20.0f, -20.0f, -20.0f));
		lampShader.setVec3("pointLight[0].ambient", glm::vec3(0.6f, 0.6f, 0.6f));
		lampShader.setVec3("pointLight[0].diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		lampShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		lampShader.setFloat("pointLight[0].constant", 1.0f);
		lampShader.setFloat("pointLight[0].linear", 0.04f);
		lampShader.setFloat("pointLight[0].quadratic", 0.01f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 300.0f));
		model = glm::scale(model, glm::vec3(0.35f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//Animacion 1: PONG
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(0.0f, pong_l_pos, 0.0f));
		lampShader.setMat4("model", tmp);
		pong_l.Draw(lampShader);
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(0.0f, pong_r_pos, 0.0f));
		lampShader.setMat4("model", tmp);
		pong_r.Draw(lampShader);
		tmp = model;
		tmp = glm::translate(tmp, glm::vec3(pong_b_xpos, pong_b_ypos, 0.0f));
		lampShader.setMat4("model", tmp);
		pong_b.Draw(lampShader);
		if(noche)
		{
			lampShader.setMat4("model", model);
			lamparas_dia.Draw(lampShader);
			postes_focos.Draw(lampShader);
		}
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		skyboxShader.use();
		if (noche)
		{
			skyboxShader.setInt("skybox", 1);
			skyboxNight.Draw(skyboxShader, view, projection, camera);
		}
		else
		{
			skyboxShader.setInt("skybox", 0);
			skybox.Draw(skyboxShader, view, projection, camera);
		}
		

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
	skyboxNight.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, (float)deltaTime * speedMultiplier);
		playIndexRec = 0;
		resetRecorridoElements();
		interpolationRecorrido();
		play = false;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime * speedMultiplier);
		playIndexRec = 0;
		resetRecorridoElements();
		interpolationRecorrido();
		play = false;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, (float)deltaTime * speedMultiplier);
		playIndexRec = 0;
		resetRecorridoElements();
		interpolationRecorrido();
		play = false;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, (float)deltaTime * speedMultiplier);
		playIndexRec = 0;
		resetRecorridoElements();
		interpolationRecorrido();
		play = false;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		speedMultiplier *= 2;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		speedMultiplier /= 2;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		noche = !noche;
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
		std::cout << "(" << camera.Position.x << "f, " << camera.Position.y << "f, " << camera.Position.z << "f);" << std::endl;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndexRec > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetRecorridoElements();
			//First Interpolation				
			interpolationRecorrido();

			play = true;
			playIndexRec = 0;
			i_curr_steps_p = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	////To Save a KeyFramePong
	//if (key == GLFW_KEY_L && action == GLFW_PRESS)
	//{
	//	if (FrameIndexRec < MAX_FRAMES_RECORRIDO)
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
	if (!play)
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
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}