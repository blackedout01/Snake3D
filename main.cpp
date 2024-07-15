/* Author: blackedout01
 *
 * Version History
 * V0.1.2 (2024-07-15) - added macOS support, switched to GLFW 3.4, removed call to glfwGetMouseButton from render thread
 * V0.1.1 (2024-07-13) - replaced my actual name with blackedout01, translated title to english
 * V0.1.0 (2017-11-11) - first version
 */

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

#include <array>
#include <thread>
#include <mutex>
#include <random>
#include <ctime>

template<typename T, size_t C>
class simple_queue
{
public:
	void push_back(const T &t)
	{
		if (s >= C)
			return;

		elements[s++] = t;
	}

	size_t size() const
	{
		return s;
	}

	const T &operator[](size_t i) const
	{
		return elements[i];
	}

	T &operator[](size_t i)
	{
		return elements[i];
	}

	void clear()
	{
		s = 0;
	}

private:
	size_t s = 0;
	std::array<T, C> elements;
};

namespace Randomf
{
	static std::default_random_engine randomGenerator;

	void seed()
	{
		randomGenerator.seed(static_cast<unsigned int>(time(0)));
	}

	int randomInt(int inclStart, int inclEnd)
	{
		std::uniform_int_distribution<int> range(inclStart, inclEnd);
		return range(randomGenerator);
	}
}

namespace ApplicationSettings
{
	// Declare defines
	// Determines whether the console should be hidden.
#ifndef _DEBUG
	#define HIDE_CONSOLE
#endif

#ifdef _DEBUG
	#define DEBUG
#endif

	// Defines the name of the application.
	constexpr const char *NAME_STRING = "Snake3D V0.1.2 by blackedout01 (Controls: W/A/S/D/Space/Shift/Mouse)";

	// Defines the minimum width of the created window.
	constexpr int WINDOW_MIN_WIDTH = 940;

	// Defines the minimum height of the created window.
	constexpr int WINDOW_MIN_HEIGHT = 520;

	// Evaluate defines
#ifdef HIDE_CONSOLE
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif // HIDE_CONSOLE
}

#ifdef DEBUG

#include <iostream>

namespace Debug
{
	template<typename T>
	void clog(T first)
	{
		::std::clog << first;
	}

	template<typename T, typename ...Args>
	void clog(T first, Args ...args)
	{
		::std::clog << first;
		clog(args ...);
	}

	template<typename T>
	void cerr(T first)
	{
		::std::cerr << first;
	}

	template<typename T, typename ...Args>
	void cerr(T first, Args ...args)
	{
		::std::cerr << first;
		cerr(args ...);
	}
}
#else
namespace Debug
{
	template<typename T, typename ...Args>
	void clog(T first, Args ...args) {	}

	template<typename T, typename ...Args>
	void cerr(T first, Args ...args) {	}
}
#endif // DEBUG

#pragma region EVENT_HPP
struct WindowPositionEventArgs
{
	int xpos, ypos;
};

struct WindowSizeEventArgs
{
	int width, height;
};

struct WindowCloseEventArgs
{

};

struct WindowRefreshEventArgs
{

};

struct WindowFocusEventArgs
{
	bool focused;
};

struct WindowIconifyEventArgs
{
	bool iconified;
};

struct FramebufferSizeEventArgs
{
	int width, height;
};

struct MouseButtonEventArgs
{
	int button, action, mods;
};

struct CursorPositionEventArgs
{
	double xpos, ypos;
};

struct CursorEnterEventArgs
{
	bool entered;
};

struct MouseScrollWheelEventArgs
{
	double xoffset;
	double yoffset;
};

struct KeyEventArgs
{
	int key, scancode, action, mods;
};

struct CharEventArgs
{
	unsigned int codepoint;
};

struct CharModsEventArgs
{
	unsigned int codepoint;
	int mods;
};

struct Event
{
	enum Type
	{
		WindowPositionEvent,
		WindowSizeEvent,
		WindowCloseEvent,
		WindowRefreshEvent,
		WindowFocusEvent,
		WindowIconifyEvent,
		FramebufferSizeEvent,
		MouseButtonEvent,
		CursorPositionEvent,
		MouseScrollWheelEvent,
		KeyEvent,
		CharEvent,
		CharModsEvent,

		Count
	};

	union
	{
		WindowPositionEventArgs windowPositionEventArgs;
		WindowSizeEventArgs windowSizeEventArgs;
		WindowCloseEventArgs windowCloseEventArgs;
		WindowRefreshEventArgs windowRefreshEventArgs;
		WindowFocusEventArgs windowFocusEventArgs;
		WindowIconifyEventArgs windowIconifyEventArgs;
		FramebufferSizeEventArgs framebufferSizeEventArgs;
		MouseButtonEventArgs mouseButtonEventArgs;
		CursorPositionEventArgs cursorPositionEventArgs;
		CursorEnterEventArgs cursorEnterEventArgs;
		MouseScrollWheelEventArgs mouseScrollWheelEventArgs;
		KeyEventArgs keyEventArgs;
		CharEventArgs charEventArgs;
		CharModsEventArgs charModsEventArgs;
	};

	Type type;
};
#pragma endregion

struct GLFWwindow;
struct AppData
{
	int width = ApplicationSettings::WINDOW_MIN_WIDTH;
	int height = ApplicationSettings::WINDOW_MIN_HEIGHT;
	GLFWwindow *window = nullptr;
	bool fullscreen = false;
	bool showGameInformation = false;
	simple_queue<Event, 1024> eventQueue;
	std::mutex mutexEventQueue;
	std::mutex initMutex;

	bool initializationDone = false;
	std::mutex initializationMutex;
	std::condition_variable initializationCondition;

	AppData();
	~AppData();
};

// Window events
void windowPositionCallback(GLFWwindow *window, int xpos, int ypos);
void windowSizeCallback(GLFWwindow *window, int width, int height);
void windowCloseCallback(GLFWwindow *window);
void windowRefreshCallback(GLFWwindow *window);
void windowFocusCallback(GLFWwindow *window, int focused);
void windowIconifyCallback(GLFWwindow *window, int minimized);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

