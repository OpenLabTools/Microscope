// Edge Detection Program

/* This program is designed to run the edgedetection class contained in edgedetection_class.h.
 * It provides a basic user interface through terminal to allow opening of the image file to be used.
 * For more information, see descrioption of edgedetection_class.h
 */



#include "edgedetection_class.h"


int main () {
	
	stringstream ss;
	string name;
	
	//int n=10;
	
	// Be sure that this is the path where you have saved your images!
	// Otherwise, write your own
	ss << "../images/";
	cout << "\nType name of picture (in .jpg format) that you wish to analyse: ";
	cin >> name;
	if (name[0] == '/')
	{
		ss.str("");
		ss << name << ".jpg";
	}
	else
	{
		ss << name << ".jpg"; 
	}
	
	string extension = ss.str();
	const char * path = extension.c_str();
	cout << "\nOpening " << path << endl;
	
	Edgedetection detecting(path);

	detecting.canny_edge_detection();
	
	return 0;

}
