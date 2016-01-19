// Autofocus Class

/* This file contains the autofocus class used in the implementation of the autofocusing algorithm.
 * The class is quite complex and contains many subsidiary methods and variables that are described in their specific comments underneath, so we won't mention them here.
 * For a more comprehensive understanding of all methods of this class read the relevant comments in the whole file.
 * Following is a brief description of the main methods of this class.
 * 
 * public:
 * Autofocus() -> Constructs the class with default values.
 * Autofocus(...) -> Constructs the class with values taken as input at construct time.
 * 		See details of this constructor below.
 * ~Autofocus() -> Destroys the class, removing the output folder if required by the user.
 * fine_tune() -> Tunes the microscope around the starting point, stopping at the focal point.
 * sweep() -> Executes a quick sweep over a long range of movement, taking a few images to find a rough estimate of the focus point position.
 * keep_in_focus() -> Recursively checks whether the microscope is in focus and corrects focusing point if wrong.
 * test_run(int) -> Executes a test run composed of equally spaced images, saving their focusing value in the file created at class construction.
 * 		Takes the number of images to be taken as input.
 * stop_stage(string) -> Verifies if the stage has finished moving before continuing with other operations.
 * 		Takes the command previously sent to move the stage as input. 
 * 
 * private:
 * algorithm() -> Computes the focusing value of the last image saved.
 * greyfy() -> Turns the last image saved into greyscale.
 * raspistill_save() -> Saves an image using RaspiStill from terminal.
 * remove_picture() -> Deletes the latest image saved.
 * remove_folder() -> Deletes the output folder, with all its content.
 * move_and_capture(int, int&) -> Moves the stage by a certain number of steps (first input) and takes a picture.
 * 		It then computes the focusing value using algorithm() and stores the position reached in the second input.
 * serial_command(...) -> Sends a command to the Arduino through the serial port, and handles the output resulting.
 * 		Read comments on the function for more details. 
 */

#ifndef AUTOFOCUS_CLASS_H
#define AUTOFOCUS_CLASS_H

#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "CImg.h"

using namespace cimg_library;
using namespace std;
using namespace boost::asio;


class Autofocus
{

/* PRIVATE SECTION */	
private:

	// Interaction variables and data storage
	io_service m_io;
	serial_port m_sp;
	vector<float> m_f_values;
	ofstream m_values;
	boost::asio::streambuf m_buffering;
	bool m_leave_output;
	string m_serial;
	
	// Numberic variables
	float m_f_max;
	int m_f_max_pos;
	int m_f_max_ind;
	int m_ind;
	int m_pos;
	
	int m_whole_distance;
	int m_steps;
	int m_min_steps;
	int m_number_images_sweep;
	int m_number_of_times;
	float m_precision;
	
	// Control strings for terminal commands
	string m_width;
	string m_height;
	string m_path;
	string m_name;
	
	string m_first_part_command;
	string m_first_part_name;
	string m_picture_input;
	string m_objective;
	
	// Image object
	CImg<float> m_picture;
	
	// String commands...
	// ...without arguments
	string m_calibrate;
	string m_is_cal;
	string m_get_z_len;
	string m_get_z_pos;
	string m_get_z_distance;
	
	// ...with arguments
	string m_move_to;
	string m_move;
	string m_set_ring_colour;				//takes a string as argument, not a number
	string m_set_ring_bright;		//takes a number only between 0-255, 0 for off. Set to a default value otherwise
	string m_set_stage_led_bright;	//takes a number only between 0-255, 0 for off. Set to a default value otherwise
	
	// Parts of commands accessible only from within the class...
	string m_number_steps;	
	string m_number_pos;
	// ...indicating stopping controls
	string m_OK;
	string m_endpoint;
	
	
	// Private functions for class usage only
	// See function declaration for more details
	float algorithm();
	
	void greyfy();
	
	void raspistill_save();
	
	void remove_picture();
	void remove_folder();
	
	float move_and_capture(int steps, int &f_pos);
	
	bool serial_command(string command, int &number, bool couting = false, string check = "OK\r");
	bool serial_command(string command, string argument = "000000", bool couting = false, string check = "OK\r");
	
	
	
	
/* PUBLIC SECTION */
public:
	
	
	/* FUNCTION DECLARATION */
	// Constructors and destructor
	Autofocus();
	
