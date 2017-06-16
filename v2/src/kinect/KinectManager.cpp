#include "KinectManager.h"
#include "Base.h"
#include <string>
#include <sstream>
#include <fstream>

#include <QPainter>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Eigen>


#include <Kinect.h>
#include <strsafe.h>
#include <math.h>
#include <Wincodec.h>

using namespace Eigen;

////////////////////////////  TO DO  ///////////////////////
// - color & depth sync (FrameEndEvent / quad time)
// - calibrate opponent
// - increase difficulty opponent (speed and aim)
// - run in release
// - increase COR paddle
////////////////////////////////////////////////////////////

#ifdef SHOW_BEHIND 
static const bool		SHOW_COLOR_VIEWER = true;
static const bool		SHOW_DEPTH_VIEWER = true;
static const bool		SHOW_RECON_VIEWER = true;
#else
static const bool		SHOW_COLOR_VIEWER = false;
static const bool		SHOW_DEPTH_VIEWER = false;
static const bool		SHOW_RECON_VIEWER = false;
#endif
static const bool		WRITE_TO_FILE = false;

static const int		PADDLE_PIXEL_RADIUS = 150;
static const int		PADDLE_MAX_DEPTH_MM = 200;
static const QColor		PADDLE_HSV_MIN = QColor::fromHsv(20, 0, 0);//(20, 80, 0);
static const QColor		PADDLE_HSV_MAX = QColor::fromHsv(60, 255, 255);

KinectManager::KinectManager() :
_nuiSensor(NULL),
_coordinateMapper(NULL),
_multiSourceFrameReader(NULL),
_depthMapped(NULL),
_depthImg(NULL),
_colorImg(NULL),
_colorPixels(NULL),
_reconImg(NULL)
{
	// Initialize and open viewers dialogs
	if (SHOW_COLOR_VIEWER)
	{
		_colorGridLayout.addWidget(&_colorDisplayLabel, 0, 0);
		_colorViewer.setLayout(&_colorGridLayout);
		_colorViewer.setWindowTitle("Color");
		_colorViewer.setMinimumHeight(50);
		_colorViewer.setMinimumWidth(50);
		_colorViewer.open();
	}
	if (SHOW_DEPTH_VIEWER)
	{
		_depthGridLayout.addWidget(&_depthDisplayLabel, 0, 0);
		_depthViewer.setLayout(&_depthGridLayout);

		_depthViewer.setWindowTitle("Depth");
		_depthViewer.setMinimumHeight(50);
		_depthViewer.setMinimumWidth(50);

		_depthViewer.open();
	}
	if (SHOW_RECON_VIEWER)
	{
		_reconGridLayout.addWidget(&_reconDisplayLabel, 0, 0);
		_reconViewer.setLayout(&_reconGridLayout);

		_reconViewer.setWindowTitle("Recon");
		_reconViewer.setMinimumHeight(50);
		_reconViewer.setMinimumWidth(50);

		_reconViewer.open();
	}
}


KinectManager::~KinectManager()
{
	SafeRelease(_nuiSensor);
	SafeRelease(_coordinateMapper);
	SafeRelease(_multiSourceFrameReader);


	// Done with pixel data
	//delete[] depthPixels;
	// delete[] colorCoordinates;
	//delete[] colorPixels;
}


bool KinectManager::initialize()
{
	int iSensorCount = 0;
	HRESULT hr = GetDefaultKinectSensor(&_nuiSensor);
	if (FAILED(hr))
		return false;

	if (_nuiSensor)
	{
		hr = _nuiSensor->Open();
		if (FAILED(hr))
			return false;
		
		// Create the sensor so we can check status, if we can't create it, move on to the next
		hr = _nuiSensor->get_CoordinateMapper(&_coordinateMapper);
		if (FAILED(hr))
			return false;

		// Opening the multiSourceFrameReader Depth Color and Body source are open. 
		// This must be correct, the | operator is commutative
		hr = _nuiSensor->OpenMultiSourceFrameReader(
			FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_Body,
			&_multiSourceFrameReader);
		if (FAILED(hr))
			return false;
	
		//Cheeck if opened correctly
		/*
			DWORD word = NULL;
			hr = _multiSourceFrameReader->get_FrameSourceTypes(&word);
			std::cerr << word << std::endl;
		*/
	
		//Subscribe to the frame in the multiSourceFrameRwader
		//hr = _multiSourceFrameReader->SubscribeMultiSourceFrameArrived(&_multiSourceFrameEvent);
		if (FAILED(hr)) {
			std::cerr << "Subscription fail" << std::endl;
			return false;
		} 
	}
	else
		return false;

	_player = getPlayer();
	_paddle = getPaddle();

	return true;
}


