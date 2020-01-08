#ifdef APP_CRAMB

#include "cmd_cramb_axial.h"
#include "../task_cramb_axial.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include "../../../sys/debug.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../cabinet.h"
#include "../../../drv/pwm.h"
#include "../../../drv/analog.h"

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(10)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"init_cc", "Start current controller"},
		{"deinit_cc", "Stop current controller"},
		{"pole_volts", "Set Pole Voltages"},
		{"set_volts", "Set voltage of output"},
		{"set_current", "Set currents value in microamps"},
		{"read_current", "print instantaneous current to screen"},
		{"read_adc", "read voltage on ADC (for determining current sensor gain)"},
		{"set_vdc", "set the DC bus voltage"},
		{"set_cc_gains","Set the current controller gains by supplying machine parameters"},
		{"set_inverter", "Set inverter output on the cabinet"}
};

void cmd_cramb_axial_register(void)
{
	// Populate the command entry block (struct)
	commands_cmd_init(&cmd_entry,
			"cramb_axial", "Combined radial axial magnetic bearing axial commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_cramb_axial
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'cramb' command
// and all sub-commands
//
int cmd_cramb_axial(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init_cc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure cramb task was not already inited
		if (task_cramb_axial_is_inited()) return FAILURE;

		task_cramb_axial_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit_cc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure cramb task was already inited
		if (!task_cramb_axial_is_inited()) return FAILURE;

		task_cramb_axial_deinit();
		return SUCCESS;
	}


	// Handle 'read_currents' sub-command
	if (strcmp("read_current", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		double I_measured;

		read_current_axial(&I_measured);

		debug_printf("%f\n\r", I_measured);

		return SUCCESS;

	}

	// Handle 'pole_volts' sub-command
	if (strcmp("pole_volts", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 4) return INVALID_ARGUMENTS;

		//convert command line arguments to doubles
		double Va, Vb;
		Va = ((double) atoi(argv[2]))/1e6;
		Vb = ((double) atoi(argv[3]))/1e6;

		//set pole voltages using command line inputs
		set_pole_volts_axial(Va, Vb);

		return SUCCESS;

	}

	// Handle 'phase_volts' sub-command
	if (strcmp("set_volts", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		//convert command line arguments to doubles
		double V_out;
		V_out = ((double) atoi(argv[2]))/1e6;

		//set line to neutral voltages using command line inputs
		set_voltage_axial(V_out);

		return SUCCESS;

	}

	// Handle 'set_currents' sub-command
	if (strcmp("set_current", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		//convert command line arguments to doubles
		double I_command;
		I_command = ((double) atoi(argv[2]))/1e6;

		//set pole voltages using command line inputs
		set_current_axial(I_command);

		return SUCCESS;

	}

	// Handle 'read_adc' sub-command
	if (strcmp("read_adc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		double adc;

		read_adc_axial(&adc);

		debug_printf("%f\n\r", adc);

		return SUCCESS;

	}

	// Handle 'set_vdc' sub-command
	if (strcmp("set_vdc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		//convert command line arguments to doubles
		double Vdc;
		Vdc = ((double) atoi(argv[2]))/1e3;

		//set pole voltages using command line inputs
		set_vdc_axial(Vdc);

		return SUCCESS;

	}

	// Handle 'set_cc_gains' sub-command
	if (strcmp("set_cc_gains", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 5) return INVALID_ARGUMENTS;

		//convert command line arguments to doubles
		double R, L, fb; //current controller bandwidth [hz]
		R = (double) atoi(argv[2])/1e3; //resistance in miliOhms
		L = (double) atoi(argv[3])/1e6; //inductance in microHenry
		fb = (double) atoi(argv[4]); //frequency in Hz

		//set pole voltages using command line inputs
		set_cc_gains_axial(R, L, fb);

		return SUCCESS;

	}

	// Handle 'set_inverter' sub-command
	if (strcmp("set_inverter", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		//convert command line arguments to doubles
		int inv_num; //current controller bandwidth [hz]
		inv_num = atoi(argv[2]); //inverter number

		//set pole voltages using command line inputs
		if ((inv_num > -1) && (inv_num < CABINET_NUM_INVERTERS)){
			set_inverter_axial(inv_num);
		}

		return SUCCESS;

	}

	return INVALID_ARGUMENTS;
}

#endif //APP_CRAMB