	Autofocus(string serial_device, string objective_type,
			string path, string name,
			bool leave_output = true,
			string width = "480", string height = "360");
			
	void initialise();

	~Autofocus();
	
	
	
	// SET and GET functions
	// See details below for each functions working
	float get_max() 
	{	return m_f_max;	}
	
	int get_max_pos() 
	{	return m_f_max_pos;	}
	
	int get_max_index() 
	{	return m_f_max_ind;	}
	
	int get_steps() 
	{	return m_steps;	}
	
	void set_steps(int steps) 
	{	m_steps = steps; return;	}
		
	void set_tolerance(float precision = 0.99)
	{	m_precision = precision; return;	}
	float get_tolerance()
	{	return m_precision;	}
	
	// Opens a serial port, by default on ttyUSB0.	
	void set_serial(string s_port = "/dev/ttyUSB0")
	{	m_serial = s_port; m_sp.open(s_port); sleep(3); return;	}
	string get_serial()
	{	return m_serial;	}
	
	void set_width(string width = "480")
	{	m_width = width; return;	}
	string get_width()
	{	return m_width;	}
	
	void set_height(string height = "360")
	{	m_height = height; return;	}
	string get_height()
	{	return m_height; 	}
	
	// Creates a directory and path where all output will be saved
	void set_path(string path = "./test/")
	{
		m_path = path;
		set_strings();
		
		// Create directory where to work
		string making_directory = "mkdir " + path;
		system(making_directory.c_str());
		return; 
	}
	string get_path()
	{	return m_path;	}
	
	// Sets the common name for images
	void set_name(string name = "test")
	{
		m_name = name; 
		set_strings();
		return; 
	}
	string get_name()
	{	return m_name;	}
	
	// Sets command strings for raspistill command to send to terminal and for saving images with correct path and name
	void set_strings()
	{
		m_first_part_command = "raspistill -n -w " + m_width + " -h " + m_height + " -o " + m_path + m_name;
		m_first_part_name = m_path + m_name;
		return;
	}
	
	// Sets a file where the values computed with the focusing formula are saved
	void set_file()
	{
		// Open output file for focusing values
		m_values.open((m_path + "focusingdata.txt").c_str());
		return;
	}
	string get_file()
	{	return m_path + "focusingdata.txt";	}
	
	// Sets whether the output should be deleted at the end or not (YES by default)
	void set_output(bool output = false)
	{	m_leave_output = output; return;	}
	bool get_output()
	{	return m_leave_output;	}
	
	// Sets minimum number of steps and initial number of steps depending on the objective chosen
	void set_objective(string objective_type = "4x")
	{
		m_objective = objective_type;
		// Set limiters for fine tuning
		if (objective_type.compare("4x") == 0)
		{
			m_min_steps = 5;
			m_steps = 560;
		}
		else if (objective_type.compare("10x") == 0)
		{
			m_min_steps = 2;
			m_steps = 100;
		}
		else if (objective_type.compare("40x") == 0)
		{
			m_min_steps = 1;
			m_steps = 20;
		}
		else if (objective_type.compare("100x") == 0)
		{
			m_min_steps = 1;
			m_steps = 5;
		}
		else
		{
			// Use default values.
			cout << "\nObjective not recognised, using default values." << endl;
			m_min_steps = 10;
			m_steps = 560;
		}
		return;
	}
	string get_objective()
	{	return m_objective;	}
	
	string get_calibrate()
	{	return m_calibrate;	}
	
	
	
	// Serial command functions.
	// These functions are for the user to send single commands to the Arduino separately from the inbuilt routines.
	bool comm_calibrate(bool out = false)
	{ 
		return serial_command(m_calibrate, "000000", out);
	}
	bool comm_is_calibrated(bool out = false)
	{ 
		return serial_command(m_is_cal, "000000", out); 
	}
	bool comm_move(int steps, bool out = false)
	{
		return serial_command(m_move, steps, out); 
	}
	bool comm_move_to(int position, bool out = false)
	{ 
		return serial_command(m_move_to, position, out); 
	}
	bool comm_get_z_len(int &length, bool out = false)
	{
		return serial_command(m_get_z_len, length, out); 
	}
	bool comm_get_z_pos(int &position, bool out = false)
	{
		return serial_command(m_get_z_pos, position, out); 
	}
	bool comm_get_dist(bool out = false)
	{
		return serial_command(m_get_z_distance, "000000", out);
	}
	bool comm_set_ring_colour(string exa_value = "000000", bool out = false)
	{
		return serial_command(m_set_ring_colour, exa_value, out);
	}
	bool comm_set_ring_bright(int value, bool out = false)
	{
		return serial_command(m_set_ring_bright, value, out); 
	}
	bool comm_set_led_bright(int value, bool out = false)
	{
		return serial_command(m_set_stage_led_bright, value, out); 
	}

	
	// See function declarations for details
	bool fine_tune() 
		{ return fine_tune(m_f_max, m_f_max_pos); }
	bool fine_tune(float f_max, int f_max_pos,
		bool previous_direction = true,
		bool up_or_down = true,
		int times_checked = 0);