/// <summary>
/// Main processing function
/// </summary>
void KinectManager::update()
{
	if (!_nuiSensor)
		return;

	IMultiSourceFrameArrivedEventArgs* multiSourceFrameArgs = NULL;
	IMultiSourceFrameReference* multiSourceFrameRef = NULL;
	IMultiSourceFrame* multiSourceFrame = NULL;

	IDepthFrame* depthFrame = NULL;
	IColorFrame* colorFrame = NULL;
	IBodyFrame* bodyFrame = NULL;

	//Handler array -> cast WAITABLE_HANDLE to HANDLE
	//HANDLE handles[] = { reinterpret_cast<HANDLE>(_multiSourceFrameEvent) };

	//Instead of acquire the frame we check if a frame is available from the subscription
	//HRESULT hr = _multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame);

	//Wait for the multiframe to arrive: 1 frame, from handle, signaling bool, 16ms = approx 60FPS
	//int eventStatus = WaitForMultipleObjects(_countof(handles), handles, false, INFINITE);

	//if (eventStatus == WAIT_OBJECT_0) {
	//	HRESULT hr = _multiSourceFrameReader->GetMultiSourceFrameArrivedEventData(_multiSourceFrameEvent, &multiSourceFrameArgs);

		//if (FAILED(hr)){
			//std::cerr << "FAILED: Failed to get the eventData" << std::endl;
			//return;
		//}

		//Get the reference of the frame from the EventData
		//HRESULT hr = multiSourceFrameArgs->get_FrameReference(&multiSourceFrameRef);

	//	if (FAILED(hr)){
		//	std::cerr << "FAILED: Failed to get the reference from the eventData" << std::endl;
			//return;
		//}

		//Get the frame from the frame reference
		//hr = multiSourceFrameRef->AcquireFrame(&multiSourceFrame);
		HRESULT hr = _multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame);

		if (FAILED(hr)){
			std::cerr << "FAILED: Failed to get the multiSourceFrame" << std::endl;
			return;
		}

		IBodyFrameReference* bodyFrameReference = NULL;
		hr = multiSourceFrame->get_BodyFrameReference(&bodyFrameReference);
		if (SUCCEEDED(hr)) {
			hr = bodyFrameReference->AcquireFrame(&bodyFrame);
			
			if (FAILED(hr)){
				std::cerr << "FAILED: Failed to get BodyFrame" << std::endl;
			}
		}
		else
			std::cerr << "FAILED: Failed to get Body frame reference" << std::endl;
		SafeRelease(bodyFrameReference);

		IDepthFrameReference* depthFrameReference = NULL;
		hr = multiSourceFrame->get_DepthFrameReference(&depthFrameReference);
		if (SUCCEEDED(hr)) {
			hr = depthFrameReference->AcquireFrame(&depthFrame);
			
			if (FAILED(hr))
				std::cerr << "FAILED: Failed to get DepthFrame" << std::endl;
		}
		else
			std::cerr << "FAILED: Failed to get Depth frame reference" << std::endl;
		SafeRelease(depthFrameReference);

		IColorFrameReference* colorFrameReference = NULL;
		hr = multiSourceFrame->get_ColorFrameReference(&colorFrameReference);
		if (SUCCEEDED(hr)) {
			hr = colorFrameReference->AcquireFrame(&colorFrame);
		
			if (FAILED(hr))
				std::cerr << "FAILED: Failed to get ColorFrame" << std::endl;
		}
		else
			std::cerr << "FAILED: Failed to get Color frame reference" << std::endl;
		SafeRelease(colorFrameReference);

		bool needToDraw = false;
		bool hasDepth = false;
		bool hasColor = false;

		if (SUCCEEDED(processSkeleton(bodyFrame)))
			needToDraw = true;

		if (SUCCEEDED(processDepth(depthFrame))){
			needToDraw = true;
			hasDepth = true;
		}

		if (SUCCEEDED(processColor(colorFrame))){
			needToDraw = true;
			hasColor = true;
		}

		if (hasDepth && hasColor)
		{
			processColorToDepth();
			computePaddlePositionAndNormal();
		}


		SafeRelease(depthFrame);
		SafeRelease(colorFrame);
		SafeRelease(bodyFrame);
		SafeRelease(multiSourceFrameRef);
		SafeRelease(multiSourceFrameArgs);
		SafeRelease(multiSourceFrame);
	//}
	//else {
		//Update the game variables?
	//	std::cerr << "TIMEOUT: Failed to get the subscribed frame" << std::endl;
	//	return;
	//}

}


