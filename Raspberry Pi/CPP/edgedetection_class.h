// Edge Detection Class

/* This file contains the class that contains all variables and methods necessary to implement the Canny edge detection algorithm.
 * These are:
 * 
 * public:
 * Edgedetection(string) -> Only class constructor.
 * 		Uses string as location of the file to be opened.
 * 		Full path should be provided, and it's the main program's job to make sure that the full path is passed to the constructor.
 * ~Edgedetection() -> Class destructor.
 * 		Deallocates all memory allocated dinamically during the construction and execution of methods in the class.
 * canny_edge_detection() -> Executes the algorithm of edge detection on the image at the path used in the constructor.
 * 		It will display all changes made on the image by the subsequent methods.
 * 
 * private:
 * greyfy() -> Turns an image into greyscale.
 * 		CAUTION! Because of the way this is implemented, this program will work only on images that are NOT previously in greyscale.
 * load_matrix() -> Loads a noise-damping convolution matrix from file.
 * 		Read comments on methods to see all details on how this is done.
 * picture_convolution() -> Convolves the image with the previously loaded matrix to dampen the effect of artifacts on the end result.
 * simple_gradient() -> Computes the gradient associated with each pixel using a simple method.
 * 		The choice of this method or the next is totally arbitrary.
 * 		They are both provided just as different options.
 * sobel_gradient() -> Computes the gradient associated with each pixel using the Sobel algorithm.
 * 		This is a more complex method than the previous one, and it is supposed to be more precise.
 * 		Whether this is true or not can be tested by the user at his/her leisure.
 * 		By default, however, this is the gradient algorithm preferred.
 * edge_decision(float, float) -> This method takes a high and low threshold as a multiplier of the average modulus of the gradients of the picture.
 * 		It will then divide the pixels in three cathegories depending on the modulus of their gradient:
 * 		above high threshold -> true edge
 * 		between high and low threshold -> secondary edge
 * 		below low threshold -> not an edge
 * edge_selection() -> This method decides whether secondary edges are true edges or not depending on various criteria, mostly dependent on their proximity to a true edge.
 * crop_and_save() -> Removes the border of the image, necessarely turned black by the previous method (read descriptions) and saves the result image to file.
 */



#ifndef EDGEDETECTION_CLASS_H
#define EDGEDETECTION_CLASS_H

#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "CImg.h"

#define PI 3.14159265359

using namespace cimg_library;
using namespace std;
using namespace boost::asio;


class Edgedetection
{
	
private:

	CImg<float> m_picture;
	CImgDisplay m_show;
	
	float ** m_matrix;
	float *** m_gradient;
	int ** m_edge;
	int m_size_matrix;
	
	char m_input_multipliers;
	char m_save;


	void edge_decision(float high_thr_mult = 3, float low_thr_mult = 1.2);
	
	void edge_selection();
	
	void simple_gradient();
	
	void sobel_gradient();
	
	void picture_convolution();
	
	float sum_matrix(float **, int, int);
	
	void crop_and_save();
	
	void greyfy();
	
	void load_matrix();
	

public:

	Edgedetection(string);
	
	~Edgedetection();
	
	CImg<float> canny_edge_detection();


};




/* Edgedetection class CONSTRUCTOR 
 * It contains all interface necessary for the correct execution of the algorithm
 * Note that the matrix size for noise damping is pre-established here, so the matrix loaded in canny_edge_detection() method must be of same size!
 * We provide one such matrix in a file available in the 'Edge Detection' folder
 * Make sure the matrix is in the same folder as the executable file at runtime */
Edgedetection::Edgedetection(string picture_location)
{
	
	m_picture.assign(picture_location.c_str());
	
	m_size_matrix = 5;
	
	cout << "\nDo you wish to save the image after processing (y/n)? ";
	cin >> m_save;
	
	cout << "\nWould you like to input your own factors (y/n)? ";
	cout << "\nNOTE: If 'n' is selected, default ones will be used... "; 
	cin >> m_input_multipliers;
	
	// Create 3D matrix of gradient values
	m_gradient = new float**[m_picture.width()];
		for (int x=0; x<m_picture.width(); x++) 
		{
			m_gradient[x] = new float*[m_picture.height()];
			for (int y=0; y<m_picture.height(); y++) 
			{
				m_gradient[x][y] = new float[4];
			}
		}
	
	// Create 2D matrix of edge values (for selection)	
	m_edge = new int*[m_picture.width()];
		for (int x=0; x<m_picture.width(); x++) 
		{
			m_edge[x] = new int[m_picture.height()];
		}
	
	// Create 2D noise damping convolution matrix	
	m_matrix = new float*[m_size_matrix];
		for (int x=0; x<m_size_matrix; x++) 
		{
			m_matrix[x] = new float[m_size_matrix];
		}
		
}	