	void sweep();
	
	void keep_in_focus();
	
	void test_run(int);
	
	void stop_stage(string command = "not_calibrate\n", bool couting = false);
	
};







/* ##########################################
 * #####		METHODS DECLARATION		#####
 * ########################################## */


/* Autofocus class CONSTRUCTOR 
 * Sets all variables to their values, either default or fixed 
 * WARNING: this constructor will create a directory where to save all results automatically
 * Make sure that you have the right authorizations to do so when launching the program */
void Autofocus::initialise()
{
	
	// Initialise Arduino commands
	m_calibrate = "calibrate\n";
	m_is_cal = "is_calibrated\n";
	m_get_z_len = "z_get_length\n";
	m_get_z_pos = "z_get_position\n";
	m_get_z_distance = "z_get_distance_to_go\n";
		
	m_endpoint = "0\r";
	m_OK = "OK\r";
	
	m_move_to = "z_move_to";
	m_move = "z_move";
	m_set_ring_colour = "set_ring_colour";
	m_set_ring_bright = "set_ring_brightness";
	m_set_stage_led_bright = "set_stage_led_brightness";
	
	// Initialise control variables
	m_f_max = 0;
	m_f_max_pos = 0;
	m_f_max_ind = 0;
	m_ind = 0;
	m_pos = 0;
	
	m_path = "./test/";
	m_name = "test";
	
	m_number_images_sweep = 10;
	m_number_of_times = 2;
	
	set_tolerance();
	set_output();
	set_objective();
		
	set_width();
	set_height();
	//set_path();
	//set_name();
	//set_file();
		
	// Create part of command and name strings
	set_strings();
	
	//set_serial();
	
	return; 
	
}

// Allows user input for various parameters through the SET functions,
// while using default values to start with
Autofocus::Autofocus()
		:m_io(), m_sp(m_io)
{
	
	initialise();
	
	cout << "\nFinished initialising class." << endl;
	
}	

// Takes parameters at constructor time and sets all variables accordingly
Autofocus::Autofocus(string serial_device, string objective, 
			string path, string name,
			bool leave_output,
			string width, string height)
		:m_io(), m_sp(m_io)
{
	
	// Initialise Arduino commands
	initialise();
	
	// Open serial port
	set_serial(serial_device);
		
	set_width(width);
	set_height(height);
	set_path(path);
	set_name(name);
	set_file();
		
	// Create part of command and name strings
	set_strings();
	
	set_output(leave_output);
	
	set_objective(objective);
	
	cout << "\nFinished initialising class" << endl;
			
}

/* Autofocus class DESTRUCTOR
 * Deletes the output folder if so required at the last operation executed by the program */
Autofocus::~Autofocus()
{
	m_values.close();
	
	if (!m_leave_output)
		remove_folder();

}




//################################
/* Computes focusing algorithm
 * For a picture, it computes the focusing value */
float Autofocus::algorithm() 
{
	
	float intensity = 0.0;
	float mean_intensity = 0.0;
	float intensity_squared_sum = 0.0;
	float focusing = 0.0;
	
	greyfy();
		
	// Calculate intensity of a matrix of pixels
	for (int x=0; x<m_picture.width(); x++) {
		for (int y=0; y<m_picture.height(); y++) 
		{			
			intensity += (float)(m_picture(x,y,0,0));
		}
	}
	
	mean_intensity = intensity/(float)(m_picture.width()*m_picture.height());
	
	// Make sure that no value of mean_intensity is effectively 0.0
	// to prevent division by 0, even though very unlikely
	if (mean_intensity == 0.0) mean_intensity = 1E-10;
	
	// Apply focusing algorithm of choice, in this case a normalised deviation based
	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{	
			intensity_squared_sum += ((float)m_picture(i,j,0,0) - mean_intensity)*((float)m_picture(i,j,0,0) - mean_intensity);
		}
	}
	
	focusing = intensity_squared_sum/((float)m_picture.width()*(float)m_picture.height()*mean_intensity);
	
	return focusing;
	
}