/// <summary>
/// Handle new skeleton data
/// </summary>
HRESULT KinectManager::processSkeleton(IBodyFrame *skeletonFrame)
{
	HRESULT hr = S_OK;

	if (!skeletonFrame)
		return E_FAIL;

	IBody* bodies[BODY_COUNT] = { 0 };
	hr = skeletonFrame->GetAndRefreshBodyData(_countof(bodies), bodies);

	if (FAILED(hr)) return hr;

	// Some smoothing with little latency (defaults).
	// Only filters out small jitters.
	// Good for gesture recognition in games.
	//static const NUI_TRANSFORM_SMOOTH_PARAMETERS defaultParams = {0.5f, 0.5f, 0.5f, 0.05f, 0.04f};

	// Smoothed with some latency.
	// Filters out medium jitters.
	// Good for a menu system that needs to be smooth but
	// doesn't need the reduced latency as much as gesture recognition does.
	//static const NUI_TRANSFORM_SMOOTH_PARAMETERS somewhatLatentParams = {0.5f, 0.1f, 0.5f, 0.1f, 0.1f};

	// Very smooth, but with a lot of latency.
	// Filters out large jitters.
	// Good for situations where smooth data is absolutely required
	// and latency is not an issue.
	//static const NUI_TRANSFORM_SMOOTH_PARAMETERS verySmoothParams = {0.7f, 0.3f, 1.0f, 1.0f, 1.0f};

	// Smooth out the skeleton data
	//nuiSensor->NuiTransformSmooth(&skeletonFrame, &defaultParams);
	//skeletonFrame->

	// Track closest skeleton
	//float closestDistance = 10000.0f;
	float mostCentered = 10000.0f;
	int closestID = -1;
	Joint joints[JointType_Count];

	for (int i = 0; i < BODY_COUNT; ++i)
	{
		IBody *body = bodies[i];

		if (!body) continue;

		BOOLEAN tracked;
		hr = body->get_IsTracked(&tracked);

		if (FAILED(hr) || !tracked) continue;

		hr = body->GetJoints(_countof(joints), joints);
		if (FAILED(hr)) continue;

		Joint &head = joints[JointType_Head];
		if (fabs(head.Position.X) < mostCentered)
		{
			closestID = i;
			mostCentered = fabs(head.Position.X);
		}
	}

	if (closestID >= 0)
	{
		IBody *body = bodies[closestID];
		hr = body->GetJoints(_countof(joints), joints);
		if (SUCCEEDED(hr))
		{
			// Set player's joints
			const CameraSpacePoint &head = joints[JointType_Head].Position;
			_player->setHead(head.X, head.Y, head.Z);

			const CameraSpacePoint &hip = joints[JointType_SpineBase].Position;
			_player->setHip(hip.X, hip.Y, hip.Z);

			const CameraSpacePoint &handLeft = joints[JointType_HandLeft].Position;
			_player->setHand(handLeft.X, handLeft.Y, handLeft.Z, true);

			const CameraSpacePoint &handRight = joints[JointType_HandRight].Position;
			_player->setHand(handRight.X, handRight.Y, handRight.Z, false);

			const CameraSpacePoint &footLeft = joints[JointType_FootLeft].Position;
			_player->setFoot(footLeft.X, footLeft.Y, footLeft.Z, true);

			const CameraSpacePoint &footRight = joints[JointType_FootRight].Position;
			_player->setFoot(footRight.X, footRight.Y, footRight.Z, false);
		}
	}

	for (int i = 0; i < _countof(bodies); ++i)
		SafeRelease(bodies[i]);

	return S_OK;
}

