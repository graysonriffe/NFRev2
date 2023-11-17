#include "pch.h"
#include "nf/Application.h"

#include "version.h"
#include "nf/Utility/Util.h"
#include "Window.h"
#include "Render/Renderer.h"

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
				//NFSleep(500);
			}

			inputThread.join();
		}

		NFLog("Shutdown");
	}

	void Application::quit() {
		m_running = false;
	}

	void Application::runInputThread(std::promise<Window&> promiseWindow) {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Input Thread");
#endif

		Window window(m_config.appName);
		promiseWindow.set_value(window);

		while (m_running) {
			window.update();
			handleWindowEvents(window);
			NFSleep(10);
		}
	}

	void Application::handleWindowEvents(Window& window) {
		Window::EventQueue& queue = window.getQueue();

		while (!queue.empty()) {
			Event* curr = queue.front();
			switch (curr->getType()) {
				case Event::Type::KeyPress: {
					input::Key key = dynamic_cast<KeyEvent*>(curr)->key;
					NFLog(std::format("Event - KeyPress (Code = {})", static_cast<unsigned char>(key)));
					break;
				}

				case Event::Type::KeyRelease: {
					input::Key key = dynamic_cast<KeyEvent*>(curr)->key;
					NFLog(std::format("Event - KeyRelease (Code = {})", static_cast<unsigned char>(key)));
					break;
				}

				case Event::Type::MousePress: {
					input::Mouse button = dynamic_cast<MouseEvent*>(curr)->button;
					NFLog(std::format("Event - MousePress (Button = {})", static_cast<unsigned char>(button)));
					break;
				}

				case Event::Type::MouseRelease: {
					input::Mouse button = dynamic_cast<MouseEvent*>(curr)->button;
					NFLog(std::format("Event - MouseRelease (Button = {})", static_cast<unsigned char>(button)));
					break;
				}

				case Event::Type::MouseScroll: {
					MouseScrollEvent::Direction direction = dynamic_cast<MouseScrollEvent*>(curr)->direction;
					NFLog(std::format("Event - MouseScroll (Direction = {})", static_cast<int>(direction)));
					break;
				}

				case Event::Type::WindowClose:
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