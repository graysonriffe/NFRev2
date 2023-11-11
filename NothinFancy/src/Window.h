#pragma once
#include <Windows.h>

#include "Event.h"
#include "nf/Config.h"

namespace nf {
	class Window {
	public:
		using EventQueue = std::queue<Event*>;

		Window(const char* title);

		RECT getMonitor();
		void show(bool show = true);
		void update();
		EventQueue& getQueue();
		HWND getHandle() const;
		void setType(DisplayConfig::Mode mode);

		~Window();
	private:
		void registerClass() const;
		static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void handleMouseButtons(bool pressed, input::Mouse button);

		HWND m_handle;
		const HINSTANCE m_hInst;
		const PCWSTR m_wndClassName;

		const DWORD m_windowedStyle;
		const DWORD m_bFullscreenStyle;

		EventQueue m_events;
	};
}