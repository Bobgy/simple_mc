#pragma once

#include "stdafx.h"

#include "scripts/script.h"

namespace scripts
{
	class SG002 : public ScriptGame
	{
	public:
		/* constructor and destructor */
		virtual ~SG002();

		/* common interface */
		void setup();
		virtual void tick(flt delta_time);

		// initializes the game
		virtual void setup_game();
	};
};