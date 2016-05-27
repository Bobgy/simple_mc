#pragma once

#include "stdafx.h"

#include "utility/vec.h"

namespace scripts
{
	class Script
	{
	public:
		/* constructor and destructor */
		virtual ~Script() = 0;

		/* common interface */
		void setup() {}
		virtual void tick(flt delta_time) = 0;
	};

	class ScriptLevel : public Script
	{
	public:
		virtual ~ScriptLevel() = 0;

		/* common interface */
		// inherits
	};

	class ScriptGame : public Script
	{
	public:
		virtual ~ScriptGame() = 0;

		/* common interface */
		// inherits

		// initializes the game
		virtual void setup_game() = 0;
	};
};