/// <summary>
/// Handle new depth data
/// </summary>
HRESULT KinectManager::processDepth(IDepthFrame *depthFrame)
{
	HRESULT hr = S_OK;

	if (!depthFrame)
	{
		std::cerr << "Process depth failed to get next frame" << std::endl;
		return E_FAIL;
	}
	IFrameDescription* description = NULL;
	hr = depthFrame->get_FrameDescription(&description);
	if (FAILED(hr)) return hr;

	hr = description->get_Width(&_depthWidth);
	if (FAILED(hr)) return hr;
	hr = description->get_Height(&_depthHeight);
	if (FAILED(hr)) return hr;

	USHORT minRelDepth, maxRelDepth;
	hr = depthFrame->get_DepthMinReliableDistance(&minRelDepth);
	if (FAILED(hr)) return hr;
	hr = depthFrame->get_DepthMaxReliableDistance(&maxRelDepth);
	if (FAILED(hr)) return hr;

	hr = depthFrame->AccessUnderlyingBuffer(&_depthPixelsSize, &_depthPixels);
	if (FAILED(hr)) return hr;

	if (!_depthCamera)
		_depthCamera = new CameraSpacePoint[_depthHeight * _depthWidth];

	if (SHOW_DEPTH_VIEWER)
	{
		if (!_depthImg || _depthImg->height() != _depthHeight || _depthImg->width() != _depthWidth){
			delete _depthImg;
			_depthImg = new QImage(_depthWidth, _depthHeight, QImage::Format_RGB32);

			//inversed depth width
			_depthViewer.setMinimumHeight(_depthHeight);
			_depthViewer.setMinimumWidth(_depthWidth);

		}

		for (int i = 0; i < _depthWidth * _depthHeight; ++i)
		{
			USHORT depth = _depthPixels[i];

			// To convert to a byte, we're discarding the most-significant
			// rather than least-significant bits.
			// We're preserving detail, although the intensity will "wrap."
			// Values outside the reliable depth range are mapped to 0 (black).

			// Note: Using conditionals in this loop could degrade performance.
			// Consider using a lookup table instead when writing production code.
			BYTE intensity = static_cast<BYTE>((depth >= minRelDepth) && (depth <= maxRelDepth) ? (depth % 256) : 0);

			const int indexI = i % _depthWidth;
			const int indexJ = i / _depthWidth;

			assert(indexI < _depthWidth);
			assert(indexJ < _depthHeight);

			// Draw a grayscale image of the depth
			QRgb col = qRgb(intensity, intensity, intensity);
			_depthImg->setPixel(indexI, indexJ, col);
		}

		// Get position of player's hand
		const Point3d &playHand = _player->getPlayingHand();
		CameraSpacePoint tmp;
		tmp.X = playHand.x(); tmp.Y = playHand.y(); tmp.Z = playHand.z();
		DepthSpacePoint handScreen;
		_coordinateMapper->MapCameraPointToDepthSpace(tmp, &handScreen);
		float hsx = handScreen.X;
		float hsy = handScreen.Y;

		// Draw circle at hand's position
		drawCircle(_depthImg, hsx, hsy, 50, 3);

		//// Display frame
		QPixmap pixmap = QPixmap::fromImage(*_depthImg);
		_depthDisplayLabel.setPixmap(pixmap.scaled(_depthWidth, _depthHeight, Qt::KeepAspectRatio));
		_depthDisplayLabel.adjustSize();
	}

	return S_OK;
}