//##############################################
/* Does a rough sweep from top, with a predetermined number of images
 * and number of steps between each image.
 * It will save an image every so many steps and put the focusing value in the file opened at class construction. */
void Autofocus::sweep()
{
	
	int pos = 0;
	int step;
	bool sweep_done = false;
	
	cout << "\nExecuting sweep" << flush;
	
	while (sweep_done == false) 
	{
		
		// Saves image and automatically updates the name to the relevant index
		raspistill_save();
		
					
		// Get position of this image
		serial_command(m_get_z_pos, pos);
		
		
		// Move down
		step = -m_steps;
		serial_command(m_move, step);
		
		
		// In the meantime, analyse picture saving focus value
		// of maximum dynamically
		m_picture.assign(m_picture_input.c_str());
		
		m_f_values.push_back( algorithm() );
		m_values << m_ind << "\t" << m_f_values[m_ind] << endl;
		if (m_f_values[m_ind] >= m_f_max)
		{
			//Save position and value of maximum
			m_f_max = m_f_values[m_ind];
			m_f_max_ind = m_ind;
			m_f_max_pos = pos;
		}
		m_ind++;
		
		// Remove picture if necessary
		if (!m_leave_output)
			remove_picture();
				
		
		// Wait for the stage to finish moving
		stop_stage();
		
		
		cout << "." << flush;
		
		
		// Exit after a certain number of images		
		if (m_ind >= m_number_images_sweep)
			sweep_done = true;
		
	}
	
	cout << endl;

	return;
	
}




//#################################################
/* Makes fine corrections to the focusing point by checking above and below the starting point.
 * It can be used separately or in conjunction with the sweep function to do a complete autofocusing. */
