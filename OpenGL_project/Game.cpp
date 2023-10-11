#include "Game.h"
#include "CollisionSystem.h"
//#include "PhysicalObject.h"

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);


void Game::initGLFW()
{
	//INIT GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

//Private functions
void Game::initWindow(
	const char* title,
	bool resizable)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, title, NULL, NULL);
	
	if (this->window == nullptr)
	{
		std::cout << "ERROR::GLFW_WINDOW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	glfwSetFramebufferSizeCallback(this->window, Game::framebuffer_resize_callback); // RESIZABLE framebuffer

	//glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight); // NOT RESIZABLE framebuffer
	//glViewport(0, 0, frameBufferWidth, frameBufferHeight);

	glfwMakeContextCurrent(this->window); //IMPORTANT!!
}

void Game::initGLEW()
{
	//INIT GLEW (NEEDS WINDOW AND OPENGL CONTEXT)
	glewExperimental = GL_TRUE;

	//Error
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initOpenGLOptions()
{
	//OPENGL OPTIONS
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::initMatrices()
{
	this->ViewMatrix = glm::mat4(1.f);
	this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);

	//Projection matrix
	this->ProjectionMatrix = glm::mat4(1.f);

	ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane
	);
}

void Game::initShaders()
{
	this->shaders.push_back(new Shader(this->GL_VERSION_MAJOR, this->GL_VERSION_MINOR,
		"vertex_core.glsl", "fragment_core.glsl"));
}

void Game::initTextures()
{
	TextureParams texParams;

	this->textures.push_back(new Texture("Images/anime_girl.png", GL_TEXTURE_2D, texParams));
	this->textures.push_back(new Texture("Images/anime_girl_specular.png", GL_TEXTURE_2D, texParams));

	this->textures.push_back(new Texture("Images/cage.png", GL_TEXTURE_2D, texParams));
	this->textures.push_back(new Texture("Images/cage_specular.png", GL_TEXTURE_2D, texParams));

	this->textures.push_back(new Texture("Images/wood.png", GL_TEXTURE_2D, texParams));
	this->textures.push_back(new Texture("Images/wood_specular.png", GL_TEXTURE_2D, texParams));
}

void Game::initMaterials()
{
	this->materials.push_back(new Material(glm::vec3(0.1f), glm::vec3(1.f), glm::vec3(3.f), 35.f, 0, 1));
	this->materials.push_back(new Material(glm::vec3(0.8f), glm::vec3(1.f), glm::vec3(0.f), 1.f, 0, 1));
}

void Game::initModels()
{
	std::vector<Mesh*> meshes;
	std::vector<Mesh*> meshes2;
	

	meshes.push_back(
		new Mesh(
			new Pyramid(),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(1.f)
		)
	);

	meshes2.push_back(
		new Mesh(
			new Quad(),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(-90.f, 0.f, 0.f),
			glm::vec3(1.f)
		)
	);

	this->sceneObjs.push_back(new Model(
		glm::vec3(0.f, 0.5f, 0.f),
		this->materials[0],
		this->textures[4],
		this->textures[5],
		meshes
	)
	);

	this->sceneObjs.push_back(new Model(
		glm::vec3(-4.f, 0.f, 4.f),
		this->materials[0],
		this->textures[4],
		this->textures[5],
		meshes
	)
	);

	this->sceneObjs.push_back(new Model(
		glm::vec3(5.f, 0.f, 4.f),
		this->materials[1],
		this->textures[0],
		this->textures[1],
		"MeshesOBJ/model.obj"
	)
	);

	/*this->sceneObjs.push_back(new Model(
		glm::vec3(0.f, 0.f, 4.f),
		this->materials[0],
		this->textures[4],
		this->textures[5],
		meshes2
	)
	);*/

	std::cout << "1: " << this->sceneObjs[0] << "\n";
	std::cout << "2: " << this->sceneObjs[1] << "\n";
	std::cout << "3: " << this->sceneObjs[2] << "\n";


	for (auto*& i : meshes)
		delete i;
	for (auto*& i : meshes2)
		delete i;

	static_cast<PhysicalObject*>(this->sceneObjs[2])->setPhysicalStatus(true);
}

