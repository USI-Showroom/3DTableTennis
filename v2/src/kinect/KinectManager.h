#ifndef KINECT_MANAGER_H__
#define KINECT_MANAGER_H__

#include "Base.h"
#include "Player.h"
#include "Paddle.h"
#include <Shlobj.h>
#include <d2d1.h>
#include <fstream>
#include <Kinect.h>

#include <QImage>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>

using namespace std;

class IKinectSensor;
class ICoordinateMapper;
class IMultiSourceFrameReader;
class IBodyFrame;
class IColorFrame;
class IDepthFrame;

class KinectManager
{
public:
    KinectManager();
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
	IKinectSensor*         _nuiSensor;
	ICoordinateMapper*     _coordinateMapper;

	IMultiSourceFrameReader* _multiSourceFrameReader;

	WAITABLE_HANDLE _multiSourceFrameEvent;

	PingPongPlayer*		_player;
	Paddle*				_paddle;

	vector<Point3d>		_paddlePixels;


	int                _depthWidth;
	int                _depthHeight;
	int                _colorWidth;
	int                _colorHeight;

	UINT16*             _depthPixels;
	UINT				_depthPixelsSize;

	LONG*               _colorCoordinates;
	RGBQUAD*			_colorPixels;
	UINT				_colorPixelsSize;

	DepthSpacePoint*	_depthMapped;
	CameraSpacePoint*	_depthCamera;
    
	QGridLayout			_colorGridLayout;
	QLabel				_colorDisplayLabel;
	QDialog				_colorViewer;
	QImage*				_colorImg;

	QGridLayout			_depthGridLayout;
	QLabel				_depthDisplayLabel;
	QDialog				_depthViewer;
	QImage*				_depthImg;

	QGridLayout			_reconGridLayout;
	QLabel				_reconDisplayLabel;
	QDialog				_reconViewer;
	QImage*				_reconImg;

	
	/// <summary>
    /// Handle new skeleton data
    /// </summary>
	HRESULT             processSkeleton(IBodyFrame *frame);

	/// <summary>
    /// Handle new color data
    /// </summary>
	HRESULT				processColor(IColorFrame *frame);

	/// <summary>
    /// Handle new depth data
    /// </summary>
	HRESULT				processDepth(IDepthFrame *frame);

	/// <summary>
    /// Handle new color with depth data
    /// </summary>
	void				processColorToDepth();

	void				computePaddlePositionAndNormal();


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