bool Autofocus::fine_tune(float f_max, int f_max_pos,
		bool previous_direction,
		bool up_or_down,
		int times_checked)
{
		
	float f_above = 0;
	float f_below = 0;
	int f_above_pos = 0;
	int f_below_pos = 0;
		
	
	if (!previous_direction && m_steps > m_min_steps)
	{
		//cout << "\nReducing number of steps... " << endl;
		m_steps = (int)(0.5*m_steps);
	}
	else if (m_steps <= m_min_steps)
	{
		//cout << "\nUsing minimum number of steps: " << m_min_steps << endl;
		m_steps = m_min_steps;
	}
	
		
	// Take picture at starting point
	raspistill_save();
	
	
	// Compute MIDDLE picture
	f_max = move_and_capture(0, f_max_pos);
	cout << "." << flush;
	m_values << m_ind << "\t" << f_max << endl;
	m_ind++;
	
	// Set max values for future use
	m_f_max = f_max;
	m_f_max_pos = f_max_pos;
	
	// Remove picture if necessary
	if (!m_leave_output)
		remove_picture();
	
	
	//cout << "\nStart call at\n" << f_max_pos << "\t" << f_max << endl;
	
	
	
	if (up_or_down)
	// Start checking from ABOVE the starting position
	{
		
		//cout << "\nChecking UP..." << endl;
		
		f_above = move_and_capture(m_steps, f_above_pos);
		//cout << f_above_pos << "\t" << f_above << endl;
		cout << "." << flush;
		
		// Remove picture if necessary
		if (!m_leave_output)
			remove_picture();
		
		
	
		if (f_above >= ((2.0-m_precision)*f_max))
		{
			
			//cout << "\nStaying UP..." << endl;
			
			// Keep checking up, maintaining the same number of steps
			fine_tune(f_above, f_above_pos, true, true);
						
		}
		else
		{
			
			//cout << "\nChecking DOWN..." << endl;
		
			f_below = move_and_capture(-2*m_steps, f_below_pos);
			//cout << f_below_pos << "\t" << f_below << endl;
			cout << "." << flush;
			
			// Remove picture if necessary
			if (!m_leave_output)
				remove_picture();
		


			if (f_below >= ((2.0-m_precision)*f_max))
			{
				
				//cout << "\nStaying DOWN..." << endl;
				
				// Change direction of movement and continue checking down
				fine_tune(f_below, f_below_pos, false, false);
				
			}
			else
			{
				
				//cout << "\nI was already there, so I am going back... " << times_checked << endl;
				
				// Move back to maximum
				serial_command(m_move_to, f_max_pos);
				stop_stage();
				
				// If the maximum hasn't changed lately (in the last two checks) then we have arrived...
				if (times_checked == m_number_of_times && m_steps > m_min_steps)
				{
					m_steps = m_min_steps;
					fine_tune(f_max, f_max_pos, false, false, ++times_checked);
				}
				else if (times_checked < m_number_of_times && m_steps > m_min_steps)
				{
					fine_tune(f_max, f_max_pos, false, false, ++times_checked);
				}
				else
				{
					return true;
				}
				
			}
			
		}
		
	}
	
	
	
	else
	// Start checking from BELOW the starting position
	{
		
		//cout << "\nChecking DOWN..." << endl;
	
		f_below = move_and_capture(-m_steps, f_below_pos);
		//cout << f_below_pos << "\t" << f_below << endl;
		cout << "." << flush;
		
		// Remove picture if necessary
		if (!m_leave_output)
			remove_picture();
	
	
		if (f_below > ((2.0-m_precision)*f_max))
		{
			
			//cout << "\nStaying DOWN..." << endl;
			
			// Keep checking down, maintaining the same number of steps
			fine_tune(f_below, f_below_pos, true, false);			
			
		}
		else
		{
			
			//cout << "\nChecking UP..." << endl;
		
			f_above = move_and_capture(2*m_steps, f_above_pos);
			//cout << f_above_pos << "\t" << f_above << endl;
			cout << "." << flush;
		
			// Remove picture if necessary
			if (!m_leave_output)
				remove_picture();


			if (f_above > ((2.0-m_precision)*f_max))
			{
				
				//cout << "\nStaying UP..." << endl;
				
				// Invert direction and start checking above
				fine_tune(f_above, f_above_pos, false, true);
				
			}
			else
			{
				
				//cout << "\nI was already there, so I am going back... " << times_checked << endl;
				
				// Move back to maximum
				serial_command(m_move_to, f_max_pos);
				stop_stage();
				
				// If the maximum hasn't changed lately (in the last two checks) then we have arrived...
				if (times_checked == m_number_of_times && m_steps > m_min_steps)
				{
					m_steps = m_min_steps;
					fine_tune(f_max, f_max_pos, false, true, ++times_checked);
				}
				else if (times_checked < m_number_of_times && m_steps > m_min_steps)
				{
					fine_tune(f_max, f_max_pos, false, true, ++times_checked);
				}
				else
				{
					return true;
				}
				
			}
			
		}
		
	}
	
	return true;
	
}




//##################################
/* Keeps microscope in focus, and readjusts if things are changed.
 * To be used after fine_tune has completed, since it assumes the starting point is almost in focus already. */
void Autofocus::keep_in_focus()
{
	
	bool stay_in_focus = true;
	while (stay_in_focus == true)
	{
		sleep(120);
		
		m_steps = 40;
		
		fine_tune();
	}
	
	return;
	
}
	



//##############################################
/* Executes a test run.
 * This corresponds to a predetermined number of images and step separation, taken at equal distances and with values saved in the previously mentioned file. */
void Autofocus::test_run(int number_images)
{
	
	serial_command(m_get_z_len, m_whole_distance);
	cout << "\nLength of travel: " << m_whole_distance << endl;
	

	// Establish how many steps between each image, and assign to the command move	
	int number_steps = (int)-m_whole_distance/number_images;
	cout << "\nImages to be taken: " << number_images;
	cout << "\nDistance between images: " << number_steps << endl;
	
	
	cout << "\nExecuting test run" << flush;
	m_ind = 0;
	while (m_ind < number_images) 
	{			
		
		// Create string for raspistill command and for name of image
		raspistill_save();
		//cout << m_picture_input << endl;
		
		
		// Create picture to analise
		m_picture.assign(m_picture_input.c_str());


		// Move the stage to next picture position
		serial_command(m_move, number_steps);
				
		
		// Calculate focusing value for the latest picture
		m_f_values.push_back( algorithm() );
		m_values << m_ind << "\t" << m_f_values[m_ind] << endl;
		m_ind++;
		
		
		// Remove picture if necessary
		if (!m_leave_output)
			remove_picture();
		
		
		// Wait for stage to finish moving		
		stop_stage();
		
		cout << "." << flush;
		
	}
	
	string save_the_data = "mv " + m_path + "focusingdata.txt ../";
	system(save_the_data.c_str());
	
	cout << endl;
	
	return;
	
}




