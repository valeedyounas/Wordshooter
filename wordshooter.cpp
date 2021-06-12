//============================================================================
// Name        : wordshooter.cpp
// Author      : Muhammad Valeed
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Word Shooter...
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

//#include <GL/gl.h>
//#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include "util.h"
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 10

string * dictionary;
const int dictionarysize = 370099;
#define KEY_ESC 27 // A

// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;
float timeL=120;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;


enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...

//my functions
float ball_distanceCalc(int x1,int x2 , int y1, int y2)
{
	float distance;
	int x1x2=x1-x2;
	int x1x2_2= pow(x1x2,2);
	int y1y2=y1-y2;
	int y1y2_2=pow(y1y2,2);

	distance=sqrt(x1x2_2 + y1y2_2);
	return distance;

}

//my variables
char alpha[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
string what_to_burst;
int store_j[9]={-1,-1,-1,-1,-1,-1,-1,-1,-1};
//string combinations_Alpha_ingame[10][15][40];
//const int combosize=1000;int Acombosize=0; string combinations_Alpha_ingame[combosize];
char alpha_twoD[10][15];
int store_i[9]={-1,-1,-1,-1,-1,-1,-1,-1,-1};
string realdictionary[350000];
int realdictionary_size=0;
int wordcount_store[dictionarysize];
int twoD[10][15];
int twoD_yx[10][15][2];
const float xcell = 31;
const int ycell = 22;
int sh_Ballx=460;
int sh_Bally=10;
int new_ball;
bool mouse_clicked=0;
bool mouse_clicked2=0;
bool mouse_clicked3=0;
int scanCheck=0;
int moving_ballC;
float moving_ballx=460,moving_bally=10;
float change_x,change_y,slope,c_intercept;
int pixelx,pixely,cellx,celly;
int wordcount;
int clickx;
//bool angle_mouseClick=0;
int count_y=0,count_x=0,count_cellx,count_celly;
float ball_d;
int scanning_count=0;
int temp;
bool gamestop=0;

//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, float sx, float sy, int cwidth = 60,int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}

int GetAlphabet()
{
	return GetRandInRange(0, 25);
}

void Pixels2Cell(int px, int py, int & cx, int &cy)
{
	cx=px/30;
	cy=py/30;
}


void Pixels2Cell_b(int px, int py, int & cx, int & cy)
{
	cx=(px-10)/60;
	cx=round(cx);
	py=height-py;
	cy=(py-120)/60;
	cy=round(cy);

}


void Cell2Pixels(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
	px=cx*30;
	py=cy*30;
}

void Cell2Pixels_b(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
	//vertica; cell starts from 1
	py=cy*60;
	py=(height-120)-py;
	//Horizontal cell starts from 0
	px=cx*(60)+10;
}

void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
/*
* Main Canvas drawing function.
* */

void DisplayFunction()
// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//if 0
{

	glClearColor(1/*Red Component*/, 1.0/*Green Component*/,
		1.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors

	//write your drawing commands here or call your drawing functions...


	if (timeL>0)
	{
			for(int i = 0, y = height -120; i < nycells; ++i, y -= 60)   //Loop draws alphabets on screen
			{
				for(int j = 0, x = 10; j < nxcells; ++j, x += 60)
				{

					if(twoD[i][j] != -1)
					{
						DrawAlphabet((alphabets)twoD[i][j], x, y, awidth, aheight);
					}
				}
			}

	}

	//DrawAlphabet((alphabets)new_ball, sh_Ballx, sh_Bally, awidth, aheight);  //Draws alphabet on shooting area
	//if (moving_ballx>40)
	DrawAlphabet((alphabets)moving_ballC, moving_ballx, moving_bally, awidth, aheight); //draws moving shoot ball
	//else
		//DrawAlphabet((alphabets)moving_ballC, mvbx, moving_bally, awidth, aheight);
	if (mouse_clicked2==0 && mouse_clicked3!=0 && gamestop==0)
		DrawString(width/2-50, height/2, width, height + 5, "Now Loading..." , colors[BLACK]);
	if (mouse_clicked3==0)
		DrawString(width/2 - 50, height/2, width, height + 5, "Please wait..." , colors[BLACK]);

	if (timeL<=0 || gamestop==1)
		DrawString(width/2-50, height/2, width, height + 5, "GAME OVER!", colors[BLUE_VIOLET]);
	DrawString(40, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]); //Draws score
	if (timeL>0)
	{
		if (gamestop==0)
			DrawString(width / 2 - 30, height - 25, width, height,"Time Left:" + Num2Str(timeL) + " secs", colors[RED]);
	}


	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height)
{
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...

	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {

	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y)
{
	cout<< "X = " << x <<" Y = " << height - y << endl;
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls

}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */
void MouseClicked(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
		cout<< "Mouse clicked\tX = " << x <<" Y = " << height - y << endl;
	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_UP)
		{

			//if (x>460 || x>470 || x>480 || x>490)
				//angle_mouseClick=0;
			//else if (x<=460 || x<450 || x<440)
				//angle_mouseClick=1;
			y = height-y;
			clickx=x;
			x-=30;
			y-=30;
			moving_ballx=460,moving_bally=10;
			temp=moving_ballC;
			moving_ballC=temp;
			//moving_ballC=new_ball;
			new_ball=GetRandInRange(0, 25);
			mouse_clicked=1;
			//mouse_clicked3=1;
			change_y= y - sh_Bally;
			//rightwall=0;
			change_x= x - sh_Ballx;
			slope = change_y/change_x;
			c_intercept=10-(460*slope);

			ball_d=ball_distanceCalc(moving_ballx,twoD_yx[9][14][0],moving_bally,twoD_yx[9][14][1]);

			Pixels2Cell_b(x,y,cellx,celly);
			cout<<cellx<<" "<<celly<<endl;

		}

	}
	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{

	}
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y)
{
	if (key == KEY_ESC/* Escape key ASCII*/)
	{
		exit(1); // exit the program when escape key is pressed.
	}
}