/// <summary>
/// Handle new color data
/// </summary>
HRESULT KinectManager::processColor(IColorFrame *colorFrame)
{
	HRESULT hr = S_OK;

	if (!colorFrame)
	{
		std::cerr << "Process color failed to get next frame" << std::endl;
		return E_FAIL;
	}

	IFrameDescription* colorFrameDescription = NULL;
	hr = colorFrame->get_FrameDescription(&colorFrameDescription);
	if (FAILED(hr)) return hr;

	hr = colorFrameDescription->get_Width(&_colorWidth);
	if (FAILED(hr)) return hr;
	hr = colorFrameDescription->get_Height(&_colorHeight);
	if (FAILED(hr)) return hr;


	ColorImageFormat imageFormat = ColorImageFormat_None;
	hr = colorFrame->get_RawColorImageFormat(&imageFormat);
	if (FAILED(hr)) return hr;
	if (imageFormat != ColorImageFormat_Bgra){
		if (!_colorPixels)
			_colorPixels = new RGBQUAD[_colorWidth * _colorHeight];
		_colorPixelsSize = _colorWidth * _colorHeight * sizeof(RGBQUAD);
		hr = colorFrame->CopyConvertedFrameDataToArray(_colorPixelsSize, reinterpret_cast<BYTE*>(_colorPixels), ColorImageFormat_Bgra);
	}
	else
		hr = colorFrame->AccessRawUnderlyingBuffer(&_colorPixelsSize, reinterpret_cast<BYTE**>(&_colorPixels));

	if (FAILED(hr)) return hr;

	if (!_depthMapped)
		_depthMapped = new DepthSpacePoint[_colorHeight*_colorWidth];

	if (SHOW_COLOR_VIEWER)
	{
		if (!_colorImg || _colorImg->height() != _colorHeight || _colorImg->width() != _colorWidth){
			delete _colorImg;
			_colorImg = new QImage(_colorWidth, _colorHeight, QImage::Format_RGB32);

			// this makes no sense 1920*1080 dived by 100 = 19 * 10 -> moreover inversed height and width
			//_colorViewer.setMinimumHeight(_colorWidth/100);
			//_colorViewer.setMinimumWidth(_colorHeight/100);

			_colorViewer.setMinimumHeight(_colorHeight);
			_colorViewer.setMinimumWidth(_colorWidth);

			delete _reconImg;
			_reconImg = new QImage(_colorWidth, _colorHeight, QImage::Format_RGB32);


			_reconViewer.setMinimumHeight(_colorHeight);
			_reconViewer.setMinimumWidth(_colorWidth);

		}

		for (int i = 0; i < _colorWidth*_colorHeight; ++i)
		{
			const RGBQUAD &color = _colorPixels[i];

			QRgb col = qRgb(color.rgbRed, color.rgbGreen, color.rgbBlue);

			_colorImg->setPixel(i % _colorWidth, i / _colorWidth, col);
		}

		// Get position of player's hand
		const Point3d &playHand = _player->getPlayingHand();
		CameraSpacePoint tmp;
		tmp.X = playHand.x(); tmp.Y = playHand.y(); tmp.Z = playHand.z();

		// Convert to screen coords
		ColorSpacePoint handScreen;
		_coordinateMapper->MapCameraPointToColorSpace(tmp, &handScreen);
		float hsx = handScreen.X;
		float hsy = handScreen.Y;

		// Draw circle at hand's position
		drawCircle(_colorImg, hsx, hsy, 60, 6);

		//// Display frame
		_colorDisplayLabel.setPixmap(QPixmap::fromImage(*_colorImg));
		_colorDisplayLabel.adjustSize();
	}

	return S_OK;
}


