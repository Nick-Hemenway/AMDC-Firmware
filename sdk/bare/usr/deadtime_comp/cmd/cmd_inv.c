#ifdef APP_DEADTIME_COMP

#include "cmd_inv.h"
#include "../inverter.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(1)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"dtc <mDcomp> <mTau>", "Set deadtime compensation"}
};

void cmd_inv_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"inv", "Inverter commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_inv
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'inv' command
// and all sub-commands
//
int cmd_inv(int argc, char **argv)
{
	// Handle 'dtc' sub-command
	if (strcmp("dtc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 4) return INVALID_ARGUMENTS;

		// Pull out mDcomp argument
		// and saturate to 0 .. 0.2
		double mDcomp = (double) atoi(argv[2]);
		if (mDcomp > 200.0) return INVALID_ARGUMENTS;
		if (mDcomp < 0.0) return INVALID_ARGUMENTS;

		// Pull out mTau argument
		// and saturate to 100 .. 1000
		double mTau = (double) atoi(argv[3]);
		if (mTau > 1000.0) return INVALID_ARGUMENTS;
		if (mTau <  100.0) return INVALID_ARGUMENTS;

		double dcomp = mDcomp / 1000.0;
		double tau = mTau / 1000.0;

		inverter_set_dtc(dcomp, tau);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

#endif // APP_DEADTIME_COMP
