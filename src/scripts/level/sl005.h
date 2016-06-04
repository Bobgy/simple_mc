#pragma once

#include "stdafx.h"

#include "scripts/script.h"

namespace scripts
{
	class SL005 : public ScriptLevel
	{
	public:
		virtual ~SL005();

		/* common interface */
		// inherits
		void setup();
		virtual void tick(flt delta_time);

		// initializes the level
		virtual void setup_level();
	};
};
