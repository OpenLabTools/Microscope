// Full Focusing Implementation

/* This program combines the sweep and fine_tune methods of the autofocusing class to create a fully automated focusing program.
 */ 

#ifndef ALLFOCUSING_H
#define ALLFOCUSING_H

#include "autofocus_class.h"
#include "autofocus_class_initialisation.h"


void focus_full(Autofocus&);
void focus_sweep(Autofocus&);
void focus_test(Autofocus&);
void focus_tune(Autofocus&);




void focus_full(Autofocus &autof)
{
	
	cout << "\nRunning full version of focusing program..." << endl;
	
	// Get length of travel
	int whole_distance;
	autof.comm_get_z_len(whole_distance);
	
	
	// Move to starting position (in this case we are already there, but easily modifiable)
	autof.comm_move_to(whole_distance);
	autof.stop_stage();
	//cout << "\nMoved back to starting position" << endl;
	
	
	// Run preliminary sweep
	autof.sweep();
	
	
	//cout << "\nGoing to position " << autof.get_max_pos() << " corresponding to image " << autof.get_max_index() << endl;
	// Move to maximum
	int max_position = autof.get_max_pos();
	autof.comm_move_to(max_position);
	autof.stop_stage();
	
	
	// Reduce number of steps
	autof.set_steps(0.5*autof.get_steps());
	
	
	// Run fine tuning
	bool tuning_done = false;
	cout << "\nRunning fine tuning" << flush;
	tuning_done = autof.fine_tune();
	cout << endl;
	
	if (tuning_done == true)
	{
		cout << "\nI think I am done, check this out!" << endl;
		sleep(1);
		system("raspivid -t 10000");
	}
	else
	{
		cout << "\nWeirdly enough, it returned 'false'" << endl;
	}

	return;
	
}



void focus_sweep(Autofocus &autof)
{

	cout << "\nRunning sweep program..." << endl;
	
	
	// Get length of travel
	int whole_distance;
	autof.comm_get_z_len(whole_distance);
	
	
	// Move to starting position (in this case we are already there, but easily modifiable)
	autof.comm_move_to(whole_distance);
	autof.stop_stage();
	cout << "\nMoved back to starting position" << endl;
	
	
	// Run sweep
	autof.sweep();
	
	
	// Moving to the maximum
	char choice;
	cout << "\n\tMove to maximum found from sweep (y/n)? "; cin >> choice;
	
	if (choice == 'y')
	{
		
		cout << "\nGoing to position " << autof.get_max_pos() << " corresponding to image " << autof.get_max_index() << endl;

		autof.comm_move_to(autof.get_max_pos());
		autof.stop_stage();
		
	}
	
	return;
	
}



void focus_test(Autofocus &autof)
{
	
	cout << "\nRunning test program..." << endl;

	
	autof.set_output(true);
	
	// Resolution of image and control values
	int number_images = 0;
	cout << "\n\tHow many images? "; cin >> number_images;

	
	// Do test run
	autof.test_run(number_images);
	
	return;
	
}



void focus_tune(Autofocus &autof)
{

	cout << "\nRunning tuning program..." << endl;
		
	
	int whole_distance;
	// Get length of travel
	autof.comm_get_z_len(whole_distance);
	
	
	// Move to starting position (in this case we are already there, but easily modifiable)
	autof.comm_move_to(whole_distance);
	autof.stop_stage();
	cout << "\nMoved back to starting position" << endl;
	
	
	// Manual or automatic first adjustment
	char choice;
	bool try_again = true;
	
	
	while (try_again == true)
	{
		
		cout << "\n\tWould you like to first adjust manually (y/n)? "; cin >> choice; cin.ignore();
		
		if (choice == 'y')
		{
				
			cout << "\nWaiting for manual adjustment. Press 'Enter' when done."; cin.ignore();
			
			if (autof.get_objective().compare("4x") == 0)
			{
				autof.set_steps(200);
				cout << "\nRunning fine tuning" << flush;
				autof.fine_tune();
			}
			else if (autof.get_objective().compare("10x") == 0)
			{
				autof.set_steps(40);
				cout << "\nRunning fine tuning" << flush;
				autof.fine_tune();
			}
			else if (autof.get_objective().compare("40x") == 0)
			{
				autof.set_steps(10);
				cout << "\nRunning fine tuning" << flush;
				autof.fine_tune();
			}
			else if (autof.get_objective().compare("100x") == 0)
			{
				autof.set_steps(3);
				cout << "\nRunning fine tuning" << flush;
				autof.fine_tune();
			}
			cout << endl;
			
			cout << "\nI think I am done!" << endl;
			
			char trying;
			cout << "\n\tTry again with same parameters (y/n)? "; cin >> trying; cin.ignore();
			
			if (trying == 'n')
				try_again = false;
			
		}
		else
		{
		
 			int start_from_here;
			cout << "\nStarting point for focus: "; cin >> start_from_here;
			autof.comm_move_to(start_from_here);
			autof.stop_stage();
			//cout << "\nMoved close to focus point" << endl;
						
			
			// Set initial number of steps
			autof.set_steps(200);
			
			
			// Run fine tuning
			cout << "\nRunning fine tuning" << flush;
			
			autof.fine_tune();
		
			char trying;
			cout << "\n\tTry again (y/n)? "; cin >> trying;
			
			if (trying == 'n')
				try_again = false;
			
		}

	}
	
	return;
	
}

void serial_send(Autofocus &autof)
{
	
	string argument_string;
	int argument_int;
	string command;
	bool serial_commanding = true;
	while (serial_commanding)
	{
		
		cout << "\n\tWhat Arduino command would you like to use?\n\t('z_get_length', 'z_get_position', 'z_move', 'z_move_to', 'set_ring_colour', 'set_ring_brightness', 'set_stage_led_brightness', 'exit')\n\n\t" << flush;
		cin >> command;
		
		if (command.compare("z_get_length") == 0)
		{
			int len;
			cout << endl;
			autof.comm_get_z_len(len, true);
		}
		else if (command.compare("z_get_position") == 0)
		{
			int pos;
			cout << endl;
			autof.comm_get_z_pos(pos, true);
		}
		else if (command.compare("z_move") == 0)
		{
			cout << "\tArgument: " << flush;
			cin >> argument_int; cout << endl;
			autof.comm_move(argument_int, true);
		}
		else if (command.compare("z_move_to") == 0)
		{
			cout << "\tArgument: " << flush;
			cin >> argument_int; cout << endl;
			autof.comm_move_to(argument_int, true);
		}
		else if (command.compare("set_ring_colour") == 0)
		{
			cout << "\tArgument: " << flush;
			cin >> argument_string; cout << endl;
			autof.comm_set_ring_colour(argument_string, true);
		}
		else if (command.compare("set_ring_brightness") == 0)
		{
			cout << "\tArgument: " << flush;
			cin >> argument_int; cout << endl;
			autof.comm_set_ring_bright(argument_int, true);
		}
		else if (command.compare("set_stage_led_brightness") == 0)
		{
			cout << "\tArgument: " << flush;
			cin >> argument_int; cout << endl;
			autof.comm_set_led_bright(argument_int, true);
		}
		else if (command.compare("exit") == 0)
		{
			cout << "\nExiting..." << endl;
			serial_commanding = false;
		}
		else
		{
			cout << "\n\tCommand not recognised. Please use the commands provided in parenthesis." << endl;
		}
	
	}
		
	return;
	
}


#endif
