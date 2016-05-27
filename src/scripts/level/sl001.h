#pragma once

#include "stdafx.h"

#include "scripts/script.h"

namespace scripts
{
	class SL001 : public ScriptLevel
	{
	public:
		virtual ~SL001();

		/* common interface */
		// inherits
		void setup();
		virtual void tick(flt delta_time);

		// initializes the level
		virtual void setup_level();
	};
};