//################################################
/* Turns image to greyscale. */
void Autofocus::greyfy() 
{
	
	for (int x=0; x<m_picture.width(); x++) {
		for (int y=0; y<m_picture.height(); y++) 
		{
			m_picture(x,y,0,0) = 0.299*m_picture(x,y,0,0) + 0.587*m_picture(x,y,0,1) + 0.114*m_picture(x,y,0,2);
			m_picture(x,y,0,1) = 0.299*m_picture(x,y,0,0) + 0.587*m_picture(x,y,0,1) + 0.114*m_picture(x,y,0,2);
			m_picture(x,y,0,2) = 0.299*m_picture(x,y,0,0) + 0.587*m_picture(x,y,0,1) + 0.114*m_picture(x,y,0,2);
		}
	}

	return;
	
}




//###########################################
/* Saves image using RaspiStill, and returns the name of the image (with the path) to be used with imaging library.
 * The picture will be saved in the folder chosen with the default name and a progressively increasing number. */
void Autofocus::raspistill_save()
{
	
	stringstream naming;
	stringstream commanding;
	naming << m_first_part_name << m_ind << ".jpg";
	commanding << m_first_part_command << m_ind << ".jpg -t 0";
				
	string extension = commanding.str();
	const char * conversion = extension.c_str();
	
	m_picture_input = naming.str();
	
	naming.str("");
	commanding.str("");
	
	system(conversion);
	
	return;
	
}




//#########################################
/* Removes last picture saved. */
void Autofocus::remove_picture()
{
	
	string removing = "rm " + m_picture_input;
	
	system(removing.c_str());
	
	return;
	
}

/* Removes folder used by program. */
void Autofocus::remove_folder()
{
	
	string removing = "rm -r " + m_path;
	
	system(removing.c_str());
	
	return;
	
}




//###################################################
/* Moves to position, records new reached position,
 * takes picture and analyses it, returning focusing value. */
float Autofocus::move_and_capture(int position, int &f_pos)
{

	// Move ABOVE the previously established maximum
	serial_command(m_move, position);
	stop_stage();
	
	// Record position of ABOVE picture
	serial_command(m_get_z_pos, f_pos);
	m_ind++;
	
	// Take picture ABOVE previous maximum for future checking
	raspistill_save();

	// Compute ABOVE picture
	m_picture.assign(m_picture_input.c_str());
	float f_value = algorithm();
		
	return f_value;
		
}




//########################################################################################
/* Takes the last command sent to move as an input, and decides course of action depending on whether
 * the command is 'calibrate' or not. */
void Autofocus::stop_stage(string command, bool couting)
{
	
	if (command.compare(m_calibrate) == 0)
	{
		istream input(&m_buffering);
		char output[256];
		memset(output, '\0', sizeof output);
		boost::asio::read_until(m_sp, m_buffering, "OK\r\n");
		input.readsome(output, sizeof output);
		if (couting)
			cout << output << endl;
		cout << "\nCalibration completed" << endl;
		if (couting)
			cout << endl;
	}
	else
	{
		bool control = false;
		while (control == false) 
		{
			control = serial_command(m_get_z_distance, "000000", couting);
			if (couting)
				cout << endl;
			if (control == false)
				sleep(1);
		}
	}
	
	return;
}



//###################################################################
/* Overloaded function that takes commands and sends them to the serial port */

