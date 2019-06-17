#ifdef APP_CRAMB

#include "cmd_cramb.h"
#include "../task_cramb.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include "../../../sys/debug.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../cabinet.h"
#include "../../../drv/pwm.h"

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(3)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"init", "Start cramb controller"},
		{"deinit", "Stop cramb controller"},
		{"hello", "print 'hello' to screen"}
};

void cmd_cramb_register(void)
{
	// Populate the command entry block (struct)
	commands_cmd_init(&cmd_entry,
			"cramb", "Combined radial axial magnetic bearing commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_cramb
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'cramb' command
// and all sub-commands
//
int cmd_cramb(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure cramb task was not already inited
		if (task_cramb_is_inited()) return FAILURE;

		task_cramb_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure cramb task was already inited
		if (!task_cramb_is_inited()) return FAILURE;

		task_cramb_deinit();
		return SUCCESS;
	}

	// Handle 'hello' sub-command
	if (strcmp("hello", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;


		// "cramb hello"
		debug_printf("hello\n\r");
		pwm_set_duty(CAB_INV6_PHA, 0.5);
		pwm_set_duty(CAB_INV6_PHB, 0.0);
		pwm_set_duty(CAB_INV6_PHC, 0.0);
		return SUCCESS;

	}

	return INVALID_ARGUMENTS;
}

#endif //APP_CRAMB





