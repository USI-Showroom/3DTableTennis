#ifndef DEFINES_H
#define DEFINES_H

#include <string>
#include <iostream>
#include <QFont>
#include <QFontDatabase>
#include <qstringlist>

//#define SHOW_BEHIND

using namespace std;

#define KINECT
//#define USE_VIRTUAL_PADDLE
//#define DEFAUL_STEUP
#define STEREO_PING_PONG 

class Path{
private:
	static Path *instance;

	string projRoot;
	string mesh;
	string textures;

	Path()
	{
		size_t index;
		projRoot=__FILE__;
		index=projRoot.find_last_of("/\\");
		projRoot=projRoot.substr(0,index);

		mesh=projRoot+"\\meshes";

		textures=projRoot+"\\textures";
	}

	virtual ~Path(){}

public:
	static const string projectRoot();

	static const string meshFolder();
	static const string textureFolder();
};

class Fonts
{
private:
	static Fonts *instance;

	QFont log;

	Fonts();
	virtual ~Fonts(){}

public:
	static const QFont logFont();
};

///////// kinect pixel to cm conversion (empirical function)
class KinectConversion
{
public:
	inline static double Width2Meters(const double dist, const double px)
	{
		return ((1.1842*dist/10.0+5.1478)/640.0*(px-320.0))/100.0;
	}

	inline static double Heigth2Meters(const double dist, const double px, const double footH)
	{
		return (1.1842*dist/10.0+5.1478)/640.0*(480-px)/100.0 - footH;
	}

	inline static double Depth2Meters(const double dist)
	{
		return dist/1000.0;
	}

	inline static double wMeters2Px(const double dist, const double cm)
	{
		return (320.0 * (5921.0 * dist + 2.5739e5 + 1.0e7*cm)) / (5921 * dist + 2.5739e5);
	}

	inline static double hMeters2Px(const double dist, const double cm, const double footH)
	{
		return (-7.68e-8) * (-3.700625e13*dist-4.7336875e15 + 3.125e14*footH + 3.125e16*cm) / (5921.0*dist+2.5739e5);
	}

};

/*

///////// opengl stuff
#define SCREEN_WIDTH 2.9 //m
#define DISTANCE 2.6 //m

#define NEAR_PLANE 0.5 //m
#define FAR_PLANE 15   //m

#define FIELD_OF_VIEW M_PI/4.0 //rad
#define SCENE_RADIUS 2.74/2.0 //m
#define DISTANCE_OFFSET 0 //m
#define LOOK_DISTANCE 5 //m
#define LOOK_HEIGTH 1.0 //m
#define PLAYER_HEIGTH 1.5 //m
#define CENTER_HEIGTH 1.0 //m
#define CENTER_DISTANCE 5 //m


///////////// physics
#define DRAG_COEFF 0.8 //without spin
#define AIR_DENSITY 1.21e3 // g/m^3

#define TABLE_HEIGHT 0.76 //m
#define TABLE_LENGTH 2.74 //m
#define TABLE_WIDTH 1.525 //m

#define NET_HEIGHT 0.1525 //m

#define BALL_MASS 2.7 //g
#define BALL_RADIUS 0.02 //m

#define PADDLE_RADIUS 0.16 //m

#define PADDLE_TANGENTIAL_FRICTION 1
#define TABLE_TANGENTIAL_FRICTION 1
#define PADDLE_CONTACT_TIME 0.2 //those values take in cosideration also the friction
#define TABLE_CONTACT_TIME 0.01 //there are used for the spin

#define BALL_TABLE_COR 0.92 //cor=coefficient of restitution
#define BALL_NET_COR 0.5
#define BALL_PADDLE_COR 0.8

#define GRAVITY_ACCELERATION -9.81 //m/s^2


*/
///////////////////// Graphics settings

//#define USE_MIPMAPPING


#endif
