#include "stdafx.h"

#include "script_utility.h"

#include "game/game.h"
#include "game/event_manager.h"
#include "game/world.h"
#include "utility/keyboard.h"

struct KeyEvent
{
	uint8_t m_key;
	const char *m_event;
};

static KeyEvent g_bindings[] = {
	{(uint8_t)'w', "forward"},
	{(uint8_t)'s', "backward"},
	{(uint8_t)'p', "switch_display"},
	{(uint8_t)'[', "simple_cube"},
	{(uint8_t)'`', "pause/resume"},
	{(uint8_t)'\\', "switch_priority"},
	{(uint8_t)'-', "remove_entities"},
	{(uint8_t)'=', "add_entities"},
};

void InitKeys()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;

	Keyboard *keyboard = game->getKeyboard();
	EventManager *event_manager = game->getEventManager();
	if (keyboard == nullptr || event_manager == nullptr) return;

	event_manager->clear();

	for (auto &binding : g_bindings) {
		event_manager->registerEventTrigger(
			keyboard->m_key_event_board,
			binding.m_key,
			STRING_ID(binding.m_event));
	}

	auto display_callback = make_shared<CallBackFunction>([]() {
		bDisplay ^= 1;
	});
	event_manager->registerEventCallback(
		STRING_ID("switch_display"),
		display_callback);

	auto simple_render_callback = make_shared<CallBackFunction>([]() {
		bCustomGLSL ^= 1;
		bSimpleCube ^= 1;
		CurrentGame()->getWorld()->changed = true;
	});
	event_manager->registerEventCallback(
		STRING_ID("simple_cube"),
		simple_render_callback);

	auto pause_resume_callback = make_shared<CallBackFunction>([]() {
		if (GAME_SPEED) CurrentGame()->pause();
		else CurrentGame()->resume();
	});
	event_manager->registerEventCallback(
		STRING_ID("pause/resume"),
		pause_resume_callback);

	auto switch_priority_callback = make_shared<CallBackFunction>([]() {
		bPriorityEnabled ^= 1;
	});
	event_manager->registerEventCallback(
		STRING_ID("switch_priority"),
		switch_priority_callback);

	auto add_num_callback = make_shared<CallBackFunction>([]() {
		NUM += 10;
	});
	event_manager->registerEventCallback(
		STRING_ID("add_entities"),
		add_num_callback);

	auto remove_num_callback = make_shared<CallBackFunction>([]() {
		NUM -= 10;
		NUM = max(NUM, 0);
	});
	event_manager->registerEventCallback(
		STRING_ID("remove_entities"),
		remove_num_callback);


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