/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */


void Timer(int m=0)
{
	if (timeL>0)
	{
		for (int j=0;j<15;++j)
		{
			if (twoD[8][j]!=-1)
			{
				gamestop=1;
				break;
			}
		}
		if (gamestop==0)
		{
			timeL=timeL- (0.014);
			glutPostRedisplay();

			if (mouse_clicked==1)
			{
				Pixels2Cell_b(moving_ballx,moving_bally,cellx,celly);
				if (ball_d>60)
				{


					if (moving_bally>=height-120)
					{
						Pixels2Cell_b(moving_ballx,moving_bally,cellx,celly);
						twoD[0][cellx]=moving_ballC;
						Pixels2Cell_b(moving_ballx,moving_bally,cellx,celly);
						moving_ballx=460;
						moving_bally=10;
						moving_ballC=new_ball;
						mouse_clicked=0;
						mouse_clicked2=0;
					}
					else
						{
							if (moving_ballx<850)
							{
								if (moving_ballx<=10)
									slope*=-1;
								moving_bally+=10;
								moving_ballx=(moving_bally-c_intercept)/slope;
							}
							else
							{
								slope*=-1;
								//slope*=1.5;
								moving_bally+=10;
								moving_ballx+=(slope);
							}
						}


					for(int i = 0; i < nycells; ++i)
							{
								for(int j = 0; j < nxcells; ++j)
								{
									if (twoD[i][j]!=-1)
									{
										for (int k=0;k<2;++k)
										{
											if (k==0)
												count_x=twoD_yx[i][j][k];
											else
												count_y=twoD_yx[i][j][k];
										}

										ball_d=ball_distanceCalc(moving_ballx,count_x,moving_bally,count_y);

										if (ball_d<=60)
											break;
									}
								}
								if (ball_d<=60)
									break;
							}
				}
				else
				{
					int min=1000000000;
					for(int i = 0; i < nycells; ++i)   //Loop draws alphabets on screen
										{
											for(int j = 0; j < nxcells; ++j)
											{
												if (twoD[i][j]!=-1)
												{
													for (int k=0;k<2;++k)
													{
														if (k==0)
															count_x=twoD_yx[i][j][k];
														else
															count_y=twoD_yx[i][j][k];
													}
													ball_d=ball_distanceCalc(moving_ballx,count_x,moving_bally,count_y);
													if (ball_d<min)
													{
														min=ball_d;
														count_cellx=j;
														count_celly=i;
													}
												}
											}
										}

					if (twoD[count_celly+1][count_cellx]==-1)
					{
						twoD[count_celly+1][count_cellx]=moving_ballC;
						alpha_twoD[count_celly+1][count_cellx]=alpha[moving_ballC];
					}

					else
					{
						if (slope>0)
						{
							twoD[count_celly][count_cellx-1]=moving_ballC;
							alpha_twoD[count_celly][count_cellx-1]=alpha[moving_ballC];
						}
						else
						{
							twoD[count_celly][count_cellx+1]=moving_ballC;
							alpha_twoD[count_celly][count_cellx+1]=alpha[moving_ballC];
						}
					}

					moving_ballx=460;
					moving_bally=10;
					moving_ballC=new_ball;
					mouse_clicked=0;
					mouse_clicked2=0;

				}

			}
			else if (mouse_clicked2==0)
			{
				glutPostRedisplay();
				for (int i=0;i<10;++i)
				{
					for (int sp=9;sp>1;--sp)
					{
						for (int l=sp,p=0;l<=15;++l,++p)
						{
							what_to_burst="";
							++scanCheck;

							for (int x=0;x<9;++x)
								{store_j[x]=-1;}
							for (int j=p,xp=0;j<l;++j)
							{
								//cout<<j<<"  ";
								if (twoD[i][j]!=-1)
									{
										what_to_burst+=alpha_twoD[i][j];
										store_j[xp]=j;
										++xp;
									}
								else
									break;
								//cout<<what_to_burst<<endl;
								//cout<<store_j[xp]<<" ";
							}

							for (int k=0;k<realdictionary_size;++k)
							{
								if (realdictionary[k].length()>=3 && what_to_burst[0]==realdictionary[k][0])
								{
									if (what_to_burst==realdictionary[k])
									{

										cout<<">>>"<<what_to_burst<<endl;
										for (int y=0,temp;y<9;++y)
										{
											if (store_j[y]!=-1)
											{
												twoD[i][store_j[y]]=-1;
												alpha_twoD[i][store_j[y]]='\0';
												if (mouse_clicked3==0)
												{
													temp=GetAlphabet();
													twoD[i][store_j[y]]=temp;
													alpha_twoD[i][store_j[y]]=alpha[temp];

												}
												else
													++score;
											}
										}
										break;
									}
								}
							}
						}
					}
				}
				what_to_burst="";
				for (int i=0;i<15;++i)
				{
					for (int sp=9;sp>1;--sp)
					{
						for (int l=sp,p=0;l<=10;++l,++p)
						{
							what_to_burst="";
							for (int x=0;x<9;++x)
								{store_j[x]=-1;}
							for (int j=p,xp=0;j<l;++j)
							{
								//cout<<j<<"  ";
								//cout<<alpha_twoD[j][i]<<" ";
								if (twoD[j][i]!=-1)
									{
										what_to_burst+=alpha_twoD[j][i];
										store_j[xp]=j;
										++xp;
									}
								else
									break;
								//cout<<store_j[xp]<<" ";
							}

							for (int k=0;k<realdictionary_size;++k)
							{
								if (realdictionary[k].length()>=3 && what_to_burst[0]==realdictionary[k][0])
								{
									if (what_to_burst==realdictionary[k])
									{
										cout<<">>>"<<what_to_burst<<endl;
										for (int y=0,temp;y<9;++y)
										{
											if (store_j[y]!=-1)
											{

												twoD[store_j[y]][i]=-1;
												alpha_twoD[store_j[y]][i]='\0';
												if (mouse_clicked3==0)
												{
													temp=GetAlphabet();
													twoD[store_j[y]][i]=temp;
													alpha_twoD[store_j[y]][i]=alpha[temp];

												}
												else
													++score;
											}
										}
										break;
									}
								}
							}
						}
					}
				}
				what_to_burst="";
				for (int k=0,sp=15;k<14;++k,--sp)
				{
						what_to_burst="";
						for (int x=0;x<9;++x)
							{store_j[x]=-1;}
						for (int i=0,xp=0,j=k;i<sp;++i,++j)
						{
							if (twoD[i][j]!=-1)
								{
									what_to_burst+=alpha_twoD[i][j];
									store_j[xp]=j;
									store_i[xp]=i;
									++xp;
								}
							else
								break;
						}
						for (int k=0;k<realdictionary_size;++k)
						{
							if (realdictionary[k].length()>=3 && what_to_burst[0]==realdictionary[k][0])
							{
								if (what_to_burst==realdictionary[k])
								{
									cout<<">>>"<<what_to_burst<<endl;
									for (int y=0,temp;y<9;++y)
									{
										if (store_j[y]!=-1)
										{
											twoD[store_i[y]][store_j[y]]=-1;
											alpha_twoD[store_i[y]][store_j[y]]='\0';
											if (mouse_clicked3==0)
											{
												temp=GetAlphabet();
												twoD[store_i[y]][store_j[y]]=temp;
												alpha_twoD[store_i[y]][store_j[y]]=alpha[temp];

											}
											else
												++score;
										}
									}
									break;
								}
							}
						}
						//cout<<what_to_burst<<endl;
				}

				if (scanCheck>=2000)
					mouse_clicked3=1;
				if (mouse_clicked3==1)
					mouse_clicked2=1;

			}
		}
		else
		{
			gamestop=1;
		}
	}
	else if (timeL<=0)
	{
		timeL=0;
	}
	glutPostRedisplay();
	glutTimerFunc(100.0/FPS, Timer, 0);
}