/// <summary>
/// Handle new color with depth data
/// </summary>
void KinectManager::processColorToDepth()
{
	using namespace std;

	if (!_depthMapped || !_depthCamera) return;
	HRESULT hr;


	// Get position of player's hand
	const Point3d &playHand = _player->getPlayingHand();
	CameraSpacePoint tmp;
	tmp.X = playHand.x(); tmp.Y = playHand.y(); tmp.Z = playHand.z();

	// Convert to screen coords
	ColorSpacePoint handScreen;
	hr = _coordinateMapper->MapCameraPointToColorSpace(tmp, &handScreen);
	if (FAILED(hr)) {
		std::cerr << "1 MapCameraPointToColorSpace failure" << endl;
		return;
	}
	const float hsx = handScreen.X;
	const float hsy = handScreen.Y;

	DepthSpacePoint handScreenD;
	hr = _coordinateMapper->MapCameraPointToDepthSpace(tmp, &handScreenD);
	if (FAILED(hr))  {
		std::cerr << "2 MapCameraPointToDepthSpace failure" << endl;
		return;
	}
	int ghindex = handScreenD.X + (handScreenD.Y * _depthWidth);
	UINT16 handDepth = 0;
	if (ghindex >= 0 && handScreenD.X < _depthWidth && handScreenD.Y < _depthHeight){
		handDepth = _depthPixels[ghindex];
	}

	_depthMapped = new DepthSpacePoint[_colorHeight * _colorWidth];
	hr = _coordinateMapper->MapColorFrameToDepthSpace(_depthWidth * _depthHeight, (UINT16*)_depthPixels, _colorWidth * _colorHeight, _depthMapped);
	if (FAILED(hr)) {
		std::cerr << "3 MapColorFrameToDepthSpace failure" << endl;
		return;
	}

	_depthCamera = new CameraSpacePoint[_depthWidth * _depthHeight];
	hr = _coordinateMapper->MapDepthFrameToCameraSpace(_depthWidth * _depthHeight, (UINT16*)_depthPixels, _depthWidth * _depthHeight, _depthCamera);
	if (FAILED(hr)) {
		std::cerr << "4 MapDepthFrameToCameraSpace failure" << endl;
		return;
	}


	const int startX = max(0.f, hsx - PADDLE_PIXEL_RADIUS);
	const int endX = min(_colorWidth*1.f, hsx + PADDLE_PIXEL_RADIUS);
	const int startY = max(0.f, hsy - PADDLE_PIXEL_RADIUS);
	const int endY = min(_colorHeight * 1.f, hsy + PADDLE_PIXEL_RADIUS);

	if (startX < endX && startY < endY){
		// Clear the vector containing the paddle's pixels of the previous frame
		_paddlePixels.clear();

		if (SHOW_RECON_VIEWER) _reconImg->fill(Qt::black); // Set background color
	}

	for (int x = startX; x < endX; ++x)
	{
		for (int y = startY; y < endY; ++y)
		{
			const int i = x + _colorWidth*y;
			DepthSpacePoint tmp = _depthMapped[i];
			int depthIndex = tmp.X + (tmp.Y * _depthWidth);
			UINT16 depth = 0;
			if (tmp.X >= 0 && tmp.X < _depthWidth && tmp.Y >= 0 && tmp.Y < _depthHeight)
				depth = _depthPixels[depthIndex];
			if (handDepth == 0)
				_reconImg->setPixel(x, y, QColor(0, 1, 0).rgb());
			else if (depth == 0)
				_reconImg->setPixel(x, y, QColor(1, 0, 0).rgb());
			else
				_reconImg->setPixel(x, y, QColor(0, 0, depth % 255).rgb());
			//		// If we're tracking a player for the current pixel, draw from the color camera
			//		if (player > 0)
			//		{

			// Make sure the depth pixel maps to a valid point in color space and is near the hand
			if (isInsideCircle(x, y, hsx, hsy, PADDLE_PIXEL_RADIUS))
			{
				RGBQUAD tmpcol = _colorPixels[i];
				QColor col = QColor(tmpcol.rgbRed, tmpcol.rgbGreen, tmpcol.rgbBlue);
				if (isPixelOnPaddle(col))
				{
					if (handDepth > 0 && depth>0 && depth >= handDepth - PADDLE_MAX_DEPTH_MM && depth <= handDepth + PADDLE_MAX_DEPTH_MM)
					{

						CameraSpacePoint point = _depthCamera[depthIndex];
						_paddlePixels.push_back(Point3d(point.X, point.Y, point.Z));

						if (SHOW_RECON_VIEWER)
							_reconImg->setPixel(x, y, col.rgb());
					}
				}

			}
			//		}
		}
	}

	if (SHOW_RECON_VIEWER)
	{
		// Draw circle at hand's position
		drawCircle(_reconImg, hsx, hsy, PADDLE_PIXEL_RADIUS, 2);

		// Display frame
		_reconDisplayLabel.setPixmap(QPixmap::fromImage(*_reconImg));
		_reconDisplayLabel.adjustSize();
	}
}

