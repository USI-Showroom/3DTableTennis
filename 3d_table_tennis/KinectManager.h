#ifndef KINECT_MANAGER_H__
#define KINECT_MANAGER_H__

#include "Player.h"
#include "Paddle.h"
#include <Shlobj.h>
#include <d2d1.h>
#include "NuiApi.h"
#include <fstream>

#include <QImage>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>

using namespace std;

class KinectManager
{
	static const int					BYTES_PER_PIXEL = 4;
	static const NUI_IMAGE_RESOLUTION	DEPTH_RESOLUTION = NUI_IMAGE_RESOLUTION_320x240;
    static const NUI_IMAGE_RESOLUTION	COLOR_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    KinectManager();

    /// <summary>
    /// Destructor
    /// </summary>
    ~KinectManager();

	inline PingPongPlayer*	getPlayer() { return PingPongPlayer::getInstance(); }

	inline Paddle*			getPaddle() { return Paddle::getInstance(); }

	/// <summary>
    /// Create the first connected Kinect found 
    /// </summary>
    /// <returns>true on success, false on failure</returns>
    bool				initialize();

	/// <summary>
    /// Main processing function
    /// </summary>
    void                update();

private:
    // Current Kinect
    INuiSensor*         nuiSensor;

	PingPongPlayer*		player;
	Paddle*				paddle;

	// Handles
	HANDLE				skeletonStreamHandle;
    HANDLE              nextSkeletonEvent;
	HANDLE              colorStreamHandle;
	HANDLE              nextColorFrameEvent;
	HANDLE              depthStreamHandle;
    HANDLE              nextDepthFrameEvent;

	LONG                depthWidth;
    LONG                depthHeight;
    LONG                colorWidth;
    LONG                colorHeight;
    LONG                colorToDepthDivisor;

	USHORT*             depthPixels;
	LONG*               colorCoordinates;
	BYTE*				colorPixels;
	vector<Vector4>		paddlePixels;
    
	QGridLayout			colorGridLayout;
	QLabel				colorDisplayLabel;
	QDialog				colorViewer;
	QImage				colorImg;

	QGridLayout			depthGridLayout;
	QLabel				depthDisplayLabel;
	QDialog				depthViewer;
	QImage				depthImg;

	QGridLayout			reconGridLayout;
	QLabel				reconDisplayLabel;
	QDialog				reconViewer;
	QImage				reconImg;

	ofstream			outStream;
	
	/// <summary>
    /// Handle new skeleton data
    /// </summary>
    HRESULT             processSkeleton();

	/// <summary>
    /// Handle new color data
    /// </summary>
	HRESULT				processColor();

	/// <summary>
    /// Handle new depth data
    /// </summary>
	HRESULT				processDepth();

	/// <summary>
    /// Handle new color with depth data
    /// </summary>
	void				processColorToDepth();

	void				computePaddlePositionAndNormal();

	/// <summary>
	/// Converts a skeleton point to screen space
	/// </summary>
	/// <param name="skeletonPoint">skeleton point to tranform</param>
	/// <param name="width">width (in pixels) of output buffer</param>
	/// <param name="height">height (in pixels) of output buffer</param>
	/// <returns>point in screen-space</returns>
	D2D1_POINT_2F		convertSkeletonToScreen(Vector4 skeletonPoint, int width, int height);

	/// <summary>
	/// Check whether x and y are inside a circle at center cx and cy with radius r
	/// </summary>
	bool				isInsideCircle(float x, float y, float cx, float cy, float r);

	/// <summary>
	/// Check whether the pixel at index pIdx is part of the paddle
	/// </summary>
	bool				isPixelOnPaddle(QColor pixelColor);

	/// <summary>
	/// Draw a circle
	/// </summary>
	void				drawCircle(QImage *target, float cx, float cy, int radius, int width);

	/// <summary>
	/// Draw a line
	/// </summary>
	void				drawLine(QImage *target, float ax, float ay, float bx, float by, int width);

};
#endif