/* Edgedetection class DESTRUCTOR 
 * Deallocates all matrices allocated dinamically */
Edgedetection::~Edgedetection()
{
	
	for (unsigned int i=0; i<sizeof m_gradient; i++)
	{
		for (unsigned int j=0; j<sizeof m_gradient[0]; j++)
		{
			delete[] m_gradient[i][j];
		}
		delete[] m_gradient[i];
	}
	delete[] m_gradient;
	
	for (unsigned int i=0; i<sizeof m_edge; i++)
	{
		delete[] m_edge[i];
	}
	delete[] m_edge;
	
	for (unsigned int i=0; i<sizeof m_matrix; i++)
	{
		delete[] m_matrix[i];
	}
	delete[] m_matrix;
	
}




/* Edge detection algorithm for images. */
CImg<float> Edgedetection::canny_edge_detection() 
{
	
	// First we show the picture we start with
	m_show.display(m_picture);
	

	// Load convolution matrix for noise damping
	// This matrix will be convolved with the image to smooth away artifacts and 'ruined' pixels
	load_matrix();
	cout << "\nFinished loading convolution matrix for noise filtering" << endl;
	
	
	
	// Reduce image to greyscale (intensity scale)
	// This is necessary for the following analysis, since we are interested in the intensity of the pixels and not their colour
	//if ((m_picture(0,0,0,0) != m_picture(0,0,0,1)) || (m_picture(0,0,0,0) != m_picture(0,0,0,2))) 
	//{
		greyfy();
		cout << "\nFinished turning image to greyscale" << endl;
	//}
	//else
	//	cout << "\nImage is already in greyscale" << endl;
	
	// Display the image after turning to grayscale
	m_show.display(m_picture);
	
	
	
	// Apply convolution with preset matrix
	picture_convolution();
	
	m_show.display(m_picture);
		
	cout << "\nFinished noise filtering" << endl;
	
	
	
	// Calculate gradients associated with each pixel
	// Refer to the 'Canny Edge Detector' article on Wikipedia to know why this is necessary
	// Here we have implemented two possibilities for the gradient calculation
	// See the specific methods within the class for further details
	//simple_gradient();
	sobel_gradient();
	
	cout << "\nFinished calculating gradient for the pixels" << endl;
	

	
	// Decide whether each pixel is on the edge or not
	// This is done following the Canny Edge Detection method of minor and major edges
	// See method for details
	edge_decision();
	
	// Assign different grey tonalities to major edges, minor edges and non-edges respectively
	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{
			
			if (m_edge[i][j] == 2) 
			{
				m_picture(i,j,0,0) = 255;
				m_picture(i,j,0,1) = 255;
				m_picture(i,j,0,2) = 255;
			}
			else if (m_edge[i][j] == 1) 
			{
				m_picture(i,j,0,0) = 130;
				m_picture(i,j,0,1) = 130;
				m_picture(i,j,0,2) = 130;
			}	
			else 
			{
				m_picture(i,j,0,0) = 0;
				m_picture(i,j,0,1) = 0;
				m_picture(i,j,0,2) = 0;
			}
			
		}
	}
	
	// Display edges
	m_show.display(m_picture);
	
	cout << "\nFinished establishing edges" << endl;
	
	m_show.wait(2000);
	
	
	
	// Select edges that are REAL edges
	// This is done by checking proximity of minor edges with major edges, or by continuing major edges into minor edges if they are reciprocal continuation
	// See method for details
	edge_selection();
	
	// Assign 'white' to real edges, and black to non-edges
	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{
			
			if (m_edge[i][j] == 2) 
			{
				m_picture(i,j,0,0) = 255;
				m_picture(i,j,0,1) = 255;
				m_picture(i,j,0,2) = 255;
			}
			else 
			{
				m_picture(i,j,0,0) = 0;
				m_picture(i,j,0,1) = 0;
				m_picture(i,j,0,2) = 0;
			}
			
		}
	}
	
	// Display final result
	m_show.display(m_picture);
	
	cout << "\nFinished selecting edges" << endl << endl;
	
	// Save final result if so required by the user, cropping away the black fringes that result from the edge detection work
	if (m_save == 'y')
	{
		crop_and_save();
		m_show.wait(2000);
	}
	else
		m_show.wait();
	
	// Return the picture result, in case one may want to do more with it (we don't here, but you never know)
	return m_picture;

}