void Game::initPointLights()
{
	this->lights.push_back(
		new PointLight(
			glm::vec3(5.f, 6.f, -10.f),
			1.f,
			glm::vec3(1.f, 0.f, 0.f)
		)
	);

	this->lights.push_back(
		new PointLight(
			glm::vec3(0.f, 4.f, 0.f),
			1.f,
			glm::vec3(1.f, 0.5f, 0.31f)
		)
	);

	this->lights.push_back(
		new PointLight(
			glm::vec3(0.f, 6.f, 10.f),
			1.f,
			glm::vec3(1.f, 0.5f, 0.31f)
		)
	);

	/*this->lights.push_back(
		new DirectionalLight(
			glm::vec3(-0.2f, -1.f, -0.3f),
			1.f
		)
	);*/

	this->lights.push_back(
		new SpotLight(
			glm::vec3(0.f, 6.f, 0.f),
			glm::vec3(1.f, -1.f, 0.f),
			10.f, 40.f
		)
	);
}

void Game::initLights()
{
	this->initPointLights();
}

void Game::initUniforms()
{
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");

	//init lights on GPU
	int pL_Iter = 0;
	int sL_Iter = 0;
	for (Light* pl : this->lights)
	{
		switch (pl->getType())
		{
		case 0:
			pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM], 0);
			break;
		case 1:
			pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM], pL_Iter);
			pL_Iter++;
			break;
		case 2:
			pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM], sL_Iter);
			sL_Iter++;
			break;
		}
	}
}

void Game::updateUniforms()
{
	//Update view matrix
	this->ViewMatrix = this->character->getViewMatrix();

	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(this->character->getCamPosition(), "camPosition");

	//update lights
	int pL_Iter = 0;
	int sL_Iter = 0;
	for (Light* pl : this->lights)
	{
		switch (pl->getType())
		{
		case 0:
			pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM], 0);
			break;
		case 1:
			pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM], pL_Iter);
			pL_Iter++;
			break;
		case 2:
			pl->sendToShader(*this->shaders[SHADER_CORE_PROGRAM], sL_Iter);
			sL_Iter++;
			break;
		}
	}

	//Update framebuffer size and projection matrix
	glfwGetFramebufferSize(window, &this->framebufferWidth, &this->framebufferHeight);

	this->ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane
	);
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
}


//Constructors / Destructor
Game::Game(
	const char* title,
	const int width, const int height,
	const int GLVerMajor, const int GLVerMinor,
	bool resizable
)
	: 
	WINDOW_WIDTH(width), 
	WINDOW_HEIGHT(height),
	GL_VERSION_MAJOR(GLVerMajor), 
	GL_VERSION_MINOR(GLVerMinor)
{
	//INIT VARIABLES
	//Window
	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->framebufferHeight;

	//Camera matrix
	this->camPosition = glm::vec3(0.f, 0.f, 1.f);
	this->worldUp = glm::vec3(0.f, 1.f, 0.f);
	this->camFront = glm::vec3(0.f, 0.f, -1.f);

	//Projection matrix
	this->fov = 90.f;
	this->nearPlane = 0.1f;
	this->farPlane = 1000.f;

	//Delta time
	this->dt = 0.f;
	this->curTime = 0.f;
	this->lastTime = 0.f;

	//INITIALIZATION
	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLEW();
	this->initOpenGLOptions();

	this->initMatrices();
	this->initShaders();
	this->initTextures();
	this->initMaterials();
	this->initModels();
	this->initLights();

	this->initUniforms();

	this->character = new CharacterEntity(
		this->window,
		//this->models[1],
		nullptr,
		glm::vec3(3.f, 0.f, 0.f),
		glm::vec3(0.f),
		glm::vec3(1.f)
		);
	
	this->sceneObjs.push_back(this->character);

	CollisionSystem::getInstance()->setObjs(&this->sceneObjs);

	EventSystem::Instance()->SendEvent("BEGINPLAY", 0);
}

Game::~Game()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	for (size_t i = 0; i < this->shaders.size(); i++)
		delete this->shaders[i];

	for (size_t i = 0; i < this->textures.size(); i++)
		delete this->textures[i];

	for (size_t i = 0; i < this->materials.size(); i++)
		delete this->materials[i];

	for (auto*& i : this->sceneObjs)
		delete i;

	for (size_t i = 0; i < this->lights.size(); i++)
		delete this->lights[i];
}

//Accessors
int Game::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}


//Modifiers
void Game::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

//Functions
void Game::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

