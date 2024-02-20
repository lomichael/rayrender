#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RES_SCALE 4
#define MAP_WIDTH 24 
#define MAP_HEIGHT 24 
#define WINDOW_WIDTH (320 * RES_SCALE)
#define WINDOW_HEIGHT (240 * RES_SCALE)

// global map state
int map[MAP_WIDTH][MAP_HEIGHT] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

// global player state
float playerX = 5.0f, playerY = 5.0f; // player start position
float dirX = -1.0f, dirY = 0.0f; // initial direction vector
float planeX = 0.0, planeY = 0.66f; // 2D raycaster camera plane

void castRays(GLFWwindow* window) {
	for (int x = 0; x < WINDOW_WIDTH; x++) {
		// for each column of pixels, calculate the projected ray position and direction
		// x-coordinate in camera space
		float cameraX = 2 * x / (float)WINDOW_WIDTH - 1;
		float rayDirX = dirX + planeX * cameraX;
		float rayDirY = dirY + planeY * cameraX;

		// find the cell of map player is in
		int mapX = (int)playerX;
		int mapY = (int)playerY;

		// length of ray from current position to next x or y side
		float sideDistX;
		float sideDistY;

		// length of ray from one x-side to next x-side or y-side to next y-side
		float deltaDistX = fabs(1 / rayDirX);
		float deltaDistY = fabs(1 / rayDirY);
		float perpWallDist;

		// what direction to step in x or y direction
		int stepX;
		int stepY;

		// check if a wall was hit
		int hit = 0;
		// check if north/south or east/west wall was hit
		int side;

		// calculate the step and initial side distance
		if (rayDirX < 0) {
			stepX = -1;
			sideDistX = (playerX - mapX) * deltaDistX;
		} else {
			stepX = 1;
			sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
		}
		if (rayDirY < 0) {
			stepY = -1;
			sideDistY = (playerY - mapY) * deltaDistY;
		} else {
			stepY = 1;
			sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
		}

		// perform DDA
		while (hit == 0) {
			// jump to next map square or in x-direction or in y-direction
			if (sideDistX < sideDistY) {
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			} else {
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}

			// check if the ray has hit a wall
			if (map[mapX][mapY] > 0) hit = 1;
		}

		// calculate distance projected on camera direction
		// if (side == 0) perpWallDist = (mapX - playerX + (1 - stepX) / 2) / rayDirX; 
		// else perpWallDist = (mapY - playerY + (1 - stepY) / 2) / rayDirY;	
		if (side == 0) perpWallDist = (sideDistX - deltaDistX);
		else perpWallDist = (sideDistY - deltaDistY);

		// calculate height of line to draw on screen
		int lineHeight = (int)(WINDOW_HEIGHT / perpWallDist);

		// calculate lowest and highest pixel to fill in the current stripe
		int drawStart = -lineHeight / 2 + WINDOW_HEIGHT / 2;
		if (drawStart < 0) drawStart = 0;
		int drawEnd = lineHeight / 2 + WINDOW_HEIGHT / 2;
		if (drawEnd >= WINDOW_HEIGHT) drawEnd = WINDOW_HEIGHT - 1;

		// choose a wall color
		float r, g, b;
		switch(map[mapX][mapY]) {
			case 1: // red
				r = 1.0;
				g = 0.0;
				b = 0.0;
				break;
			case 2: // green 
				r = 0.0;
				g = 1.0;
				b = 0.0;
				break;
			case 3: // blue 
				r = 0.0;
				g = 0.0;
				b = 1.0;
				break;
			case 4: // white
				r = 1.0;
				g = 1.0;
				b = 1.0;
				break;
			default: // yellow 
				r = 1.0; 
				g = 1.0;
				b = 0.0;
				break; 
		}

		// give the x and y sides different brightness
		if (side == 1) {
			r = r / 2;
			g = g / 2;
			b = b / 2;
		}

		// draw the pixels of the stripe as a vertical line on the screen
		glBegin(GL_QUADS);
		glColor3f(r, g, b);
		glVertex2f(x, drawStart);
		glVertex2f(x, drawEnd);
		glVertex2f(x + 1, drawEnd);
		glVertex2f(x + 1, drawStart);
		glEnd();
	}
}

void processInput(GLFWwindow* window) {
	// move speed modifier in squares/second
	const float moveSpeed = 0.05f;
	// rotate speed modifier in radians/second
	const float rotSpeed = 0.05f;

	// move forward 
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		// check if wall is ahead
		if (map[(int)(playerX + dirX * moveSpeed)][(int)(playerY)] == 0) playerX += dirX * moveSpeed;
		if (map[(int)(playerX)][(int)(playerY + dirY * moveSpeed)] == 0) playerY += dirY * moveSpeed;
	}	
	// move backwards
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// check if wall is behind
		if (map[(int)(playerX - dirX * moveSpeed)][(int)(playerY)] == 0) playerX -= dirX * moveSpeed;
		if (map[(int)(playerX)][(int)(playerY - dirY * moveSpeed)] == 0) playerY -= dirY * moveSpeed;
	}	
	// rotate to the right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		float oldDirX = dirX;
		dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
		dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
		float oldPlaneX = planeX;
		planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
		planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
	}	
	// rotate to the left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		float oldDirX = dirX;
		dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
		dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
		float oldPlaneX = planeX;
		planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
		planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
	}
}

int main() {
	// the window object we work with
	GLFWwindow* window;

	// initialize GLFW
	if (!glfwInit()) {
		printf("Failed to initialize GLFW\n");
		return -1;
	}
	
	// create a window and context
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Application", NULL, NULL);
	if (!window) {
		glfwTerminate();
		printf("Failed to create GLFW window\n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	// enable vsync
	glfwSwapInterval(1);

	// window loop
	while (!glfwWindowShouldClose(window)) {
		// handle input 
		processInput(window);

		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

		// raycast 
		castRays(window);
	
		// idk	
		glfwSwapBuffers(window);

		// poll a queue for events
		glfwPollEvents();
	}

	// teardown
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