/* Decide whether each pixel is on an edge or not depending on gradient
 * Two thresholds are established either by the user or by default (see CONSTRUCTOR)
 * All pixels with gradient magnitude greater than the high threshold are selected as major edges
 * All pixels with gradient magnitude between the high and the low threshold are selected as minor edges
 * All other pixels are non-edges
 * Note that pixels around the borders of the picture need to be selected as non-edges because they don't have neighbours to calculate the gradient from
 * The gradient values are pre-established externally from this method, by the invocation of one of the *_gradient methods */
void Edgedetection::edge_decision(float high_thr_mult, float low_thr_mult) 
{
	
	float intermediate = 0;
	for (int i=3; i<m_picture.width()-3; i++) {
		for (int j=3; j<m_picture.height()-3; j++) 
		{
			intermediate += m_gradient[i][j][2];
		}
	}
	float average = (float)intermediate/(float)(m_picture.width()*m_picture.height());
	//cout << intermediate << endl;
	
	// Establish thresholds of edge detection from average of gradients
	if (m_input_multipliers == 'y')
	{
		m_show.close();
		cout << "\n\tAverage of gradient: " << average;
		cout << "\n\tThe following will be multiplied by the average of the gradient.";
		cout << "\n\tInsert high threshold multiplier : ";
		cin >> high_thr_mult;
		cout << "\tInsert low threshold multiplier : ";
		cin >> low_thr_mult;
		m_show.show();
	}
	float high_thr = average * high_thr_mult;
	float low_thr = average * low_thr_mult;
	
	// Establish primary and secondary edges
	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{
			
			if (i==0 || i==1 || i==2 || i==m_picture.width()-2 || i==m_picture.width()-1 || i==m_picture.width())//|| i==3 || i==m_picture.width()-3 || i==m_picture.width()-2 || i==m_picture.width()-1 || i==m_picture.width()) 
				m_edge[i][j] = 0;
			else if (j==0 || j==1 || j==2 || j==m_picture.height()-2 || j==m_picture.height()-1 || j==m_picture.height())//|| j==3 || j==m_picture.height()-3 || j==m_picture.height()-2 || j==m_picture.height()-1 || j==m_picture.height()) 
				m_edge[i][j] = 0;
			else if (m_gradient[i][j][3] == 0 && m_gradient[i-1][j][2] < m_gradient[i][j][2] && m_gradient[i+1][j][2] < m_gradient[i][j][2]) 
			{
				if (m_gradient[i][j][2] > high_thr) 			
					m_edge[i][j] = 2;
				else if (m_gradient[i][j][2] > low_thr)
					m_edge[i][j] = 1;
				else
					m_edge[i][j] = 0;
			}
			else if (m_gradient[i][j][3] == 45 && m_gradient[i+1][j+1][2] < m_gradient[i][j][2] && m_gradient[i-1][j-1][2] < m_gradient[i][j][2]) 
			{
				if (m_gradient[i][j][2] > high_thr) 			
					m_edge[i][j] = 2;
				else if (m_gradient[i][j][2] > low_thr)
					m_edge[i][j] = 1;
				else
					m_edge[i][j] = 0;
			}
			else if (m_gradient[i][j][3] == 90 && m_gradient[i][j-1][2] < m_gradient[i][j][2] && m_gradient[i][j+1][2] < m_gradient[i][j][2]) 
			{
				if (m_gradient[i][j][2] > high_thr) 			
					m_edge[i][j] = 2;
				else if (m_gradient[i][j][2] > low_thr)
					m_edge[i][j] = 1;
				else
					m_edge[i][j] = 0;
			}
			else if (m_gradient[i][j][3] == 135 && m_gradient[i-1][j+1][2] < m_gradient[i][j][2] && m_gradient[i+1][j-1][2] < m_gradient[i][j][2]) 
			{
				if (m_gradient[i][j][2] > high_thr) 			
					m_edge[i][j] = 2;
				else if (m_gradient[i][j][2] > low_thr)
					m_edge[i][j] = 1;
				else
					m_edge[i][j] = 0;
			}
			else 
				m_edge[i][j] = 0;			
			
		}
	}
	
	return;
	
}




/* Establish whether minor edges should be part of major or not
 * The picture is swept first left to right to check whether minor edges are continuation or close neighbours of major edges
 * Then the picture is swept right to left to make sure that nothing is left unchecked (edges can go in both directions!)
 * This is necessary because if a minor edge connects to a major edge at its rightmost point, it will be ignored in the first sweep but not in the second */
