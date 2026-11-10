#include "pch.h"
#include "Input.h"

namespace gl
{
	bool Input::Initialize()
	{
		int numKeys;
		const bool* keyState = SDL_GetKeyboardState(&numKeys);
		m_keyStates.resize(numKeys);

		std::copy(keyState, keyState + numKeys, m_keyStates.begin());
		m_prevKeyStates = m_keyStates;

		return true;
	}

	void Input::Shutdown()
	{
		//
	}

	void Input::Update()
	{
		m_prevKeyStates = m_keyStates;
		const bool* keyState = SDL_GetKeyboardState(NULL);
		std::copy(keyState, keyState + m_keyStates.size(), m_keyStates.begin());

		m_prevButtonStates = m_buttonStates;
		m_prevMousePosition = m_mousePosition;
		m_buttonStates = SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
	}

	uint32_t Input::GetButtonBit(MouseButton button) const
	{
		return SDL_BUTTON_MASK((uint32_t)button);
	}
}