// Version that takes a string as 'number' for argument of command
// Used for setting LED ring to certain color
// String of argument should be a triple of exadecimal values for RGB, like FF0000 for full red.
// This is left as a separate function for future applications that require string inputs.
bool Autofocus::serial_command(string command, string argument, bool couting, string check) 
{	
	stringstream ss;
	istream is(&m_buffering);
	string line;
	bool control = false;
	bool exiting = false;
	
	if (command.compare(m_calibrate) == 0)
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		return true;
	}
	else if (command.compare(m_is_cal) == 0)
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (atoi(line.c_str()) != 0)
				control = true;
			if (line.compare(check) == 0)
				exiting = true;
		}
	}
	else if (command.compare(m_get_z_distance) == 0)
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		check = m_endpoint;
		for (int i=0; i<3; i++) 
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (line.compare(check) == 0)
				control = true;
			
			line.clear();
		}
	}
	// Requires string as argument, since it must be exadecimal
	else if (command.compare(m_set_ring_colour) == 0)
	{
		ss << command << " " << argument << "\n";
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (atoi(line.c_str()) != 0)
				control = true;
			else
				control = false;
			if (line.compare(check) == 0)
				exiting = true;
		}
	}
	else 
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (line.compare(check) == 0)
			{
				exiting = true;
				control = true;
			}
			
			if (line.compare("ERR: UNKNOWN COMMAND\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: NOT CALIBRATED\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: POSITION OUT OF RANGE\r") == 0)
			{
				exiting = true;
			}
				
			line.clear();
		}
	}
		
	return control;	
}

// Version that takes a number as integer passed by reference.
// Number can be used both as input argument or as 'storage space' for an ouput depending on the command sent.
// I.e. z_get_length will save the length of travel in number of steps into the number provided.
bool Autofocus::serial_command(string command, int &number, bool couting, string check)
{
	stringstream ss;
	istream is(&m_buffering);
	string line;
	bool control = false;
	bool exiting = false;
	
		
	if (command.compare(m_calibrate) == 0)
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		return true;
	}
	else if (command.compare(m_is_cal) == 0)
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (atoi(line.c_str()) != 0)
				control = true;
			if (line.compare(check) == 0)
				exiting = true;
				
			line.clear();
		}
	}
	else if (command.compare(m_get_z_distance) == 0)
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		check = m_endpoint;
		
		for (int i=0; i<3; i++) 
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (line.compare(check) == 0)
				control = true;
			
			line.clear();
		}
	}
	// Requires unsigned integer as argument
	else if (command.compare(m_move_to) == 0)
	{
		string number_in_words = boost::lexical_cast<string>((int)(number));
		ss << command << " " << number_in_words << "\n";
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (line.compare(check) == 0)
			{
				exiting = true;
				control = true;
			}
			
			if (line.compare("ERR: UNKNOWN COMMAND\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: NOT CALIBRATED\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: POSITION OUT OF RANGE\r") == 0)
			{
				exiting = true;
			}
				
			line.clear();
		}
	}
	// Requires signed integer as argument
	else if (command.compare(m_move) == 0)
	{
		string number_in_words = boost::lexical_cast<string>((int)(number));
		ss << command << " " << number_in_words << "\n";
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (line.compare(check) == 0)
			{
				exiting = true;
				control = true;
			}
			
			if (line.compare("ERR: UNKNOWN COMMAND\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: NOT CALIBRATED\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: POSITION OUT OF RANGE\r") == 0)
			{
				exiting = true;
			}
				
			line.clear();
		}
	}
	// Requires a number between 0 and 255 as argument
	else if (command.compare(m_set_stage_led_bright) == 0 || command.compare(m_set_ring_bright) == 0)
	{
		if (number < 0 || number > 255)
			number = 70;
		
		string number_in_words = boost::lexical_cast<string>((int)(number));
		ss << command << " " << number_in_words << "\n";
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (line.compare(check) == 0)
			{
				exiting = true;
				control = true;
			}
			
			if (line.compare("ERR: UNKNOWN COMMAND\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: NOT CALIBRATED\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: POSITION OUT OF RANGE\r") == 0)
			{
				exiting = true;
			}
				
			line.clear();
		}
	}
	else 
	{
		ss << command;
		write(m_sp, buffer(ss.str()));
		ss.str("");
		
		while (exiting == false)
		{
			boost::asio::read_until(m_sp, m_buffering, '\n');
			getline(is, line);
			if (couting)
				cout << line << endl;
			if (atoi(line.c_str()) != 0)
				number = atoi(line.c_str());
			if (line.compare(check) == 0)
			{
				exiting = true;
				control = true;
			}
			
			if (line.compare("ERR: UNKNOWN COMMAND\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: NOT CALIBRATED\r") == 0)
			{
				exiting = true;
			}
			if (line.compare("ERR: POSITION OUT OF RANGE\r") == 0)
			{
				exiting = true;
			}
				
			line.clear();
		}
	}
		
	return control;	
}


#endif
