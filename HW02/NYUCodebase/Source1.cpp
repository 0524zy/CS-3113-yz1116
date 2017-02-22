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

SDL_Window *displayWindow;

struct Paddle {
	Paddle(float lt, float rt, float tp, float bt) : left(lt), right(rt), top(tp), bottom(bt) {}
	float left;
	float right;
	float top;
	float bottom;
};

struct Ball {
	Ball(float px, float py, float sp, float ac, float dx, float dy) :
		position_x(px), position_y(py), speed(sp), acceleration(ac), direction_x(dx), direction_y(dy) {}
	float position_x = 0.0f;
	float position_y = 0.0f;
	float speed = 0.05f;
	float acceleration = 0.0025f;
	float direction_x = (float)rand();
	float direction_y = (float)rand();

	void move() {
		position_x += (speed * direction_x);
		position_y += (speed * direction_y);
	}
};

ShaderProgram setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Noman Hamlani - Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 360);
	return ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
}

int main(int argc, char *argv[])
{
	ShaderProgram program = setup();

	Matrix paddle_left_matrix;
	Matrix paddle_right_matrix;
	Matrix ball_matrix;
	Matrix view_matrix;
	Matrix projection_matrix;

	projection_matrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	SDL_Event event;
	bool done = false;
	bool game_started = false;

	Paddle paddle_left(-3.5f, -3.4f, 0.5f, -0.5f);
	Paddle paddle_right(3.4f, 3.5f, 0.5f, -0.5f);
	Ball ball(0.0f, 0.0f, 0.05f, 0.0025f, 0.05, 0.01);

	float rightTranslate = 0.0f;
	float leftTranslate = 0.0f;
	float last_frame_ticks = 0.0f;
	glUseProgram(program.programID);

	while (!done)
	{
		//Controls for the game
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - last_frame_ticks;
		last_frame_ticks = ticks;

		program.setModelMatrix(paddle_left_matrix);
		program.setViewMatrix(view_matrix);
		program.setProjectionMatrix(projection_matrix);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		//left paddle
		if (keys[SDL_SCANCODE_W]) {
			if (paddle_left.top < 2.0f){
				paddle_left.top += 0.005f;
				paddle_left.bottom += 0.005f;
				paddle_left_matrix.Translate(0.0f, 0.005, 0.0f);
			}
		}
		if (keys[SDL_SCANCODE_S]) {
			if (paddle_left.bottom > -2.0f){
				paddle_left.top -= 0.005f;
				paddle_left.bottom -= 0.005f;
				paddle_left_matrix.Translate(0.0f, -0.005f, 0.0f);
			}
		}

		//right paddle
		if (keys[SDL_SCANCODE_UP]) {
			if (paddle_right.top < 2.0f){
				paddle_right.top += 0.005f;
				paddle_right.bottom += 0.005f;
				paddle_right_matrix.Translate(0.0f, 0.005, 0.0f);
			}
		}
		if (keys[SDL_SCANCODE_DOWN])
		{
			if (paddle_right.bottom > -2.0f){
				paddle_right.top -= 0.005f;
				paddle_right.bottom -= 0.005f;
				paddle_right_matrix.Translate(0.0f, -0.005f, 0.0f);
			}
		}

		if (keys[SDL_SCANCODE_SPACE])
		{
			if (!game_started)
				game_started = true;
		}

		// left paddle
		program.setModelMatrix(paddle_left_matrix);
		float leftVertices[] = { -3.5f, -0.5f, -3.4f, -0.5f, -3.4f, 0.5f, -3.4f, 0.5f, -3.5f, 0.5f, -3.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, leftVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		// right paddle
		program.setModelMatrix(paddle_right_matrix);
		float rightVertices[] = { 3.4f, -0.5f, 3.5f, -0.5f, 3.5f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rightVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		// ball
		program.setModelMatrix(ball_matrix);
		float ballVertices[] = { -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, -0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		if (game_started)
		{
			// right wins
			if (ball.position_x - 0.1f <= paddle_left.left)
			{
				game_started = false;
				ball_matrix.Translate(-ball.position_x, -ball.position_y, 0.0f);
				ball.position_x = 0.0f;
				ball.position_y = 0.0f;
				ball.direction_x = 0.05f;
				ball.direction_y = 0.01f;
				ball.speed = 0.05f;
				std::cout << "Right player wins!\n";
			}

			// left wins
			else if (ball.position_x + 0.1f >= paddle_right.right)
			{
				game_started = false;
				ball_matrix.Translate(-ball.position_x, -ball.position_y, 0.0f);
				ball.position_x = 0.0f;
				ball.position_y = 0.0f;
				ball.direction_x = 0.05f;
				ball.direction_y = 0.01f;
				ball.speed = 0.05f;
				std::cout << "Left player wins!\n";
			}

			// hits top or bottom of screen
			else if (ball.position_y + 0.1f >= 2.0f || ball.position_y - 0.1f <= -2.0f)
			{
				ball.direction_y *= -1;
				ball.move();
				ball_matrix.Translate(ball.speed * ball.direction_x, ball.speed * ball.direction_y, 0.0f);
			}

			// hits a paddle
			else if ((ball.position_x - 0.1f <= paddle_left.right && ball.position_y - 0.1f <= paddle_left.top && ball.position_y + 0.1f >= paddle_left.bottom) ||
				(ball.position_x + 0.1f >= paddle_right.left && ball.position_y - 0.1f <= paddle_right.top && ball.position_y + 0.1f >= paddle_right.bottom))
			{
				ball.direction_x *= -1;
				ball.move();
				ball_matrix.Translate((ball.speed * ball.direction_x), (ball.speed * ball.direction_y), 0.0f);
			}

			// general movement
			else
			{
				ball.move();
				ball_matrix.Translate((ball.speed * ball.direction_x), (ball.speed * ball.direction_y), 0.0f);
			}
		}
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}