/*
* our gateway main function
  */
int main(int argc, char*argv[])
{
	InitRandomizer(); // seed the random number generator...

	//Dictionary for matching the words. It contains the 370099 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	//print first 5 words from the dictionary



	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets

	for(int i = 0; i < 10; ++i)    //Loop fills the array with alphabets which then tells display function what to draw
	{
		for(int j = 0; j < 15; ++j)
		{
			if( i < 2)
				twoD[i][j] = GetRandInRange(0, 25);
			else
				twoD[i][j] = -1;

//			cout<<i<<" "<<j;
		}
	//	cout<<endl;
	}

	for(int i = 0, y = height -120; i < nycells; ++i, y -= 60)   //Saves coordinates of every ball in 3d array
		{
			for(int j = 0, x = 10; j < nxcells; ++j, x += 60)
			{
				for (int k=0,temp=x;k<2;++k)
				{

					twoD_yx[i][j][k]=temp;
					temp=y;

				}
			}
		}
	for (int i=0,temp;i<10;++i) //maps array numers and converts them to respective alphabets
	{
		for (int j=0;j<15;++j)
		{
			temp=twoD[i][j];
			alpha_twoD[i][j]=alpha[temp];
		}
	}

	for (int i=0; i<dictionarysize; ++i)   //stores lenghth of each word in a array
	{
		wordcount=0;
		for (int j=0;;++j)
		{
			if (dictionary[i][j]=='\0')
				break;
			++wordcount;

		}
		wordcount_store[i]=wordcount;
	}

	for(int i=0,xp=0; i < dictionarysize; ++i)
		{
			if (wordcount_store[i]<=9 && wordcount_store[i]>=3)
			{
				realdictionary[xp]+=dictionary[i];
				++realdictionary_size;
				++xp;
			}
		}
	cout<<"Dictionary size used: "<<realdictionary_size<<endl;
	for (int k=0;k<realdictionary_size;++k)
	{
		//cout<<realdictionary[k]<<endl;
	}

	//my variables
	moving_ballC=GetRandInRange(0, 25);   //Gets random alphabet
	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("Valeed's ITCs Word Shooter"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse

	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0/FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	return 1;
}
#endif /* */