// Input Events
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
void cursorEnterCallback(GLFWwindow *window, int entered);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void charCallback(GLFWwindow *window, unsigned int codepoint);
void charModsCallback(GLFWwindow *window, unsigned int codepoint, int mods);

// Own events
void onGlfwErrorEvent(int error, const char *description);

namespace su
{
	// Background
	constexpr float BG_R = 0.05f;
	constexpr float BG_G = 0.10f;
	constexpr float BG_B = 0.15f;

	// Empty tile
	constexpr float ET_R = BG_R * 0.7f;
	constexpr float ET_G = BG_G * 0.7f;
	constexpr float ET_B = BG_B * 0.7f;

	// Food tile
	constexpr float FT_R = 0.8f;
	constexpr float FT_G = 0.1f;
	constexpr float FT_B = 0.1f;

	// Snake tile
	constexpr float ST_R = 0.9f;
	constexpr float ST_G = 1.0f;
	constexpr float ST_B = 0.0f;
}

AppData::AppData()
{
	glfwSetErrorCallback(onGlfwErrorEvent);

	// GLFW
	if (glfwInit() == GLFW_FALSE)
		Debug::cerr("Error while initializing GLFW.\n");
	else
		Debug::clog("Using GLFW ", glfwGetVersionString(), '\n');

	// Window
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(width, height, ApplicationSettings::NAME_STRING, nullptr, nullptr);
	glfwSetWindowSizeLimits(window, ApplicationSettings::WINDOW_MIN_WIDTH, ApplicationSettings::WINDOW_MIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(window, this);

	// Window icon
#ifndef __APPLE__
	{
		constexpr size_t ICON_WIDTH = 16;
		constexpr size_t ICON_HEIGHT = 16;
		constexpr size_t ICON_DEPTH = 4;
		std::array<unsigned char, ICON_WIDTH * ICON_HEIGHT * ICON_DEPTH> pixels;

		constexpr size_t BD = 3;
		constexpr size_t BDM = BD + 1;

		GLFWimage icon{ static_cast<int>(ICON_WIDTH), static_cast<int>(ICON_HEIGHT), pixels.data() };

		for (size_t y = 0; y < ICON_HEIGHT; ++y)
		{
			for (size_t x = 0; x < ICON_WIDTH; ++x)
			{
				unsigned char r, g, b;
				if (x == BD || y == BD || x == ICON_WIDTH - BDM || y == ICON_HEIGHT - BDM)
				{ // background
					r = static_cast<unsigned char>(su::BG_R * 255);
					g = static_cast<unsigned char>(su::BG_G * 255);
					b = static_cast<unsigned char>(su::BG_B * 255);
				}
				else if (x < BD || y < BD || x > ICON_WIDTH - BDM || y > ICON_HEIGHT - BDM)
				{ // empty tile
					if (x < BD && (y > BD && y < ICON_HEIGHT - BDM))
					{ // snake tile
						r = static_cast<unsigned char>(su::ST_R * 255);
						g = static_cast<unsigned char>(su::ST_G * 255);
						b = static_cast<unsigned char>(su::ST_B * 255);
					}
					else if (x > ICON_WIDTH - BDM && (y > BD && y < ICON_HEIGHT - BDM))
					{ // food tile
						r = static_cast<unsigned char>(su::FT_R * 255);
						g = static_cast<unsigned char>(su::FT_G * 255);
						b = static_cast<unsigned char>(su::FT_B * 255);
					}
					else
					{ // empty tile
						r = static_cast<unsigned char>(su::ET_R * 255);
						g = static_cast<unsigned char>(su::ET_G * 255);
						b = static_cast<unsigned char>(su::ET_B * 255);
					}
				}
				else
				{ // snake tile
					r = static_cast<unsigned char>(su::ST_R * 255);
					g = static_cast<unsigned char>(su::ST_G * 255);
					b = static_cast<unsigned char>(su::ST_B * 255);
				}

				size_t i = (y * ICON_WIDTH + x) * ICON_DEPTH;

				pixels[i + 0] = r;
				pixels[i + 1] = g;
				pixels[i + 2] = b;
				pixels[i + 3] = 255;
			}
		}

		glfwSetWindowIcon(window, 1, &icon);
	}
#endif

	// Set window callbacks
	glfwSetWindowPosCallback(window, windowPositionCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetWindowCloseCallback(window, windowCloseCallback);
	glfwSetWindowRefreshCallback(window, windowRefreshCallback);
	glfwSetWindowFocusCallback(window, windowFocusCallback);
	glfwSetWindowIconifyCallback(window, windowIconifyCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Set input callbacks
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetCursorEnterCallback(window, cursorEnterCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);
	glfwSetCharModsCallback(window, charModsCallback);

	glfwMakeContextCurrent(0);
}

AppData::~AppData()
{
	// Window
	glfwDestroyWindow(window);

	// GLFW
	glfwTerminate();
}

// Constants
namespace su
{
	constexpr size_t FIELD_WIDTH = 8;
	constexpr size_t FIELD_HEIGHT = 8;
	constexpr size_t FIELD_DEPTH = 8;
	constexpr size_t FIELD_SIZE = FIELD_WIDTH * FIELD_HEIGHT * FIELD_DEPTH;

	constexpr float FIELD_WIDTH_F = static_cast<float>(FIELD_WIDTH);
	constexpr float FIELD_HEIGHT_F = static_cast<float>(FIELD_HEIGHT);
	constexpr float FIELD_DEPTH_F = static_cast<float>(FIELD_DEPTH);

	constexpr float FIELD_WIDTH_FH = FIELD_WIDTH_F * 0.5f;
	constexpr float FIELD_HEIGHT_FH = FIELD_HEIGHT_F * 0.5f;
	constexpr float FIELD_DEPTH_FH = FIELD_DEPTH_F * 0.5f;

	constexpr static float CUBE_SIZE = 0.8f;
	constexpr static float CUBE_SIZE_H = CUBE_SIZE * 0.5f;
	constexpr static float CUBE_DIST = 1.0f - CUBE_SIZE;
	constexpr static float CUBE_DIST_H = CUBE_DIST * 0.5f;

	glm::mat4 mvp;

	inline glm::vec4 transformPosition4(const glm::vec4 &pos)
	{
		return static_cast<glm::vec4>(mvp * pos);
	}

	void drawCube(const glm::vec3 &p, const glm::vec3 &c)
	{
		glm::vec4 v0t = transformPosition4({ p.x - CUBE_SIZE_H, p.y + CUBE_SIZE_H, p.z - CUBE_SIZE_H, 1.0f });
		glm::vec4 v1t = transformPosition4({ p.x + CUBE_SIZE_H, p.y + CUBE_SIZE_H, p.z - CUBE_SIZE_H, 1.0f });
		glm::vec4 v2t = transformPosition4({ p.x + CUBE_SIZE_H, p.y + CUBE_SIZE_H, p.z + CUBE_SIZE_H, 1.0f });
		glm::vec4 v3t = transformPosition4({ p.x - CUBE_SIZE_H, p.y + CUBE_SIZE_H, p.z + CUBE_SIZE_H, 1.0f });
				
		glm::vec4 v0b = transformPosition4({ p.x - CUBE_SIZE_H, p.y - CUBE_SIZE_H, p.z - CUBE_SIZE_H, 1.0f });
		glm::vec4 v1b = transformPosition4({ p.x + CUBE_SIZE_H, p.y - CUBE_SIZE_H, p.z - CUBE_SIZE_H, 1.0f });
		glm::vec4 v2b = transformPosition4({ p.x + CUBE_SIZE_H, p.y - CUBE_SIZE_H, p.z + CUBE_SIZE_H, 1.0f });
		glm::vec4 v3b = transformPosition4({ p.x - CUBE_SIZE_H, p.y - CUBE_SIZE_H, p.z + CUBE_SIZE_H, 1.0f });

		glm::vec3 cf0 = c * 0.9f;
		glm::vec3 cf1 = c * 0.85f;
		glm::vec3 cf2 = c * 0.8f;
		glm::vec3 cb0 = c * 0.5f;
		glm::vec3 cb1 = c * 0.45f;
		glm::vec3 cb2 = c * 0.4f;

		// Top tile
		glColor3fv(&cf0[0]);
		glVertex4fv(&v0t[0]);
		glVertex4fv(&v1t[0]);
		glVertex4fv(&v2t[0]);

		glVertex4fv(&v2t[0]);
		glVertex4fv(&v3t[0]);
		glVertex4fv(&v0t[0]);

		// Bottom Tile
		glColor3fv(&cb2[0]);
		glVertex4fv(&v2b[0]);
		glVertex4fv(&v1b[0]);
		glVertex4fv(&v0b[0]);

		glVertex4fv(&v0b[0]);
		glVertex4fv(&v3b[0]);
		glVertex4fv(&v2b[0]);

		// Front tile
		glColor3fv(&cf1[0]);
		glVertex4fv(&v3t[0]);
		glVertex4fv(&v2t[0]);
		glVertex4fv(&v2b[0]);

		glVertex4fv(&v2b[0]);
		glVertex4fv(&v3b[0]);
		glVertex4fv(&v3t[0]);

		// Back tile
		glColor3fv(&cb1[0]);
		glVertex4fv(&v1b[0]);
		glVertex4fv(&v1t[0]);
		glVertex4fv(&v0t[0]);

		glVertex4fv(&v0t[0]);
		glVertex4fv(&v0b[0]);
		glVertex4fv(&v1b[0]);

		// Right tile
		glColor3fv(&cf2[0]);
		glVertex4fv(&v2t[0]);
		glVertex4fv(&v1t[0]);
		glVertex4fv(&v1b[0]);

		glVertex4fv(&v1b[0]);
		glVertex4fv(&v2b[0]);
		glVertex4fv(&v2t[0]);

		// Left tile
		glColor3fv(&cb0[0]);
		glVertex4fv(&v0b[0]);
		glVertex4fv(&v0t[0]);
		glVertex4fv(&v3t[0]);
		
		glVertex4fv(&v3t[0]);
		glVertex4fv(&v3b[0]);
		glVertex4fv(&v0b[0]);
	}

	class Field
	{
	public:
		constexpr static size_t MAX_OBSTACLES = FIELD_SIZE + 5;

		Field()
		{
			Randomf::seed();
			newFood();
		}
		
		void draw() const
		{
			drawCube(food + glm::vec3(CUBE_SIZE_H), { su::FT_R, su::FT_G, su::FT_B });
		}

		constexpr size_t getWidth() const
		{
			return FIELD_WIDTH;
		}

		constexpr size_t getHeight() const
		{
			return FIELD_HEIGHT;
		}

		const glm::vec3 &getFood() const
		{
			return food;
		}

		void newFood()
		{
			int x = Randomf::randomInt(0, FIELD_WIDTH - 1);
			int y = Randomf::randomInt(0, FIELD_HEIGHT - 1);
			int z = Randomf::randomInt(0, FIELD_DEPTH - 1);

			food.x = static_cast<float>(x);
			food.y = static_cast<float>(y);
			food.z = static_cast<float>(z);
		}

	private:
		//std::array<glm::vec2, MAX_OBSTACLES> obstacles;
		glm::vec3 food;
	};


	class Snake
	{
	public:
		using pos_t = glm::vec3;
		using part_t = pos_t;

		Snake(Field &field)
			: field(field)
		{

		}

		constexpr static size_t MAX_LENGTH = FIELD_SIZE + 5;

		void setDirection(const pos_t &dir)
		{
			this->rdir = dir;
		}

		void reset(const pos_t &position)
		{
			for (size_t i = 0; i < getLength(); ++i)
			{
				parts[i] = position;
			}
			head = 0;
			tail = 0;
		}

		size_t leftIndex(size_t index)
		{
			if (index == 0)
				return length - 1;
			return index - 1;
		}

		size_t rightIndex(size_t index)
		{
			if (index == length - 1)
				return 0;
			return index + 1;
		}

		void grow()
		{
			//    H 1 T
			// -> H 1 2 T

			//    1 T H
			// -> 1 2 T H

			//    T H 1
			// -> 2 T H 1

			// push tail sequence to the right and insert tail at old tail
			// move head to right except if head is zero, then the head is on the tails left side
			// Implementation: copy objects from left to right starting from the right side moving towards the tail (left) -> tail is dublicated automatically

			size_t index = length;
			while (index != tail)
			{
				parts[index] = parts[index - 1];
				--index;
			}
			++length;
			++tail;
			if(head > 0)
				++head;

			if (length > bestLength)
				bestLength = length;
		}

		void update()
		{
			// Just change if direction is not the opposite
			if (this->cdir + rdir != pos_t())
				this->cdir = rdir;

			// Last part to new first part
			// Implementation: index magic, as this is not a dynamic list

			pos_t newPos = parts[head];
			newPos += cdir;

			if (newPos.x < 0.0f)
				newPos.x = FIELD_WIDTH - 1;
			if (newPos.x > FIELD_WIDTH - 1)
				newPos.x = 0.0f;
			
			if (newPos.y < 0.0f)
				newPos.y = FIELD_HEIGHT - 1;
			if (newPos.y > FIELD_HEIGHT - 1)
				newPos.y = 0.0f;

			if (newPos.z < 0.0f)
				newPos.z = FIELD_DEPTH - 1;
			if (newPos.z > FIELD_DEPTH - 1)
				newPos.z = 0.0f;

			// Check if new position is deadly
			for (size_t i = 0; i < length; ++i)
			{
				if (parts[i] == newPos && i != tail) // tail does not need to be checked as it is removed with this update
				{
					reset({1.0f, 1.0f, 1.0f});
					length = 1;
					return;
				}
			}

			// Check if new position is food
			if (field.getFood() == newPos)
			{
				grow();
				field.newFood();
			}

			head = leftIndex(head);
			tail = leftIndex(head);

			parts[head] = newPos;
		}

		size_t getLength() const
		{
			return this->length;
		}

		size_t getBestLength() const
		{
			return this->bestLength;
		}

		void draw()
		{
			for (size_t i = 0; i < getLength(); ++i)
			{
				su::drawCube(parts[i] + glm::vec3(CUBE_SIZE_H), { su::ST_R, su::ST_G, su::ST_B });
			}
		}

		const pos_t &getHeadPos() const
		{
			return parts[head];
		}

	private:
		Field &field;

		pos_t cdir = pos_t(0.0f, 1.0f, 0.0f); // current direction
		pos_t rdir = pos_t(0.0f, 1.0f, 0.0f); // requested direction
		size_t length = 1;
		size_t head = 0;
		size_t tail = length - 1;
		size_t bestLength = length;

		std::array<part_t, MAX_LENGTH> parts;
	};

	// p, h, r
	glm::vec3 sphericalCoords{ glm::half_pi<float>(), glm::half_pi<float>() * 0.5f, 15.0f };

	// x = p, y = a|h, z = r
	glm::vec3 toCartesianCoords(const glm::vec3 &shericalCoords)
	{
		return {
			shericalCoords.z * glm::sin(shericalCoords.x) * glm::cos(shericalCoords.y),
			shericalCoords.z * glm::cos(shericalCoords.x),
			shericalCoords.z * glm::sin(shericalCoords.x) * glm::sin(shericalCoords.y)
		};
	}

	bool isH1() // bottom
	{
		return sphericalCoords.y > 0.25f * glm::pi<float>() && sphericalCoords.y <= 0.75f * glm::pi<float>();
	}

	bool isH2() // left
	{
		return sphericalCoords.y > 0.75f * glm::pi<float>() && sphericalCoords.y <= 1.25f * glm::pi<float>();
	}

	bool isH3() // top
	{
		return sphericalCoords.y > 1.25f * glm::pi<float>() && sphericalCoords.y <= 1.75f * glm::pi<float>();
	}
	// else right

	void drawNum3D(size_t num, const glm::vec3 &noff, const glm::vec3 &c)
	{
		switch (num)
		{
		case 0:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 1:
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 2:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 3:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 4:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 5:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 6:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 7:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 8:
			su::drawCube({noff.x + 0.0f, noff.y + 4.0f, noff.z}, c);
			su::drawCube({noff.x + 0.0f, noff.y + 3.0f, noff.z}, c);
			su::drawCube({noff.x + 0.0f, noff.y + 2.0f, noff.z}, c);
			su::drawCube({noff.x + 0.0f, noff.y + 1.0f, noff.z}, c);
			su::drawCube({noff.x + 0.0f, noff.y + 0.0f, noff.z}, c);
																												  
			su::drawCube({noff.x + 1.0f, noff.y + 4.0f, noff.z}, c);
			su::drawCube({noff.x + 1.0f, noff.y + 2.0f, noff.z}, c);
			su::drawCube({noff.x + 1.0f, noff.y + 0.0f, noff.z}, c);
																												  
			su::drawCube({noff.x + 2.0f, noff.y + 4.0f, noff.z}, c);
			su::drawCube({noff.x + 2.0f, noff.y + 3.0f, noff.z}, c);
			su::drawCube({noff.x + 2.0f, noff.y + 2.0f, noff.z}, c);
			su::drawCube({noff.x + 2.0f, noff.y + 1.0f, noff.z}, c);
			su::drawCube({noff.x + 2.0f, noff.y + 0.0f, noff.z}, c);
			break;
		case 9:
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		}
	}

	void drawChar3D(char cha, const glm::vec3 &noff, const glm::vec3 &c)
	{
		switch (cha)
		{
		case 'S':
			drawNum3D(5, noff, c);
			break;
		case 'N':
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);

			su::drawCube({ noff.x + 3.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 3.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 3.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 3.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 3.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 'A':
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 'K':
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 'E':
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 0.0f, noff.z }, c);
			break;
		case 'D':
			su::drawCube({ noff.x + 0.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 1.0f, noff.z }, c);
			su::drawCube({ noff.x + 0.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 1.0f, noff.y + 4.0f, noff.z }, c);
			su::drawCube({ noff.x + 1.0f, noff.y + 0.0f, noff.z }, c);

			su::drawCube({ noff.x + 2.0f, noff.y + 3.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 2.0f, noff.z }, c);
			su::drawCube({ noff.x + 2.0f, noff.y + 1.0f, noff.z }, c);
			break;
		}
	}
}

void mainThread(void *data)
{
	AppData &appData = *static_cast<AppData *>(data);

	glfwMakeContextCurrent(appData.window);

	windowSizeCallback(appData.window, appData.width, appData.height);

	{
		std::unique_lock<std::mutex> lk(appData.initializationMutex);
		appData.initializationDone = true;
		appData.initializationCondition.notify_one();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	su::Field field;
	su::Snake snake(field);
	snake.reset({1.0f, 1.0f, 0.0f});

	glClearColor(su::BG_R, su::BG_G, su::BG_B, 1.0f);

	double lmx = 0.0, lmy = 0.0;

	float width = 0.0f, height = 0.0f;
	bool shouldClose = false;
	bool leftMouseButtonDown = false;

	double ticker = 0.0;
	double frameStart = 0.0;
	double deltaTime = 0.0;
	while (!shouldClose)
	{
		{
			std::lock_guard<std::mutex> lock(appData.mutexEventQueue);
			for (size_t i = 0; i < appData.eventQueue.size(); ++i)
			{
				Event &e = appData.eventQueue[i];

				switch (e.type)
				{
				case Event::Type::WindowPositionEvent:
					break;
				case Event::Type::WindowSizeEvent:
					glViewport(0, 0, e.windowSizeEventArgs.width, e.windowSizeEventArgs.height);
					width = static_cast<float>(e.windowSizeEventArgs.width);
					height = static_cast<float>(e.windowSizeEventArgs.height);
					break;
				case Event::Type::WindowCloseEvent:
					shouldClose = true;
					break;
				case Event::Type::WindowRefreshEvent:
					break;
				case Event::Type::WindowFocusEvent:
					break;
				case Event::Type::WindowIconifyEvent:
					break;
				case Event::Type::FramebufferSizeEvent:
					break;
				case Event::Type::MouseButtonEvent:
					if(e.mouseButtonEventArgs.button == GLFW_MOUSE_BUTTON_LEFT) {
						leftMouseButtonDown = e.mouseButtonEventArgs.action != GLFW_RELEASE;
					}
					break;
				case Event::Type::CursorPositionEvent:
					if (leftMouseButtonDown)
					{
						float dx = static_cast<float>(lmx - e.cursorPositionEventArgs.xpos);
						float dy = static_cast<float>(lmy - e.cursorPositionEventArgs.ypos);

						su::sphericalCoords.y = su::sphericalCoords.y - dx * 0.01f;
						if (su::sphericalCoords.y >= glm::two_pi<float>())
							su::sphericalCoords.y -= glm::two_pi<float>();
						if(su::sphericalCoords.y < 0.0f)
							su::sphericalCoords.y += glm::two_pi<float>();

						su::sphericalCoords.x = glm::clamp(su::sphericalCoords.x + dy * 0.01f, 0.01f, glm::pi<float>() - 0.01f);
					}
					lmx = e.cursorPositionEventArgs.xpos;
					lmy = e.cursorPositionEventArgs.ypos;
					break;
				case Event::Type::MouseScrollWheelEvent:
					su::sphericalCoords.z -= su::sphericalCoords.z * e.mouseScrollWheelEventArgs.yoffset * 0.1f;
					if (su::sphericalCoords.z > 50.0f)
						su::sphericalCoords.z = 50.0f;
					if (su::sphericalCoords.z < 0.1f)
						su::sphericalCoords.z = 0.1f;
					break;
				case Event::Type::KeyEvent:
					if (e.keyEventArgs.action == GLFW_PRESS)
					{
						switch (e.keyEventArgs.key)
						{
						case GLFW_KEY_W:
						case GLFW_KEY_UP:
							// opposite dir
							if(su::isH1()) // from +z
								snake.setDirection({ +0.0f, +0.0f, -1.0f });
							else if(su::isH2()) // from -x
								snake.setDirection({ +1.0f, +0.0f, +0.0f });
							else if(su::isH3()) // from -z
								snake.setDirection({ +0.0f, +0.0f, +1.0f });
							else // from +x
								snake.setDirection({ -1.0f, +0.0f, +0.0f });
							break;
						case GLFW_KEY_S:
						case GLFW_KEY_DOWN:
							// same dir
							if (su::isH1()) // from +z
								snake.setDirection({ +0.0f, +0.0f, +1.0f });
							else if (su::isH2()) // from -x
								snake.setDirection({ -1.0f, +0.0f, +0.0f });
							else if (su::isH3()) // from -z
								snake.setDirection({ +0.0f, +0.0f, -1.0f });
							else // from +x
								snake.setDirection({ +1.0f, +0.0f, +0.0f });
							break;
						case GLFW_KEY_A:
						case GLFW_KEY_LEFT:
							// left dir
							if (su::isH1()) // from +z
								snake.setDirection({ -1.0f, +0.0f, +0.0f });
							else if (su::isH2()) // from -x
								snake.setDirection({ +0.0f, +0.0f, -1.0f });
							else if (su::isH3()) // from -z
								snake.setDirection({ +1.0f, +0.0f, +0.0f });
							else // from +x
								snake.setDirection({ +0.0f, +0.0f, +1.0f });
							break;
						case GLFW_KEY_D:
						case GLFW_KEY_RIGHT:
							// right dir
							if (su::isH1()) // from +z
								snake.setDirection({ +1.0f, +0.0f, +0.0f });
							else if (su::isH2()) // from -x
								snake.setDirection({ +0.0f, +0.0f, +1.0f });
							else if(su::isH3()) // from -z
								snake.setDirection({ -1.0f, +0.0f, +0.0f });
							else // from +x
								snake.setDirection({ +0.0f, +0.0f, -1.0f });
							break;
						case GLFW_KEY_SPACE:
							snake.setDirection({ +0.0f, +1.0f, +0.0f });
							break;
						case GLFW_KEY_LEFT_SHIFT:
							snake.setDirection({ +0.0f, -1.0f, +0.0f });
							break;
						case GLFW_KEY_F3:
							appData.showGameInformation = !appData.showGameInformation;
						case GLFW_KEY_F11:
							glfwSwapInterval(1);
							break;
						}
					}
					break;
				case Event::Type::CharEvent:
					break;
				case Event::Type::CharModsEvent:
					break;
				default:
					break;
				}
			}
			appData.eventQueue.clear();
		}
		
#ifdef __APPLE__
		// NOTE(blackedout): Fix for https://github.com/glfw/glfw/issues/1997
		CGLContextObj cglContext = CGLGetCurrentContext();
   		CGLLockContext(cglContext);
#endif
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 pMatGame = glm::perspective(glm::half_pi<float>() * 0.5f, width / height, 0.1f, 100.0f);
		glm::mat4 vMatGame = glm::lookAt(su::toCartesianCoords(su::sphericalCoords), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mMatGame = glm::translate(glm::mat4(), glm::vec3{ su::FIELD_WIDTH * -0.5f, su::FIELD_HEIGHT * -0.5f, su::FIELD_DEPTH * -0.5f });

		glm::mat4 vMatText = glm::lookAt(glm::vec3(0.0f, -10.0f, 40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 vpGame = pMatGame * vMatGame;
		glm::mat4 vpText = pMatGame * vMatText;

		// Render game scene
		glBegin(GL_TRIANGLES);

		su::mvp = vpGame * mMatGame;

		field.draw();
		snake.draw();

		glEnd();

		// Render game scene lines
		glBegin(GL_LINES);

		// Draw borders
		glColor4f(0.8f, 0.2f, 0.2f, 0.4f);
		{
			glm::vec4 v0b = su::transformPosition4({ 0.0f, 0.0f, 0.0f, 1.0f });
			glm::vec4 v1b = su::transformPosition4({ +su::FIELD_WIDTH_F, 0.0f, 0.0f, 1.0f });
			glm::vec4 v2b = su::transformPosition4({ +su::FIELD_WIDTH_F, 0.0f, +su::FIELD_DEPTH_F, 1.0f });
			glm::vec4 v3b = su::transformPosition4({ 0.0f, 0.0f, +su::FIELD_DEPTH_F, 1.0f });

			glm::vec4 v0t = su::transformPosition4({ 0.0f, +su::FIELD_HEIGHT_F, 0.0f, 1.0f });
			glm::vec4 v1t = su::transformPosition4({ +su::FIELD_WIDTH_F, +su::FIELD_HEIGHT_F, 0.0f, 1.0f });
			glm::vec4 v2t = su::transformPosition4({ +su::FIELD_WIDTH_F, +su::FIELD_HEIGHT_F, +su::FIELD_DEPTH_F, 1.0f });
			glm::vec4 v3t = su::transformPosition4({ 0.0f, +su::FIELD_HEIGHT_F, +su::FIELD_DEPTH_F, 1.0f });

			glVertex4fv(&v0b[0]);
			glVertex4fv(&v1b[0]);

			glVertex4fv(&v1b[0]);
			glVertex4fv(&v2b[0]);

			glVertex4fv(&v2b[0]);
			glVertex4fv(&v3b[0]);

			glVertex4fv(&v3b[0]);
			glVertex4fv(&v0b[0]);

			glVertex4fv(&v0t[0]);
			glVertex4fv(&v1t[0]);

			glVertex4fv(&v1t[0]);
			glVertex4fv(&v2t[0]);

			glVertex4fv(&v2t[0]);
			glVertex4fv(&v3t[0]);

			glVertex4fv(&v3t[0]);
			glVertex4fv(&v0t[0]);
		}

		// Draw snake direction borders
		glColor4f(su::ST_R, su::ST_G, su::ST_B, 0.1f);
		{
			auto hp = snake.getHeadPos() + glm::vec3(su::CUBE_SIZE_H);

			// x dir
			{
				glm::vec4 v0b = su::transformPosition4({ 0.0f, hp.y - su::CUBE_SIZE_H, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v1b = su::transformPosition4({ +su::FIELD_WIDTH_F, hp.y - su::CUBE_SIZE_H, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v2b = su::transformPosition4({ +su::FIELD_WIDTH_F, hp.y - su::CUBE_SIZE_H, hp.z + su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v3b = su::transformPosition4({ 0.0f, hp.y - su::CUBE_SIZE_H, hp.z + su::CUBE_SIZE_H, 1.0f });

				glm::vec4 v0t = su::transformPosition4({ 0.0f, hp.y + su::CUBE_SIZE_H, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v1t = su::transformPosition4({ +su::FIELD_WIDTH_F, hp.y + su::CUBE_SIZE_H, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v2t = su::transformPosition4({ +su::FIELD_WIDTH_F, hp.y + su::CUBE_SIZE_H, hp.z + su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v3t = su::transformPosition4({ 0.0f, hp.y + su::CUBE_SIZE_H, hp.z + su::CUBE_SIZE_H, 1.0f });

				glVertex4fv(&v0b[0]);
				glVertex4fv(&v1b[0]);

				glVertex4fv(&v2b[0]);
				glVertex4fv(&v3b[0]);

				glVertex4fv(&v0t[0]);
				glVertex4fv(&v1t[0]);

				glVertex4fv(&v2t[0]);
				glVertex4fv(&v3t[0]);
			}

			// y dir
			{
				glm::vec4 v0b = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, 0.0f, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v1b = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, 0.0f, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v2b = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, 0.0f, hp.z + su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v3b = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, 0.0f, hp.z + su::CUBE_SIZE_H, 1.0f });

				glm::vec4 v0t = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, +su::FIELD_HEIGHT_F, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v1t = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, +su::FIELD_HEIGHT_F, hp.z - su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v2t = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, +su::FIELD_HEIGHT_F, hp.z + su::CUBE_SIZE_H, 1.0f });
				glm::vec4 v3t = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, +su::FIELD_HEIGHT_F, hp.z + su::CUBE_SIZE_H, 1.0f });

				glVertex4fv(&v0b[0]);
				glVertex4fv(&v0t[0]);

				glVertex4fv(&v1b[0]);
				glVertex4fv(&v1t[0]);

				glVertex4fv(&v2b[0]);
				glVertex4fv(&v2t[0]);

				glVertex4fv(&v3b[0]);
				glVertex4fv(&v3t[0]);
			}

			// z dir
			{
				glm::vec4 v0b = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, hp.y - su::CUBE_SIZE_H, 0.0f, 1.0f });
				glm::vec4 v1b = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, hp.y - su::CUBE_SIZE_H, 0.0f, 1.0f });
				glm::vec4 v2b = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, hp.y - su::CUBE_SIZE_H, +su::FIELD_DEPTH_F, 1.0f });
				glm::vec4 v3b = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, hp.y - su::CUBE_SIZE_H, +su::FIELD_DEPTH_F, 1.0f });

				glm::vec4 v0t = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, hp.y + su::CUBE_SIZE_H, 0.0f, 1.0f });
				glm::vec4 v1t = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, hp.y + su::CUBE_SIZE_H, 0.0f, 1.0f });
				glm::vec4 v2t = su::transformPosition4({ hp.x + su::CUBE_SIZE_H, hp.y + su::CUBE_SIZE_H, +su::FIELD_DEPTH_F, 1.0f });
				glm::vec4 v3t = su::transformPosition4({ hp.x - su::CUBE_SIZE_H, hp.y + su::CUBE_SIZE_H, +su::FIELD_DEPTH_F, 1.0f });

				glVertex4fv(&v3b[0]);
				glVertex4fv(&v0b[0]);

				glVertex4fv(&v2b[0]);
				glVertex4fv(&v1b[0]);

				glVertex4fv(&v3t[0]);
				glVertex4fv(&v0t[0]);

				glVertex4fv(&v2t[0]);
				glVertex4fv(&v1t[0]);
			}
		}

		// Draw axis
		if (appData.showGameInformation) // only modified in this thread
		{
			su::mvp = vpGame;
			glm::vec4 axis_o = su::transformPosition4({ 0.0f, 0.0f, 0.0f, 1.0f });
			glm::vec4 axis_x = su::transformPosition4({ 1.0f, 0.0f, 0.0f, 1.0f });
			glm::vec4 axis_y = su::transformPosition4({ 0.0f, 1.0f, 0.0f, 1.0f });
			glm::vec4 axis_z = su::transformPosition4({ 0.0f, 0.0f, 1.0f, 1.0f });

			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex4fv(&axis_o[0]);
			glVertex4fv(&axis_x[0]);

			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex4fv(&axis_o[0]);
			glVertex4fv(&axis_y[0]);

			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex4fv(&axis_o[0]);
			glVertex4fv(&axis_z[0]);
		}

		glEnd();

		glClear(GL_DEPTH_BUFFER_BIT);

		// Render ui
		glBegin(GL_TRIANGLES);

		// Render title text
		su::mvp = glm::translate(glm::mat4(), glm::vec3{ 0.0f, 0.8f, 0.0f }) * vpText;

		glm::vec3 titleColor{ su::ST_R, su::ST_G, su::ST_B };
		su::drawChar3D('S', { 0.0f - 12.5f, -2.0f, 0.0f }, titleColor);
		su::drawChar3D('N', { 4.0f - 12.5f, -2.0f, 0.0f }, titleColor);
		su::drawChar3D('A', { 9.0f - 12.5f, -2.0f, 0.0f }, titleColor);
		su::drawChar3D('K', { 13.0f - 12.5f, -2.0f, 0.0f }, titleColor);
		su::drawChar3D('E', { 17.0f - 12.5f, -2.0f, 0.0f }, titleColor);
		su::drawNum3D(3, { 21.0f - 12.5f, -2.0f, 0.0f }, titleColor);
		su::drawChar3D('D', { 25.0f - 12.5f, -2.0f, 0.0f }, titleColor);

		// Render score num
		su::mvp = glm::translate(glm::mat4(), glm::vec3{ -0.9f, -0.9f, 0.0f }) * vpText;

		glm::vec3 scoreColor{ su::ST_R, su::ST_G, su::ST_B };
		// Get length digit count
		{
			size_t digitNum = 0;
			size_t dnum = snake.getLength();
			while (dnum != 0)
			{
				dnum /= 10;
				++digitNum;
			}

			// Draw score
			size_t i = digitNum - 1;
			dnum = snake.getLength();
			while (dnum != 0)
			{
				size_t digit = dnum % 10;
				dnum /= 10;
				su::drawNum3D(digit, { i * 4, 0.0f, 0.0f }, scoreColor);
				--i;
			}
		}

		// Render highscore num
		su::mvp = glm::translate(glm::mat4(), glm::vec3{ +0.8f, -0.9f, 0.0f }) * vpText;

		// Get best length digit count
		{
			size_t bdigitNum = 0;
			size_t bdnum = snake.getBestLength();
			while (bdnum != 0)
			{
				bdnum /= 10;
				++bdigitNum;
			}

			// Draw best score
			size_t i = bdigitNum - 1;
			bdnum = snake.getBestLength();
			while (bdnum != 0)
			{
				size_t digit = bdnum % 10;
				bdnum /= 10;
				su::drawNum3D(digit, { i * 4.0f - bdigitNum * 4.0f + 4.0f, 0.0f, 0.0f }, scoreColor);
				--i;
			}
		}

		glEnd();
		
		glfwSwapBuffers(appData.window);

#ifdef __APPLE__
		CGLUnlockContext(cglContext);
#endif

		double currTime = glfwGetTime();
		deltaTime = currTime - frameStart;
		frameStart = currTime;
		//Debug::clog(1.0 / deltaTime, '\n');

		ticker += deltaTime;
		while(ticker >= 0.2)
		{
			ticker -= 0.2;
			snake.update();
		}
	}
}

