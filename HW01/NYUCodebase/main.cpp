#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
#endif

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

using namespace std;

SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	if (image == NULL) {
		cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(image);
	return retTexture;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	GLuint text1 = LoadTexture(RESOURCE_FOLDER"a.png");
	GLuint text2 = LoadTexture(RESOURCE_FOLDER"b.png");
	GLuint text3 = LoadTexture(RESOURCE_FOLDER"c.png");

	float text1Pos = 0.0f;
	float text2Pos = 0.0f;
	float angle = 0.0f;

	Matrix projectionMatrix;
	//Matrix projectionMatrix1, projectionMatrix2, projectionMatrix3;
	Matrix modelMatrix;
	//Matrix modelMatrix1, modelMatrix2, modelMatrix3;

	Matrix viewMatrix;
	//Matrix viewMatrix1, viewMatrix2, viewMatrix3;

	projectionMatrix.setOrthoProjection(-5.33, 5.33, -3.0f, 3.0f, -1.0f, 1.0f);
	/*projectionMatrix1.setOrthoProjection(-5.33, 5.33, -3.0f, 3.0f, -1.0f, 1.0f);
	projectionMatrix2.setOrthoProjection(-5.33, 5.33, -3.0f, 3.0f, -1.0f, 1.0f);
	projectionMatrix3.setOrthoProjection(-5.33, 5.33, -3.0f, 3.0f, -1.0f, 1.0f);*/

	//glUseProgram(program.programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float lastFrameTicks = 0.0f;

	//modelMatrix.identity();
	//modelMatrix.Translate(1.0, 0.0, 0.0);
	//modelMatrix.Rotate(45.0 * (3.1415926 / 180.0));

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program.programID);

		modelMatrix.identity();
		modelMatrix.Rotate(angle);

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, text1);
		float vertices1[] = { 2.0, 2.0, 3.0, 2.0, 3.0, 3.0, 2.0, 2.0, 3.0, 3.0, 2.0, 3.0 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords1[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, text2);
		float vertices2[] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, text3);
		float vertices3[] = { -3.0, -3.0, -2.0, -3.0, -2.0, -2.0, -3.0, -3.0, -2.0, -2.0, -3.0, -2.0 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		angle += (45.0 * (3.1415926 / 180.0));

		text1Pos += elapsed;
		text2Pos += elapsed;

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}