void Edgedetection::edge_selection() 
{
	
	// Swipe through secondary edges first time to assign them to primary or not
	for (int i=2; i<m_picture.width()-2; i++) {
		for (int j=2; j<m_picture.height()-2; j++) 
		{
			
			if (m_edge[i][j] == 1) 
			{
				for (int x=-2; x<3; x++) {
					for (int y=-2; y<3; y++) 
					{
						if (m_edge[i-x][j-y] == 2) 
							m_edge[i][j] = 2;
					}
				}
			}
			
		}
	}

	// Swipe through secondary edges in reverse order to check for possible updates from previous swipe
	for (int i=m_picture.width()-2; i>2; i--) {
		for (int j=m_picture.height()-2; j>2; j--) 
		{
			
			if (m_edge[i][j] == 1) 
			{
				for (int x=-2; x<3; x++) {
					for (int y=-2; y<3; y++) 
					{
						if (m_edge[i-x][j-y] == 2) 
							m_edge[i][j] = 2;
					}
				}
			}
			
		}
	}
	
	// Cleanup
	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{			
			if (m_edge[i][j] == 1) 
				m_edge[i][j] = 0;
		}
	}
	
	return;
	
}




/* Compute the gradient of the pixels in an image in a rather straightforward way
 * See Wikipedia articles on gradient-based edge detections for details on the mathematics of calculating a gradient of the pixels in an image
 * Note that gradients also have a direction, which at the end of this method is simplified into four possible angles since pixels can only change horizontally, vertically or diagonally */
void Edgedetection::simple_gradient() 
{

	float temporary;

	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{
			
			if (i==0 || i==(m_picture.width()-1)) 
			{
				m_gradient[i][j][0] = 0;
				m_gradient[i][j][1] = 0;
			}
			else if (j==0 || j==(m_picture.height()-1)) 
			{
				m_gradient[i][j][0] = 0;
				m_gradient[i][j][1] = 0;
			}
			else { 	
				m_gradient[i][j][0] = - 0.5*m_picture(i-1,j,0,0) + 0.5*m_picture(i+1,j,0,0);
				m_gradient[i][j][1] = - 0.5*m_picture(i,j-1,0,0) + 0.5*m_picture(i,j+1,0,0);
				m_gradient[i][j][2] = sqrt( pow(m_gradient[i][j][0],2) + pow(m_gradient[i][j][1],2) );
				
				temporary = abs(atan2(m_gradient[i][j][1],m_gradient[i][j][0]));
				if (temporary < PI/8.0 || temporary >= 7.0*PI/8.0) 			m_gradient[i][j][3] = 0;
				else if (PI/8.0 <= temporary && temporary < 3.0*PI/8.0) 		m_gradient[i][j][3] = 45;
				else if (3.0*PI/8.0 <= temporary && temporary < 5.0*PI/8.0) 	m_gradient[i][j][3] = 90;
				else if (5.0*PI/8.0 <= temporary && temporary < 7.0*PI/8.0) 	m_gradient[i][j][3] = 135;
				else m_gradient[i][j][3] = 0;
			}
			
			//cout << m_gradient[i][j][0] << " " << m_gradient[i][j][1] << " " << m_gradient[i][j][2];
			
		}
		
		//cout << endl;
	}
	
	return;
	
}




/* Compute the gradient of pixels in an image using the Sobel algorithm
 * See Wikipedia article on Sobel gradient for edge detection for details
 * Note that gradients also have a direction, which at the end of this method is simplified into four possible angles since pixels can only change horizontally, vertically or diagonally */