int main()
{
	AppData appData;

	std::thread thread(&mainThread, &appData);
	{
		std::unique_lock<std::mutex> lk(appData.initializationMutex);
		while (!appData.initializationDone)
		{
			Debug::clog("Waiting for initialization...\n");
			appData.initializationCondition.wait(lk);

			if (!appData.initializationDone)
				Debug::clog("Spurious wakeup while initializing.\n");
		}
		Debug::clog("Initialization done!\n");
	}

	while (!glfwWindowShouldClose(appData.window))
		glfwWaitEvents();

	thread.join();

	return 0;
}

void onGlfwErrorEvent(int error, const char *description)
{
	Debug::cerr("Error ", error, ": ", description, '\n');
}

// Window events
void windowPositionCallback(GLFWwindow *window, int xpos, int ypos)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));
	Event e;
	e.type = Event::Type::WindowPositionEvent;
	e.windowPositionEventArgs.xpos = xpos;
	e.windowPositionEventArgs.ypos = ypos;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);

	for (size_t i = 0; i < appData->eventQueue.size(); ++i)
	{
		if (appData->eventQueue[i].type == Event::Type::WindowPositionEvent)
		{
			appData->eventQueue[i] = e;
			return;
		}
	}

	appData->eventQueue.push_back(e);
}

