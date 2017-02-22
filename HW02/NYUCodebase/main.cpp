#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

using namespace std;

SDL_Window *displayWindow;

struct Paddle {
	Paddle(float lt, float rt, float tp, float bt) : left(lt), right(rt), top(tp), bottom(bt) {}
	float left;
	float right;
	float top;
	float bottom;
};

struct Ball {
	Ball(float px, float py, float sp, float dx, float dy) : positionX(px), positionY(py), speed(sp), directionX(dx), directionY(dy) {}
	float positionX = 0.0f;
	float positionY = 0.0f;
	float speed = 0.1f;
	float directionX = (float)rand();
	float directionY = (float)rand();

	void move() {
		positionX += (speed * directionX);
		positionY += (speed * directionY);
	}
};

ShaderProgram setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 360);
	return ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
}

int main(int argc, char *argv[]) {
	ShaderProgram program = setup();

	Matrix paddleLeftM;
	Matrix paddleRightM;
	Matrix ballM;
	Matrix viewM;
	Matrix gameM;

	gameM.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	SDL_Event event;
	bool done = false;
	bool start = false;

	Paddle paddleLeft(-3.5f, -3.4f, 0.5f, -0.5f);
	Paddle paddleRight(3.4f, 3.5f, 0.5f, -0.5f);
	Ball ball(0.0f, 0.0f, 0.5f, 0.05, 0.05);

	float rightTranslate = 0.0f;
	float leftTranslate = 0.0f;
	float lastFrameTicks = 0.0f;
	glUseProgram(program.programID);

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					start = true;
				}
			}
		}
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		program.setModelMatrix(paddleLeftM);
		program.setViewMatrix(viewM);
		program.setProjectionMatrix(gameM);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT);

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_W]) {
			if (paddleLeft.top < 2.0f){
				paddleLeft.top += 0.05f;
				paddleLeft.bottom += 0.05f;
				paddleLeftM.Translate(0.0f, 0.05f, 0.0f);
			}
		}
		if (keys[SDL_SCANCODE_S]) {
			if (paddleLeft.bottom > -2.0f){
				paddleLeft.top -= 0.05f;
				paddleLeft.bottom -= 0.05f;
				paddleLeftM.Translate(0.0f, -0.05f, 0.0f);
			}
		}

		if (keys[SDL_SCANCODE_UP]) {
			if (paddleRight.top < 2.0f){
				paddleRight.top += 0.05f;
				paddleRight.bottom += 0.05f;
				paddleRightM.Translate(0.0f, 0.05f, 0.0f);
			}
		}

		if (keys[SDL_SCANCODE_DOWN]) {
			if (paddleRight.bottom > -2.0f){
				paddleRight.top -= 0.05f;
				paddleRight.bottom -= 0.05f;
				paddleRightM.Translate(0.0f, -0.05f, 0.0f);
			}
		}

		program.setModelMatrix(paddleLeftM);
		float leftVertices[] = { -3.5f, -0.5f, -3.4f, -0.5f, -3.4f, 0.5f, -3.4f, 0.5f, -3.5f, 0.5f, -3.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, leftVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		program.setModelMatrix(paddleRightM);
		float rightVertices[] = { 3.4f, -0.5f, 3.5f, -0.5f, 3.5f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rightVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		program.setModelMatrix(ballM);
		float ballVertices[] = { -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, -0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		if (start) {
			if (ball.positionX + 0.1f < paddleLeft.right) {
				start = false;
				ballM.Translate(-ball.positionX, -ball.positionY, 0.0f);
				ball.positionX = 0.0f;
				ball.positionY = 0.0f;
				ball.directionX = 0.05f;
				ball.directionY = 0.05f;
				ball.speed = 0.5f;
			}

			else if (ball.positionX - 0.1f > paddleRight.left) {
				start = false;
				ballM.Translate(-ball.positionX, -ball.positionY, 0.0f);
				ball.positionX = 0.0f;
				ball.positionY = 0.0f;
				ball.directionX = 0.05f;
				ball.directionY = 0.05f;
				ball.speed = 0.5f;
			}

			else if (ball.positionY + 0.1f >= 2.0f || ball.positionY - 0.1f <= -2.0f) {
				ball.directionY *= -1;
				ball.move();
				ballM.Translate(ball.speed * ball.directionX, ball.speed * ball.directionY, 0.0f);
			}

			else if ((ball.positionX >= paddleRight.left && ball.positionY >= paddleRight.bottom
				&& ball.positionY <= paddleRight.top) || (ball.positionX <= paddleLeft.right
				&& ball.positionY <= paddleLeft.top && ball.positionY >= paddleLeft.bottom)) {
				ball.directionX *= -1;
				ball.move();
				ballM.Translate((ball.speed * ball.directionX), (ball.speed * ball.directionY), 0.0f);
			}

			else {
				ball.move();
				ballM.Translate((ball.speed * ball.directionX), (ball.speed * ball.directionY), 0.0f);
			}
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}