void KinectManager::computePaddlePositionAndNormal()
{
	// Handle the case where there are too few pixels on the paddle
	//cout << paddlePixels.size() << endl;
	if (_paddlePixels.size() < 100)
	{
		_paddle->updatePosition(_player->getPlayingHand());
		_paddle->updateNormal(Point3d(0, 0, -1));
		return;
	}

	typedef std::pair<double, int> myPair;
	typedef std::vector<myPair> PermutationIndices;

	// Convert vector of 3d points into matrix
	MatrixX3d mat(_paddlePixels.size(), 3);
	for (size_t i = 0; i < _paddlePixels.size(); i++)
	{
		mat(i, 0) = _paddlePixels[i].x();
		mat(i, 1) = _paddlePixels[i].y();
		mat(i, 2) = _paddlePixels[i].z();
	}

	Vector3d mean = mat.colwise().mean();
	MatrixX3d mat_center = mat.rowwise() - mat.colwise().mean();
	MatrixX3d cov = mat_center.adjoint() * mat_center;

	SelfAdjointEigenSolver<MatrixX3d> eig(cov);
	Vector3d eigval = eig.eigenvalues().real();
	MatrixX3d eigvec = eig.eigenvectors().real();

	// Find the smallest eigenvalue and set the respective eigenvector as the normal of the paddle 
	PermutationIndices pi;
	for (int i = 0; i < 3; i++)
		pi.push_back(std::make_pair(eigval(i), i));
	sort(pi.begin(), pi.end());
	int idx_smallest_eigval = pi[0].second;
	double smallestEigval = pi[0].first;
	int idx_second_smallest_eigval = pi[1].second;
	double secondSmallestEigval = pi[1].first;

	// Set default paddle if points are too disperse to form a plane
	//cout << secondSmallestEigval - smallestEigval << endl;
	if (secondSmallestEigval - smallestEigval < 0.5)
	{
		//paddle->updatePosition(player->getPlayingHand());
		_paddle->updatePosition(Point3d(mean.x(), mean.y(), mean.z()));
		_paddle->updateNormal(Point3d(0, 0, -1));
		return;
	}

	Vector3d paddleNormal = eigvec.col(idx_smallest_eigval);
	paddleNormal.normalize();

	// Invert the normal to face the kinect, if necessary
	if (paddleNormal.dot(Vector3d(0, 0, -1)) < 0)
		paddleNormal *= -1;

	// Correct the normal w.r.t the sensor's angle
	//LONG cameraAngle = 0;
	//nuiSensor->NuiCameraElevationGetAngle(&cameraAngle);
	//cout << cameraAngle << endl;

	if (SHOW_RECON_VIEWER)
	{
		// Draw the mean of the points
		//Vector4 meanV4 = { mean(0), mean(1), mean(2), 1 };
		//D2D1_POINT_2F meanScreen = convertSkeletonToScreen(meanV4, colorWidth * 2, colorHeight * 2);
		//drawCircle(_reconImg, meanScreen.x, meanScreen.y, 3, 5);

		// Draw the normal of the paddle's plane
	//	Vector4 eigV4 = { mean(0) + paddleNormal(0) / 5, mean(1) + paddleNormal(1) / 5, mean(2) + paddleNormal(2) / 5, 1 };
	//	D2D1_POINT_2F eigScreen = convertSkeletonToScreen(eigV4, colorWidth * 2, colorHeight * 2);
	//	drawLine(&reconImg, meanScreen.x, meanScreen.y, eigScreen.x, eigScreen.y, 2);

	//	reconDisplayLabel.setPixmap(QPixmap::fromImage(reconImg));
	}

	// Set paddle position and normal
	//_paddle->updatePosition(Point3d(floor(mean.x()*1000)/1000.0, floor(mean.y()*1000)/1000.0, floor(mean.z()*1000)/1000.0));
	_paddle->updatePosition(_player->getPlayingHand());
	_paddle->updateNormal(Point3d(paddleNormal.x(), paddleNormal.y(), paddleNormal.z()));
}


/// <summary>
/// Check whether x and y are inside a circle at center cx and cy with radius r
/// </summary>
bool KinectManager::isInsideCircle(float x, float y, float cx, float cy, float r)
{
	float dx = x < cx ? cx - x : x - cx;
	float dy = y < cy ? cy - y : y - cy;

	return sqrt(dx*dx + dy*dy) <= r;
}


/// <summary>
/// Check whether the pixel at index pIdx is part of the paddle
/// </summary>
bool KinectManager::isPixelOnPaddle(QColor pixelColor)
{
	return	pixelColor.hue() == -1 ||	// take also white color
		(PADDLE_HSV_MIN.hue() <= pixelColor.hue() &&
		PADDLE_HSV_MAX.hue() >= pixelColor.hue() &&
		PADDLE_HSV_MIN.saturation() <= pixelColor.saturation() &&
		PADDLE_HSV_MAX.saturation() >= pixelColor.saturation() &&
		PADDLE_HSV_MIN.value() <= pixelColor.value() &&
		PADDLE_HSV_MAX.value() >= pixelColor.value());
}

/// <summary>
/// Draw a circle
/// </summary>
void KinectManager::drawCircle(QImage *target, float cx, float cy, int radius, int width)
{
	QPainter painter(target);
	QPen pen(Qt::blue);
	pen.setWidth(width);
	painter.setPen(pen);
	painter.drawEllipse(QPointF(cx, cy), radius, radius);
}

/// <summary>
/// Draw a line
/// </summary>
void KinectManager::drawLine(QImage *target, float ax, float ay, float bx, float by, int width)
{
	QPainter painter(target);
	QPen pen(Qt::blue);
	pen.setWidth(width);
	painter.setPen(pen);
	painter.drawLine(ax, ay, bx, by);
}