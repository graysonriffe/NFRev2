#include "pch.h"
#include "nf/Application.h"

#include "version.h"
#include "nf/Utility/Util.h"
#include "Window.h"
#include "Render/Renderer.h"
#include "Input/InputState.h"

namespace nf {
	Application::Application(Config config)
		: m_config(config)
		, m_running(false)
	{}

	void Application::run() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Main Thread");
		std::string consoleTitle = std::format("Nothin' Fancy Debug Console (" NFVERSION ") - {} {}", m_config.appName, m_config.appVersion);
		SetConsoleTitle(util::toWideStr(consoleTitle).c_str());
#endif
		NFLog(std::format("Nothin' Fancy {}", NFVERSION));
		NFLog(std::format("Starting {} {}", m_config.appName, m_config.appVersion));

		m_running = true;

		{
			std::promise<Window&> promiseWindow;
			std::future futureWindow = promiseWindow.get_future();
			std::thread inputThread(&Application::runInputThread, this, std::move(promiseWindow));
			Window& window = futureWindow.get();

			render::Renderer renderer(window, m_config.display);

			while (m_running) {
				renderer.doFrame();
			}

			inputThread.join();
		}

		NFLog("Shutdown");
	}

	void Application::quit() {
		m_running = false;
	}

	using namespace input;

	void Application::runInputThread(std::promise<Window&> promiseWindow) {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Input Thread");
#endif
		Window window(m_config.appName);
		InputState::initialize(&window);
		promiseWindow.set_value(window);

		while (m_running) {
			window.update();
			handleWindowEvents(window);
			InputState::updateMouse();
			NFSleep(10);
		}
	}

	void Application::handleWindowEvents(Window& window) {
		using enum input::Event::Type;

		Window::EventQueue& queue = window.getQueue();

		while (!queue.empty()) {
			input::Event* curr = queue.front();
			switch (curr->getType()) {
				case KeyPress: {
					input::Code key = dynamic_cast<input::KeyEvent*>(curr)->key;
					NFLog(std::format("Event - KeyPress (Code = {})", static_cast<unsigned char>(key)));
					InputState::updateState(key, true);
					break;
				}

				case KeyRelease: {
					input::Code key = dynamic_cast<input::KeyEvent*>(curr)->key;
					NFLog(std::format("Event - KeyRelease (Code = {})", static_cast<unsigned char>(key)));
					InputState::updateState(key, false);
					break;
				}

				case MouseScroll: {
					input::MouseScrollEvent::Direction direction = dynamic_cast<input::MouseScrollEvent*>(curr)->direction;
					NFLog(std::format("Event - MouseScroll (Direction = {})", static_cast<int>(direction)));
					break;
				}

				case WindowClose:
					NFLog("Event - WindowClose");
					quit();
					break;
			}

			queue.pop();
		}

	}

	Application::~Application() {

	}
}