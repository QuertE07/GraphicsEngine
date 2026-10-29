#pragma once

namespace gl
{
	class Input
	{
	public:
		enum MouseButton
		{
			Left = 1,
			Middle,
			Right
		};

		bool Initialize();
		void Shutdown();

		void Update();

		bool GetKeyDown(int key) const { return m_keyStates[key]; }
		bool GetPreviousKeyDown(int key) const { return m_prevKeyStates[key]; }
		bool GetButtonDown(MouseButton button) const { return m_buttonStates & GetButtonBit(button); }
		bool GetPrevButtonDown(MouseButton button) const { return m_prevButtonStates & GetButtonBit(button); }
		
		bool GetKeyPressed(int key) const { return !GetPreviousKeyDown(key) && GetKeyDown(key); }
		bool GetKeyReleased(int key) const { return GetPreviousKeyDown(key) && !GetKeyDown(key); }
		bool GetButtonPressed(MouseButton button) const { return !GetPrevButtonDown(button) && GetButtonDown(button); }
		bool GetButtonReleased(MouseButton button) const { return GetPrevButtonDown(button) && !GetButtonDown(button); }

		bool GetMouseDown(int button) const { return false; }

		glm::vec2 GetMousePosition() const { return m_mousePosition; }

	private:
		// keyboard
		std::vector<bool> m_keyStates;
		std::vector<bool> m_prevKeyStates;

		// mouse
		uint32_t m_buttonStates = 0;
		uint32_t m_prevButtonStates = 0;

		glm::vec2 m_mousePosition{ 0, 0 };

		uint32_t GetButtonBit(MouseButton button) const;
	};
}