// Autofocus Class Initialisation

/* Provides user interface to initialise the Autofocus class.
 * the autofocus_initialisation(Autofocus&) method should be used to set up the class the first time.
 * autofocus_reset(Autofocus&) can be used to change parameters while the program is running.
 */

#ifndef AUTOFOCUS_CLASS_INITIALISATION_H
#define AUTOFOCUS_CLASS_INITIALISATION_H

#include "autofocus_class.h"

void autofocus_initialisation(Autofocus&);
void autofocus_reset(Autofocus&);


// Initialising parameters in class autodoing
// To be used to change the default parameters normally used at class initialisation
// Remember to open serial port, as standard constructor of class does not do so
void autofocus_initialisation(Autofocus &autodoing)
{
	
	autodoing.set_width();
	autodoing.set_height();
	
	// ## CONSTRUCTOR VARIABLES SELECTION
	
	// Type in serial port to connect Arduino
	string port;
	cout << "\n\tOn what port is the arduino (default)?\n\t" << flush; cin >> port;
	if (port.compare("default") == 0)
		port = "/dev/ttyUSB0";
	else if (port[0] == 't')
		port = "/dev/" + port;
	else
		port = "/dev/tty" + port;
		
	autodoing.set_serial(port);
	
	
	// Objective picking
	string objective;
	cout << "\n\tWhat kind of objective will we be using (4x, 10x or 40x)?\n\t" << flush; cin >> objective;
	
	autodoing.set_objective(objective);
	
	
	// Choose whether to maintain output or not, and create variables for it
	char output_yes_no;
	bool keep_output = true;
	string read_path;
	string read_name;
	cout << "\n\tWould you like to keep the output (y/n)? "; cin >> output_yes_no;
	if (output_yes_no == 'n')
	{
		keep_output = false;
		read_path = "./.temporary/";
		read_name = "temporary";
	}
	else
	{
		// Choose path for output
		cout << "\n\tType where images will be stored (full path or just name of folder):\n\t" << flush; cin >> read_path;
		if (read_path[0] != '/')
			read_path.insert(0, "./");
		if (read_path[read_path.length()-1] != '/')
			read_path.push_back('/');
				
		// Choose common name for images to be saved
		cout << "\n\tType common name of images:\n\t" << flush; cin >> read_name;
	}
	
	autodoing.set_output(keep_output);
	autodoing.set_path(read_path);
	autodoing.set_name(read_name);
	autodoing.set_file();
	autodoing.set_strings();
		
	
	// Set brightness of backlight LED
	int brightness = 0;
	cout << "\n\tChoose brightness of LED for backlight (0-255, 0 for off, -1 for default):\n\t" << flush; cin >> brightness;
	if (brightness < 0 || brightness > 255) 
	{
		brightness = 70;
		cout << "\nUsing default value, " << brightness << endl;
	} 
	
	autodoing.comm_set_led_bright(brightness);
	
	
	// Set tolerance of working
	float tolerance;
	cout << "\n\tChoose tolerance of comparison between values (0->1):\n\t" << flush; cin >> tolerance;
	if (tolerance < 0 || tolerance > 1)
	{
		tolerance = 0.98;
		cout << "\nUsing default value, " << tolerance << endl;
	}
	
	autodoing.set_tolerance(tolerance);
	
	// ## END OF CONSTRUCTOR VARIABLES SELECTION
	
	return;
	
}



// Resetting parameters for new call of the program
void autofocus_reset(Autofocus &autodoing)
{
	
	char change;
	
	// Change objective
	cout << "\n\tDo you wish to change objective (y/n)? "; cin >> change;
	if (change == 'y')
	{
		string obj;
		cout << "\n\tWhat kind of objective will we be using (4x, 10x or 40x)?\n\t" << flush; cin >> obj;
		autodoing.set_objective(obj);
	}
	
	
	// Change path of next operation
	bool keep_output = true;
	string read_path;
	string read_name;
	cout << "\n\tDo you wish to keep the output for the next operation (y/n)? "; cin >> change;
	if (change == 'n')
	{
		keep_output = false;
		read_path = "./.temporary/";
		read_name = "temporary";
	}
	else
	{
		// Choose path for output
		cout << "\n\tType where images will be stored (full path or just name of folder):\n\t" << flush; cin >> read_path;
		if (read_path[0] != '/')
			read_path.insert(0, "./");
		if (read_path[read_path.length()-1] != '/')
			read_path.push_back('/');
				
		// Choose common name for images to be saved
		cout << "\n\tType common name of images:\n\t" << flush; cin >> read_name;
	}
	autodoing.set_output(keep_output);
	autodoing.set_path(read_path);
	autodoing.set_name(read_name);
	autodoing.set_strings();
	
	
	// Set brightness of LED
	int brightness = 0;
	cout << "\n\tChoose brightness of LED for backlight (0-255, 0 for off, -1 for default):\n\t" << flush; cin >> brightness;
	if (brightness < 0 || brightness > 255) 
	{
		brightness = 70;
		cout << "\nUsing default value, " << brightness << endl;
	} 
	autodoing.comm_set_led_bright(brightness);
	
	
	// Set tolerance of working
	float tolerance;
	cout << "\n\tChoose tolerance of comparison between values (0->1):\n\t" << flush; cin >> tolerance;
	if (tolerance < 0 || tolerance > 1)
	{
		tolerance = 0.98;
		cout << "\nUsing default value, " << tolerance << endl;
	}
	autodoing.set_tolerance(tolerance);
	
	return;
	
}

#endif
