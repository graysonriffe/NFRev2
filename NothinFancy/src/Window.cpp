#include "pch.h"
#include "Window.h"

#include "nf/Utility/util.h"

namespace nf {
	Window::Window(const char* title)
		: m_window(nullptr)
		, m_hInst(GetModuleHandle(NULL))
		, m_wndClassName(L"NothinFancyWindow")
		, m_windowedStyle(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
		, m_bFullscreenStyle(WS_SYSMENU)
	{
		registerClass();

		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

		RECT initialMonitor = getMonitor();
		m_window = CreateWindow(m_wndClassName, util::toWideStr(title).c_str(), m_windowedStyle, initialMonitor.left + 100, initialMonitor.top + 100, 300, 300, NULL, NULL, m_hInst, this);
	}

	void Window::show(bool show) {
		ShowWindow(m_window, show ? SW_SHOW : SW_HIDE);
	}

	void Window::update() {
		MSG msg = {};
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Window::EventQueue& Window::getQueue() {
		return m_events;
	}

	void Window::registerClass() const {
		WNDCLASS wc = {};
		wc.lpszClassName = m_wndClassName;
		wc.hInstance = m_hInst;
		wc.lpfnWndProc = wndProc;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);

		RegisterClass(&wc);
	}

	RECT Window::getMonitor() {
		MONITORINFO mi = {};
		mi.cbSize = sizeof(mi);
		POINT mouse = {};
		GetCursorPos(&mouse);
		GetMonitorInfo(MonitorFromPoint(mouse, MONITOR_DEFAULTTONEAREST), &mi);

		return mi.rcMonitor;
	}

	LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		static Window* window = nullptr;

		switch (msg) {
			case WM_CREATE:
				window = reinterpret_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
				return 0;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				if (static_cast<input::Key>(wParam) == input::Key::F4 && GetAsyncKeyState(static_cast<unsigned int>(input::Key::Alt)) & 0x8000) {
					window->m_events.emplace(new WindowCloseEvent);
					return 0;
				}

				if (!(lParam & (1 << 30))) //If not repeating
					window->m_events.emplace(new KeyPressEvent(static_cast<input::Key>(wParam)));
				return 0;

			case WM_KEYUP:
			case WM_SYSKEYUP:
				window->m_events.emplace(new KeyReleaseEvent(static_cast<input::Key>(wParam)));
				return 0;

			case WM_MENUCHAR:
				return MNC_CLOSE << 16;

			case WM_CLOSE:
				window->m_events.emplace(new WindowCloseEvent);
				return 0;

			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}


	Window::~Window() {
		DestroyWindow(m_window);
	}
}