void Game::updateInput()
{
	glfwPollEvents();

	//Program
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		this->setWindowShouldClose();
	}

	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		//this->pointLights[0]->setPosition(this->character->getPosition());

		//static_cast<PointLight*>(this->lights[0])->setPosition(this->character->getPosition());
		static_cast<Model*>(this->sceneObjs[0])->applyForce(glm::vec3(-100.f, 0.f, 0.f));
		
	}

	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		static_cast<Model*>(this->sceneObjs[0])->applyForce(glm::vec3(100.f, 0.f, 0.f));
		
		//std::cout << this->sceneObjs[0] << "\n";
	}
}

void Game::update()
{
	this->updateDt();
	this->updateInput();

	//this->character->update();

	for (size_t i = 0; i < this->sceneObjs.size(); i++)
		this->sceneObjs.at(i)->update();

	void* voidPtr = &dt;
	EventSystem::Instance()->SendEvent("TICK", voidPtr);
	EventSystem::Instance()->ProcessEvents();
}

void Game::render()
{
	//DRAW ---
	//clear
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Update uniforms
	this->updateUniforms();

	//Render Scene objects
	for (auto& i : this->sceneObjs)
		i->render(this->shaders[SHADER_CORE_PROGRAM]);

	//this->character->render(this->shaders[SHADER_CORE_PROGRAM]);

	//End draw
	glfwSwapBuffers(this->window);
	glFlush();
	
	//char* path = (char*)"D:/testImg.bmp";


	/*unsigned char* pixels = new unsigned char[3 * this->WINDOW_WIDTH * this->WINDOW_HEIGHT];
	glReadPixels(0, 0, this->WINDOW_WIDTH, this->WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	generateBitmapImage(pixels, this->WINDOW_HEIGHT, this->WINDOW_WIDTH, path);*/

	//Reset
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Static functions
void Game::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
}


// THEN DELETE NEXT:
const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName)
{
	int widthInBytes = width * BYTES_PER_PIXEL;

	unsigned char padding[3] = { 0, 0, 0 };
	int paddingSize = (4 - (widthInBytes) % 4) % 4;

	int stride = (widthInBytes)+paddingSize;

	FILE* imageFile = fopen(imageFileName, "wb");

	unsigned char* fileHeader = createBitmapFileHeader(height, stride);
	fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

	unsigned char* infoHeader = createBitmapInfoHeader(height, width);
	fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

	int i;
	for (i = 0; i < height; i++) {
		fwrite(image + (i * widthInBytes), BYTES_PER_PIXEL, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}

	fclose(imageFile);
}

unsigned char* createBitmapFileHeader(int height, int stride)
{
	int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

	static unsigned char fileHeader[] = {
		0,0,     /// signature
		0,0,0,0, /// image file size in bytes
		0,0,0,0, /// reserved
		0,0,0,0, /// start of pixel array
	};

	fileHeader[0] = (unsigned char)('B');
	fileHeader[1] = (unsigned char)('M');
	fileHeader[2] = (unsigned char)(fileSize);
	fileHeader[3] = (unsigned char)(fileSize >> 8);
	fileHeader[4] = (unsigned char)(fileSize >> 16);
	fileHeader[5] = (unsigned char)(fileSize >> 24);
	fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

	return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width)
{
	static unsigned char infoHeader[] = {
		0,0,0,0, /// header size
		0,0,0,0, /// image width
		0,0,0,0, /// image height
		0,0,     /// number of color planes
		0,0,     /// bits per pixel
		0,0,0,0, /// compression
		0,0,0,0, /// image size
		0,0,0,0, /// horizontal resolution
		0,0,0,0, /// vertical resolution
		0,0,0,0, /// colors in color table
		0,0,0,0, /// important color count
	};

	infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
	infoHeader[4] = (unsigned char)(width);
	infoHeader[5] = (unsigned char)(width >> 8);
	infoHeader[6] = (unsigned char)(width >> 16);
	infoHeader[7] = (unsigned char)(width >> 24);
	infoHeader[8] = (unsigned char)(height);
	infoHeader[9] = (unsigned char)(height >> 8);
	infoHeader[10] = (unsigned char)(height >> 16);
	infoHeader[11] = (unsigned char)(height >> 24);
	infoHeader[12] = (unsigned char)(1);
	infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

	return infoHeader;
}