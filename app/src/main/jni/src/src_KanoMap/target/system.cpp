
#include "target_dep.h"
#include <FSystem.h>

unsigned long target::system::GetTicks(void)
{
	static double s_guide = 0;

	if (s_guide == 0)
	{
		long long tick;
		Osp::System::SystemTime::GetTicks(tick);
		s_guide = tick;
	}

	long long tick;
	Osp::System::SystemTime::GetTicks(tick);
	double current_tick = tick;

	return (unsigned long)((int)(current_tick - s_guide));
}
