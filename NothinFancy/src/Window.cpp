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
		while (GetMessage(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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

			case WM_CLOSE:
				DestroyWindow(hWnd);
				return 0;

			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}


	Window::~Window() {

	}
}