void Edgedetection::sobel_gradient() 
{

	float temporary;
	
	for (int i=0; i<m_picture.width(); i++) {
		for (int j=0; j<m_picture.height(); j++) 
		{
			if (i==0 || i==(m_picture.width()-1)) 
			{
				m_gradient[i][j][0] = 0;
				m_gradient[i][j][1] = 0;
			}
			else if (j==0 || j==(m_picture.height()-1)) 
			{
				m_gradient[i][j][0] = 0;
				m_gradient[i][j][1] = 0;
			}
			else 
			{ 	
				m_gradient[i][j][0] = -m_picture(i-1,j-1,0,0) -2*m_picture(i-1,j,0,0) -m_picture(i-1,j+1,0,0) +m_picture(i+1,j-1,0,0) +2*m_picture(i+1,j,0,0) +m_picture(i+1,j+1,0,0);
				m_gradient[i][j][1] = +m_picture(i-1,j-1,0,0) +2*m_picture(i,j-1,0,0) +m_picture(i+1,j-1,0,0) -m_picture(i-1,j+1,0,0) -2*m_picture(i,j+1,0,0) -m_picture(i+1,j+1,0,0);
				m_gradient[i][j][2] = sqrt( pow(m_gradient[i][j][0],2) + pow(m_gradient[i][j][1],2) );
				//cout << m_gradient[i][j][0] << m_gradient[i][j][1] << m_gradient[i][j][2] << " ";
			}

			temporary = abs(atan2(m_gradient[i][j][1],m_gradient[i][j][0]));
			if (temporary < PI/8.0 || temporary >= 7.0*PI/8.0) 			m_gradient[i][j][3] = 0;
			else if (PI/8.0 <= temporary && temporary < 3.0*PI/8.0) 		m_gradient[i][j][3] = 45;
			else if (3.0*PI/8.0 <= temporary && temporary < 5.0*PI/8.0) 	m_gradient[i][j][3] = 90;
			else if (5.0*PI/8.0 <= temporary && temporary < 7.0*PI/8.0) 	m_gradient[i][j][3] = 135;
			else m_gradient[i][j][3] = 0;
					
		}
		//cout << endl;
	}
	
	return;
	
}




/* Convolves an image with the matrix given
 * WARNING! It will assume that the size of the side of the matrix loaded is of ODD length
 * Otherwise, it goes boom 
 * If you don't know what matrix convolution in terms of computational mathematics is, well, Google it */
void Edgedetection::picture_convolution() 
{
	
	CImg<float> convolution(m_picture.width(),m_picture.height(),1,3);
	int limit = (m_size_matrix - 1)/2;
	float ** temp = new float * [m_size_matrix];
		for (int x=0; x<m_size_matrix; x++) 
		{
			temp[x] = new float[m_size_matrix];
		}
	
	for (int i=limit; i<(m_picture.width()-limit); i++) {
		for (int j=limit; j<(m_picture.height()-limit); j++) 
		{
			
			for (int x=0; x<m_size_matrix; x++) {
				for (int y=0; y<m_size_matrix; y++) 
				{
					temp[x][y] = m_matrix[x][y]*m_picture(i-limit+x,j-limit+y,0,0);
				}
			}
				
			convolution(i,j,0,0) = sum_matrix(temp, m_size_matrix, m_size_matrix);
			convolution(i,j,0,1) = convolution(i,j,0,0);
			convolution(i,j,0,2) = convolution(i,j,0,0);
		
		}
	}

	m_picture.assign(convolution);
	
	for (int i=0; i<m_size_matrix; i++)
	{
		delete[] temp[i];
	}
	delete[] temp;

	return;
	
}




/* Sum all values of a square matrix of prefixed size */
float Edgedetection::sum_matrix(float ** matrix, int width, int height)
{
	
	float total = 0;

	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) 
		{			
			total += matrix[i][j];
		}
	}

	return total;

}




/* Turn image to grayscale
 * Check RGB to greyscale conversion on the internet
 * The prefactors for each RGB value come from the colour intensity form due to eye perception 
 * There are lots of papers on the subject, if you are interested 
 * Good luck finding them */
void Edgedetection::greyfy() 
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




/* Load noise convolution matrix 
 * WARNING! The matrix is loaded from the same folder as the executalbe file, and it has a predefined name
 * Make sure that all of these parameters match! */
void Edgedetection::load_matrix()
{
	
	float factor = 0;
	float element = 0;
	
	ifstream reading;
	
	string location = "./matrix_size5.txt";
	reading.open(location.c_str());
	reading >> factor;
	for (int i=0; i<m_size_matrix; i++) {
		for (int j=0; j<m_size_matrix; j++) 
		{
			reading >> element;
			m_matrix[i][j] = factor * element;
		}
	}
	
	reading.close();
	
	return;
	
}



/* Crop and save image, removing black edges 
 * In the edge_selection method we created a black rim around the images because some pixels don't have neighbours to calculate the gradient from
 * Now, since it looks bad, we remove it */
void Edgedetection::crop_and_save()
{
	
	CImg<float> cropped(m_picture.width()-6, m_picture.height()-6, 1, 3);
	
	for (int x=0; x<m_picture.width()-6; x++) {
		for (int y=0; y<m_picture.height()-6; y++)
		{
			cropped(x,y,0,0) = m_picture(x+3,y+3,0,0);
			cropped(x,y,0,1) = m_picture(x+3,y+3,0,1);
			cropped(x,y,0,2) = m_picture(x+3,y+3,0,2);
		}
	}
	
	cropped.save("./result_of_edge_detection.jpg");
	
	return;

}




#endif