void windowSizeCallback(GLFWwindow *window, int width, int height)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::WindowSizeEvent;
	e.windowSizeEventArgs.width = width;
	e.windowSizeEventArgs.height = height;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);

	appData->width = width;
	appData->height = height;

	for (size_t i = 0; i < appData->eventQueue.size(); ++i)
	{
		if (appData->eventQueue[i].type == Event::Type::WindowSizeEvent)
		{
			appData->eventQueue[i] = e;
			return;
		}
	}

	appData->eventQueue.push_back(e);
}

void windowCloseCallback(GLFWwindow *window)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::WindowCloseEvent;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void windowRefreshCallback(GLFWwindow *window)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::WindowRefreshEvent;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void windowFocusCallback(GLFWwindow *window, int focused)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));
	Event e;
	e.type = Event::Type::WindowFocusEvent;
	e.windowFocusEventArgs.focused = focused == GLFW_TRUE;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void windowIconifyCallback(GLFWwindow *window, int minimized)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));
	Event e;
	e.type = Event::Type::WindowIconifyEvent;
	e.windowIconifyEventArgs.iconified = minimized == GLFW_TRUE;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));
	Event e;
	e.type = Event::Type::FramebufferSizeEvent;
	e.framebufferSizeEventArgs.width = width;
	e.framebufferSizeEventArgs.height = height;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

