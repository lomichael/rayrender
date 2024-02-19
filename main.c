#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

// 1 represents walls, 0 represents empty space
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// player state
float posX = 22, posY = 12;
float dirX = -1, dirY = 0;
float planeX = 0, planeY = 0.66; // raycaster camera plane

void renderFrame(GLFWwindow* window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int x = 0; x < width; x++) {
		// x-coordinate in camera space
		double cameraX = 2 * x / (double)width - 1;
		
		// calculate ray pos and dir
		double rayDirX = dirX + planeX * cameraX;
		double rayDirY = dirY + planeY * cameraX;

		// calculate which cell of map we are in
		int mapX = (int)posX;
		int mapY = (int)posY;

		// length of ray from one x-side to next x-side, or y-side to next y-side
		double sideDistX;
		double sideDistY;

		// length of ray from current position to next x-side or y-side
		double deltaDistX = fabs(1 / rayDirX);
		double deltaDistY = fabs(1 / rayDirY);
		double perpWallDist;

		// direction to step in x or y direction (-1 or +1)
		int stepX, stepY;

		int hit = 0; // check wall hit
		int side; // check boundary wall hit (top, bottom, left, right)
		
		// calculate step and initial sideDist
		if (rayDirX < 0) {
			stepX = -1;
			sideDistX = (posX - mapX) * deltaDistX;
		} else {
			stepX = 1;
			sideDistX = (mapX + 1.0 - posX) * deltaDistX;
		}
		if (rayDirY < 0) {
			stepY = -1;
			sideDistY = (posY - mapY) * deltaDistY;
		} else {
			stepY = 1;
			sideDistY = (mapY + 1.0 - posY) * deltaDistY;
		}

		// perform DDA
		while (hit == 0) {
			// jump to next map cell in x-direction or y-direction
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
			if (worldMap[mapX][mapY] > 0) {
				hit = 1;
			}
		}

		// calculate distance projected on camera direction (Euclidean gives fisheye)
		if (side == 0) {
			perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
		} else {
			perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;
		}

		// calculate height of line to draw on screen
		int lineHeight = (int)(height / perpWallDist);

		// calculate lowest and highest pixel to fill in current stripe
		int drawStart = (-lineHeight / 2) + (height / 2);
		if (drawStart < 0) {
			drawStart = 0;
		}

		int drawEnd = lineHeight / 2 + height / 2;
		if (drawEnd >= height) {
			drawEnd = height - 1;
		}

		// choose the wall color
		float color;
		switch(worldMap[mapX][mapY]) {
			case 1: color = 0.7f; break; // red walls
			default: color = 1.0f; break; // white walls
		}

		// give x and y sides different brightness
		if (side == 1) {
			color = color * 0.7f;
		}

		// draw pixels of stripe as vertical line
		glBegin(GL_QUADS);
		glColor3f(color, color, color); 
		glVertex2f(x, drawStart);
		glVertex2f(x, drawEnd);
		glVertex2f(x + 1, drawEnd);
		glVertex2f(x + 1, drawStart);
		glEnd();
	}

	glfwSwapBuffers(window);
}

void updatePlayerPosition(GLFWwindow *window, float deltaTime) {
	// speed modifiers
	float moveSpeed = deltaTime * 1.0f; // squares/second
	float rotateSpeed = deltaTime * 3.0f; // radians/second

	// move forward if no wall is in front of player
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!worldMap[(int)(posX + dirX * moveSpeed)][(int)(posY)]) {
			posX += dirX * moveSpeed;
		}
		if (!worldMap[(int)(posX)][(int)(posY + dirY * moveSpeed)]) {
			posY += dirY * moveSpeed;
		}
	}
	// move backward if no wall is behind player	
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!worldMap[(int)(posX - dirX * moveSpeed)][(int)(posY)]) {
			posX -= dirX * moveSpeed;
		}
		if (!worldMap[(int)(posX)][(int)(posY - dirY * moveSpeed)]) {
			posY -= dirY * moveSpeed;
		}
	}

	// rotate to the right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		float oldDirX = dirX;
		dirX = dirX * cos(-rotateSpeed) - dirY * sin(-rotateSpeed);
		dirY = oldDirX * sin(-rotateSpeed) + dirY * cos(-rotateSpeed);

		float oldPlaneX = planeX;
		planeX = planeX * cos(-rotateSpeed) - planeY * sin(-rotateSpeed);
		planeY = oldPlaneX * sin(-rotateSpeed) + planeY * cos(-rotateSpeed);
	}	
	// rotate to the left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		float oldDirX = dirX;
		dirX = dirX * cos(rotateSpeed) - dirY * sin(rotateSpeed);
		dirY = oldDirX * sin(rotateSpeed) + dirY * cos(rotateSpeed);

		float oldPlaneX = planeX;
		planeX = planeX * cos(rotateSpeed) - planeY * sin(rotateSpeed);
		planeY = oldPlaneX * sin(rotateSpeed) + planeY * cos(rotateSpeed);
	}		
}

int main(void) {
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(640, 480, "Raycasting Engine", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glOrtho(0, 640, 480, 0, -1, 1); // simple 2D orthographic projection
	
	// caclulating deltaTime
	float lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// update
		updatePlayerPosition(window, deltaTime);

		// render
		renderFrame(window);
	
		// poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
