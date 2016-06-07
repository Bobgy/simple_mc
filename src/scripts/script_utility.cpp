#include "stdafx.h"

#include "script_utility.h"

#include "game/game.h"
#include "game/event_manager.h"
#include "game/world.h"
#include "utility/keyboard.h"

void InitKeys()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;

	Keyboard *keyboard = game->getKeyboard();
	EventManager *event_manager = game->getEventManager();
	if (keyboard == nullptr || event_manager == nullptr) return;

	event_manager->clear();

	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'w', STRING_ID("forward"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'s', STRING_ID("backward"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'p', STRING_ID("switch_display"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'[', STRING_ID("simple_cube"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'`', STRING_ID("pause/resume"));

	auto display_callback = make_shared<function<void(void)>>([]() {
		bDisplay ^= 1;
	});
	if (display_callback) {
		event_manager->registerEventCallback(
			STRING_ID("switch_display"),
			display_callback,
			EnumEventType::ON_UP,
			true);
	}

	auto simple_render_callback = make_shared<function<void(void)>>([]() {
		bCustomGLSL ^= 1;
		bSimpleCube ^= 1;
		CurrentGame()->getWorld()->changed = true;
	});
	if (simple_render_callback) {
		event_manager->registerEventCallback(
			STRING_ID("simple_cube"),
			simple_render_callback,
			EnumEventType::ON_UP,
			true);
	}

	auto pause_resume_callback = make_shared<CallBackFunction>([]() {
		if (GAME_SPEED) CurrentGame()->pause();
		else CurrentGame()->resume();
	});
	if (pause_resume_callback) {
		event_manager->registerEventCallback(
			STRING_ID("pause/resume"),
			pause_resume_callback,
			EnumEventType::ON_UP,
			true);
	}


	for (uint8_t i = '1'; i <= '5'; ++i) {
		char stmp[100] = "select_level_1";
		stmp[strlen(stmp) - 1] = i;
		StringID event = STRING_ID(stmp);
		event_manager->registerEventTrigger(keyboard->m_key_event_board, i, event);
		auto select_level_x = make_shared<CallBackFunction>([i]() {
			g_selected_level_script = (int32_t)i - (int32_t)'0';
			if (g_selected_level_script == 2) g_selected_game_script = 2;
			else g_selected_game_script = 1;
			g_require_reset = true;
			CurrentGame()->pause();
		});
		if (select_level_x) {
			event_manager->registerEventCallback(
				event,
				select_level_x,
				EnumEventType::ON_UP,
				true);
		}
	}
}