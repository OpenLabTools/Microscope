// Focusing Main Program

/* Provides controls for the functionalities of the microscope, and a basic user interface through terminal.
 */ 

#include "autofocus_class.h"
#include "autofocus_class_initialisation.h"
#include "focus_everything.h"


int main () 
{
	
	Autofocus autofocusing;
	
	char defaults;
	cout << "\nDo you wish to input your own parameters (y/n)? "; cin >> defaults;
	if (defaults == 'y')
		autofocus_initialisation(autofocusing);
	else if (defaults == 'n')
	{
		cout << "\nUsing default parameters..." << endl;
		autofocusing.set_serial();
		cout << "Serial port: " << autofocusing.get_serial() << endl;
		cout << "Objective: " << autofocusing.get_objective() << endl;
		cout << "Keeping output: " << autofocusing.get_output() << endl;
		autofocusing.set_path();
		cout << "Destination folder: " << autofocusing.get_path() << endl;
		autofocusing.set_file();
		cout << "File for data saving: " << autofocusing.get_file() << endl;
		autofocusing.set_name();
		cout << "Common picture name: " << autofocusing.get_name() << endl;
		autofocusing.comm_set_led_bright(70);
		cout << "Backlight LED brightness (0->255): " << 70 << endl;
		cout << "Tolerance (0->1): " << autofocusing.get_tolerance() << endl;
	}
	else
	{
		cout << "\nNOT RECOGNISED! Using default parameters..." << endl;
		autofocusing.set_serial();
		cout << "Serial port: " << autofocusing.get_serial() << endl;
		cout << "Objective: " << autofocusing.get_objective() << endl;
		cout << "Keeping output: " << autofocusing.get_output() << endl;
		autofocusing.set_path();
		cout << "Destination folder: " << autofocusing.get_path() << endl;
		autofocusing.set_file();
		cout << "File for data saving: " << autofocusing.get_file() << endl;
		autofocusing.set_name();
		cout << "Common picture name: " << autofocusing.get_name() << endl;
		autofocusing.comm_set_led_bright(70);
		cout << "Backlight LED brightness (0->255): " << 70 << endl;
		cout << "Tolerance (0->1): " << autofocusing.get_tolerance() << endl;
	}
	

	cout << "\nWARNING: Remember to calibrate at the start of the program!" << flush;
	
	bool keep_working = true;
	int times_cycled = 0;
	while (keep_working)
	{
		
		string program_choice;
		cout << "\n\tWhat program would you like to run ('full', 'sweep', 'tune', 'test', 'calibrate', 'serial', 'exit')?\n\t" << flush; cin >> program_choice;
		
		if (times_cycled > 1 && program_choice.compare("exit") != 0 && program_choice.compare("serial") != 0)
		{
			char reset_choice;
			cout << "\n\tDo you wish to change some parameters for the next execution (y/n)? "; cin >> reset_choice;
			if (reset_choice == 'y')
				autofocus_reset(autofocusing);
			else if (reset_choice == 'n')
				cout << "\nContinuing with previously set parameters..." << endl;
			else
				cout << "\nNOT RECOGNISED! Continuing with previously set parameters..." << endl;
		}
		
		if (program_choice.compare("full") == 0)
		{
			focus_full(autofocusing);
		}
		else if (program_choice.compare("sweep") == 0)
		{
			focus_sweep(autofocusing);
		}
		else if (program_choice.compare("tune") == 0)
		{
			focus_tune(autofocusing);
		}
		else if (program_choice.compare("test") == 0)
		{
			focus_test(autofocusing);
		}
		else if (program_choice.compare("calibrate") == 0)
		{
			autofocusing.comm_calibrate();
			autofocusing.stop_stage(autofocusing.get_calibrate());
			int path_length;
			autofocusing.comm_get_z_len(path_length);
			cout << "Path length: " << path_length << endl;
		}
		else if (program_choice.compare("serial") == 0)
		{
			serial_send(autofocusing);
		}
		else if (program_choice.compare("exit") == 0)
		{
			keep_working = false;
		}
		else
		{
			cout << "\nChoice not recognised. Please use the commands provided in parenthesis." << endl;
			continue;
		}
		
		times_cycled++;
		
	}
		
		
	cout << endl;
	return 0;
	
}
