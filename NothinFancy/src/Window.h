#pragma once
#include <Windows.h>

namespace nf {
	class Window {
	public:
		Window(const char* title);

		void show(bool show = true);
		void update();

		~Window();
	private:
		void registerClass() const;
		RECT getMonitor();

		static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_window;
		const HINSTANCE m_hInst;
		const PCWSTR m_wndClassName;

		const DWORD m_windowedStyle;
		const DWORD m_bFullscreenStyle;
	};
}