// Input Events
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));
	Event e;
	e.type = Event::Type::MouseButtonEvent;
	e.mouseButtonEventArgs.button = button;
	e.mouseButtonEventArgs.action = action;
	e.mouseButtonEventArgs.mods = mods;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::CursorPositionEvent;
	e.cursorPositionEventArgs.xpos = xpos;
	e.cursorPositionEventArgs.ypos = ypos;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void cursorEnterCallback(GLFWwindow *window, int entered)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::CursorPositionEvent;
	e.cursorEnterEventArgs.entered = entered == GLFW_TRUE;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));
	Event e;
	e.type = Event::Type::MouseScrollWheelEvent;
	e.mouseScrollWheelEventArgs.xoffset = xoffset;
	e.mouseScrollWheelEventArgs.yoffset = yoffset;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	{
		Event e;
		e.type = Event::Type::KeyEvent;
		e.keyEventArgs.key = key;
		e.keyEventArgs.scancode = scancode;
		e.keyEventArgs.action = action;
		e.keyEventArgs.mods = mods;
		std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
		appData->eventQueue.push_back(e);
	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F11:
		{
			GLFWmonitor *pmon = glfwGetPrimaryMonitor();
			const GLFWvidmode *vmod = glfwGetVideoMode(pmon);

			if (appData->fullscreen)
			{
				appData->fullscreen = false;
				glfwSetWindowMonitor(appData->window, nullptr, vmod->width / 2 - ApplicationSettings::WINDOW_MIN_WIDTH, vmod->height / 2 - ApplicationSettings::WINDOW_MIN_HEIGHT, ApplicationSettings::WINDOW_MIN_WIDTH, ApplicationSettings::WINDOW_MIN_HEIGHT, GLFW_DONT_CARE);
			}
			else
			{
				appData->fullscreen = true;
				glfwSetWindowMonitor(appData->window, pmon, 0, 0, vmod->width, vmod->height, vmod->refreshRate);
			}
		} break;
		}
	}
}

void charCallback(GLFWwindow *window, unsigned int codepoint)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::CharEvent;
	e.charEventArgs.codepoint = codepoint;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}

void charModsCallback(GLFWwindow *window, unsigned int codepoint, int mods)
{
	AppData *appData = static_cast<AppData *>(glfwGetWindowUserPointer(window));

	Event e;
	e.type = Event::Type::CharModsEvent;
	e.charModsEventArgs.codepoint = codepoint;
	e.charModsEventArgs.mods = mods;
	std::lock_guard<std::mutex> lock(appData->mutexEventQueue);
	appData->eventQueue.push_back(e);
}