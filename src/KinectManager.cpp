#include "KinectManager.h"
#include "Base.h"
#include <string>
#include <sstream>
#include <fstream>

#include <QPainter>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Eigen>
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

static const int		PADDLE_PIXEL_RADIUS = 50;
static const int		PADDLE_MAX_DEPTH_MM = 100;
static const QColor		PADDLE_HSV_MIN = QColor::fromHsv(20, 0, 0);//(20, 80, 0);
static const QColor		PADDLE_HSV_MAX = QColor::fromHsv(60, 255, 255);

KinectManager::KinectManager() :
	nuiSensor(NULL),
	nextSkeletonEvent(INVALID_HANDLE_VALUE),
    skeletonStreamHandle(INVALID_HANDLE_VALUE),
	nextColorFrameEvent(INVALID_HANDLE_VALUE),
    colorStreamHandle(INVALID_HANDLE_VALUE),
	nextDepthFrameEvent(INVALID_HANDLE_VALUE),
    depthStreamHandle(INVALID_HANDLE_VALUE)
{
	// Get resolution as DWORDS, but store as LONGs to avoid casts later
    DWORD width = 0;
    DWORD height = 0;

    NuiImageResolutionToSize(DEPTH_RESOLUTION, width, height);
    depthWidth  = static_cast<LONG>(width);
    depthHeight = static_cast<LONG>(height);

    NuiImageResolutionToSize(COLOR_RESOLUTION, width, height);
    colorWidth  = static_cast<LONG>(width);
    colorHeight = static_cast<LONG>(height);

    colorToDepthDivisor = colorWidth/depthWidth;

	// Create heap storage for depth, color and paddle pixel data
    depthPixels = new USHORT[depthWidth*depthHeight];
    colorCoordinates = new LONG[depthWidth*depthHeight*2];
	colorPixels = new BYTE[colorWidth*colorHeight*BYTES_PER_PIXEL];

	// Initialize and open viewers dialogs
	if(SHOW_COLOR_VIEWER)
	{
		colorGridLayout.addWidget(&colorDisplayLabel,0,0);
		colorViewer.setLayout(&colorGridLayout);
		colorViewer.open();
		colorImg = QImage(colorWidth, colorHeight, QImage::Format_RGB32);
	}
	if(SHOW_DEPTH_VIEWER)
	{
		depthGridLayout.addWidget(&depthDisplayLabel,0,0);
		depthViewer.setLayout(&depthGridLayout);
		depthViewer.open();
		depthImg = QImage(depthWidth, depthHeight, QImage::Format_RGB32);
	}
	if(SHOW_RECON_VIEWER)
	{
		reconGridLayout.addWidget(&reconDisplayLabel,0,0);
		reconViewer.setLayout(&reconGridLayout);
		reconViewer.open();
		reconImg = QImage(colorWidth, colorHeight, QImage::Format_RGB32);
		reconImg.fill(Qt::white);
	}
	
	if (WRITE_TO_FILE)
	{
		outStream.open("myfile.txt", std::ios::app);
	}
}


KinectManager::~KinectManager()
{
	if (nextSkeletonEvent && (nextSkeletonEvent != INVALID_HANDLE_VALUE))
        CloseHandle(nextSkeletonEvent);

	if (nextDepthFrameEvent != INVALID_HANDLE_VALUE)
        CloseHandle(nextDepthFrameEvent);

	if (nextColorFrameEvent != INVALID_HANDLE_VALUE)
        CloseHandle(nextColorFrameEvent);

	if (nuiSensor != NULL)
    {
        nuiSensor->Release();
        nuiSensor = NULL;
    }

	if (WRITE_TO_FILE)
	{
		outStream.close();
	}

	// Done with pixel data
    delete[] depthPixels;
    delete[] colorCoordinates;
    delete[] colorPixels;
}


bool KinectManager::initialize()
{    
	INuiSensor *tmpSensor;

	int iSensorCount = 0;
	HRESULT hr = NuiGetSensorCount(&iSensorCount);
	if (FAILED(hr))
		return false;

	for (int i = 0; i < iSensorCount; ++i)
	{
		// Create the sensor so we can check status, if we can't create it, move on to the next
		hr = NuiCreateSensorByIndex(i, &tmpSensor);
		if (FAILED(hr))
			continue;

		hr = tmpSensor->NuiStatus();
		if (S_OK == hr)
		{
			nuiSensor = tmpSensor;
			break;
		}

		tmpSensor->Release();
	}

	if(!nuiSensor)
		return false;

	// Initialize the Kinect and specify that we'll be using skeleton
	hr = nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
	if (FAILED(hr))
		return false;

	// Create an event that will be signaled when skeleton data is available
	nextSkeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	// Open a skeleton stream to receive skeleton data
	hr = nuiSensor->NuiSkeletonTrackingEnable(nextSkeletonEvent, 0);
	if(FAILED(hr))
	{
		std::cerr << "Failed to initalize skeleton tracking" << std::endl;
		return false;
	}

	// Create an event that will be signaled when depth data is available
    nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    // Open a depth image stream to receive depth frames
    hr = nuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, DEPTH_RESOLUTION, 0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if(FAILED(hr))
	{
		std::cerr << "Failed to open depth stream" << std::endl;
		return false;
	}

	// Create an event that will be signaled when color data is available
    nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    // Open a color image stream to receive color frames
    hr = nuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, COLOR_RESOLUTION, 0, 2, nextColorFrameEvent, &colorStreamHandle);
	if(FAILED(hr))
	{
		std::cerr << "Failed to open color stream" << std::endl;
		return false;
	}

	player = getPlayer();
	paddle = getPaddle();

	return SUCCEEDED(hr);
}


/// <summary>
/// Main processing function
/// </summary>
void KinectManager::update()
{             
	if(!nuiSensor)
		return;

	/*HANDLE events[] = {nextSkeletonEvent, nextDepthFrameEvent, nextColorFrameEvent};

	DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, false, INFINITE);

    if (WAIT_OBJECT_0 == ret)
	{
		processSkeleton();
		processDepth();
		processColor();
		processColorToDepth();
	}*/

	bool needToDraw = false;

	if ( WAIT_OBJECT_0 == WaitForSingleObject(nextSkeletonEvent, 0) )
	{
		if ( SUCCEEDED(processSkeleton()) )
        {
            needToDraw = true;
        }
	}

	if ( WAIT_OBJECT_0 == WaitForSingleObject(nextDepthFrameEvent, 0) )
    {
		if ( SUCCEEDED(processDepth()) )
        {
            needToDraw = true;
        }
    }

	if ( WAIT_OBJECT_0 == WaitForSingleObject(nextColorFrameEvent, 0) )
    {
		if ( SUCCEEDED(processColor()) )
        {
            needToDraw = true;
        }
    }
	
	if (needToDraw)
    {
		processColorToDepth();
		computePaddlePositionAndNormal();
	}
}


/// <summary>
/// Handle new skeleton data
/// </summary>
HRESULT KinectManager::processSkeleton()
{
	HRESULT hr = S_OK;
	NUI_SKELETON_FRAME skeletonFrame = {0};
	
	// Attempt to get the skeleton frame
	hr = nuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if(FAILED(hr))
		return hr;

	// Some smoothing with little latency (defaults).
	// Only filters out small jitters.
	// Good for gesture recognition in games.
	static const NUI_TRANSFORM_SMOOTH_PARAMETERS defaultParams = {0.5f, 0.5f, 0.5f, 0.05f, 0.04f};

	// Smoothed with some latency.
	// Filters out medium jitters.
	// Good for a menu system that needs to be smooth but
	// doesn't need the reduced latency as much as gesture recognition does.
	static const NUI_TRANSFORM_SMOOTH_PARAMETERS somewhatLatentParams = {0.5f, 0.1f, 0.5f, 0.1f, 0.1f};

	// Very smooth, but with a lot of latency.
	// Filters out large jitters.
	// Good for situations where smooth data is absolutely required
	// and latency is not an issue.
	static const NUI_TRANSFORM_SMOOTH_PARAMETERS verySmoothParams = {0.7f, 0.3f, 1.0f, 1.0f, 1.0f};

	// Smooth out the skeleton data
	nuiSensor->NuiTransformSmooth(&skeletonFrame, &defaultParams);

	// Track closest skeleton
    //float closestDistance = 10000.0f;
	float mostCentered = 10000.0f;
    int closestID = -1;

	for (int i = 0; i < NUI_SKELETON_COUNT; ++i)
	{
		const NUI_SKELETON_DATA &skeleton = skeletonFrame.SkeletonData[i];
		
		if (skeleton.eTrackingState == NUI_SKELETON_TRACKED)
		{
			//if (skeleton.Position.z < closestDistance)
			if (abs(skeleton.Position.x) < mostCentered)
			{
				closestID = i;
				//closestDistance = skeleton.Position.z;
				mostCentered = abs(skeleton.Position.x);
			}
		}
	}

	if (closestID >= 0)
	{
		const NUI_SKELETON_DATA &skeleton = skeletonFrame.SkeletonData[closestID];

		// Set player's joints
		Vector4 head = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
		player->setHead(head.x, head.y, head.z);

		Vector4 hip = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER];
		player->setHip(hip.x, hip.y, hip.z);

		Vector4 handLeft = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
		player->setHand(handLeft.x, handLeft.y, handLeft.z, true);

		Vector4 handRight = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
		player->setHand(handRight.x, handRight.y, handRight.z, false);

		Vector4 elbowLeft = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
		player->setElbow(elbowLeft.x, elbowLeft.y, elbowLeft.z, true);

		Vector4 elbowRight = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
		player->setElbow(elbowRight.x, elbowRight.y, elbowRight.z, false);

		Vector4 footLeft = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT];
		player->setFoot(footLeft.x, footLeft.y, footLeft.z, true);

		Vector4 footRight = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT];
		player->setFoot(footRight.x, footRight.y, footRight.z, false);
	}

	return hr;
}


/// <summary>
/// Handle new depth data
/// </summary>
HRESULT KinectManager::processDepth()
{
    HRESULT hr = S_OK;
    NUI_IMAGE_FRAME depthFrame;

    // Attempt to get the depth frame
    hr = nuiSensor->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
    if (FAILED(hr))
	{	
		std::cerr << "Process depth failed to get next frame" << std::endl;
        return hr;
	}

    BOOL nearMode;
    INuiFrameTexture * texture = depthFrame.pFrameTexture;

	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	NUI_LOCKED_RECT lockedRect;
    texture->LockRect(0, &lockedRect, NULL, 0);

    if (lockedRect.Pitch != 0)
    {
		// Copy depth data for cameras alignment
		memcpy(depthPixels, lockedRect.pBits, lockedRect.size);

		if(SHOW_DEPTH_VIEWER)
		{
			// Get the min and max reliable depth for the current frame
			//int minDepth = (nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
			//int maxDepth = (nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

			for (int i = 0; i < depthWidth*depthHeight; i++)
			{
				// Get depth in millimeters
				USHORT depth = depthPixels[i] >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

				// To convert to a byte, we're discarding the most-significant
				// rather than least-significant bits.
				// We're preserving detail, although the intensity will "wrap."
				// Values outside the reliable depth range are mapped to 0 (black).

				// Note: Using conditionals in this loop could degrade performance.
				// Consider using a lookup table instead when writing production code.
				//BYTE intensity = static_cast<BYTE>(depth >= minDepth && depth <= maxDepth ? depth % 256 : 0);
				BYTE intensity = (BYTE) depth % 256;

				// Draw a grayscale image of the depth
				QRgb col = qRgb(intensity, intensity, intensity);
				depthImg.setPixel(i % depthWidth, i / depthWidth, col);
			}

			// Get position of player's hand
			Point3d playHand = player->getPlayingHand();
			Vector4 hand = {playHand.x(), playHand.y(), playHand.z(), 1};
			// Convert to screen coords
			D2D1_POINT_2F handScreen = convertSkeletonToScreen(hand, depthWidth*2, depthHeight*2);
			float hsx = handScreen.x;
			float hsy = handScreen.y;

			// Draw circle at hand's position
			drawCircle(&depthImg, hsx, hsy, 10, 3);

			// Get position of player's elbow
			Point3d playElbow = player->getPlayingElbow();
			Vector4 elbow = {playElbow.x(), playElbow.y(), playElbow.z(), 1};
			// Convert to screen coords
			D2D1_POINT_2F elbowScreen = convertSkeletonToScreen(elbow, depthWidth*2, depthHeight*2);
			float esx = elbowScreen.x;
			float esy = elbowScreen.y;

			// Draw circle at hand's position
			drawCircle(&depthImg, esx, esy, 5, 3);

			// Display frame
			QPixmap pixmap = QPixmap::fromImage(depthImg);
			depthDisplayLabel.setPixmap(pixmap.scaled(colorWidth, colorHeight, Qt::KeepAspectRatio));
			depthDisplayLabel.adjustSize();
		}
    }

    // We're done with the texture so unlock it
    texture->UnlockRect(0);

	// Release the frame
    nuiSensor->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame);
	
	// Get of x, y coordinates for color in depth space
    // This will allow us to later compensate for the differences in location, angle, etc between the depth and color cameras
    nuiSensor->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
		COLOR_RESOLUTION,
        DEPTH_RESOLUTION,
        depthWidth*depthHeight,
        depthPixels,
        depthWidth*depthHeight*2,
        colorCoordinates);

	return hr;
}


/// <summary>
/// Handle new color data
/// </summary>
HRESULT KinectManager::processColor()
{
    HRESULT hr = S_OK;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the color frame
    hr = nuiSensor->NuiImageStreamGetNextFrame(colorStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
	{
		std::cerr << "Process color failed to get next frame" << std::endl;
        return hr;
	}

    INuiFrameTexture *texture = imageFrame.pFrameTexture;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    NUI_LOCKED_RECT lockedRect;
	texture->LockRect(0, &lockedRect, NULL, 0);

    // Make sure we've received valid data
    if (lockedRect.Pitch != 0)
    {
		// Copy color data for cameras alignment
		memcpy(colorPixels, lockedRect.pBits, lockedRect.size);
		
		if(SHOW_COLOR_VIEWER)
		{
			for(int i = 0; i < colorWidth; ++i)
			{
				for(int j = 0; j < colorHeight; ++j)
				{
					// Calculate index into color array
					LONG colorIndex = (j * colorWidth + i) * BYTES_PER_PIXEL;

					//const byte a=colorPixels[colorIndex + 3];
					const byte r = colorPixels[colorIndex + 2];
					const byte g = colorPixels[colorIndex + 1];
					const byte b = colorPixels[colorIndex];

					QRgb col = qRgb(r, g, b);

					colorImg.setPixel(i, j, col);
				}
			}

			// Get position of player's hand
			Point3d playHand = player->getPlayingHand();
			Vector4 hand = {playHand.x(), playHand.y(), playHand.z(), 1};
			// Convert to screen coords
			D2D1_POINT_2F handScreen = convertSkeletonToScreen(hand, colorWidth*2, colorHeight*2);
			float hsx = handScreen.x;
			float hsy = handScreen.y;

			// Draw circle at hand's position
			drawCircle(&colorImg, hsx, hsy, 20, 6);

			// Get position of player's elbow
			Point3d playElbow = player->getPlayingElbow();
			Vector4 elbow = {playElbow.x(), playElbow.y(), playElbow.z(), 1};
			// Convert to screen coords
			D2D1_POINT_2F elbowScreen = convertSkeletonToScreen(elbow, colorWidth*2, colorHeight*2);
			float esx = elbowScreen.x;
			float esy = elbowScreen.y;

			// Draw circle at hand's position
			drawCircle(&colorImg, esx, esy, 10, 6);
		
			// Display frame
			colorDisplayLabel.setPixmap(QPixmap::fromImage(colorImg));
			colorDisplayLabel.adjustSize();
		}
    }

    // We're done with the texture so unlock it
    texture->UnlockRect(0);

    // Release the frame
    nuiSensor->NuiImageStreamReleaseFrame(colorStreamHandle, &imageFrame);
	
	return hr;
}


/// <summary>
/// Handle new color with depth data
/// </summary>
void KinectManager::processColorToDepth()
{
	// Clear the vector containing the paddle's pixels of the previous frame
	paddlePixels.clear();

	// Set background color
	reconImg.fill(Qt::black);

	// Get position of player's hand
	Point3d playHand = player->getPlayingHand();
	Vector4 hand = {playHand.x(), playHand.y(), playHand.z(), 1};
	USHORT handDepth = playHand.z()*1000; // in mm
	// Convert to screen coords
	D2D1_POINT_2F handScreen = convertSkeletonToScreen(hand, colorWidth*2, colorHeight*2);
	float hsx = handScreen.x;
	float hsy = handScreen.y;

	//LONG yMin = hsy > PADDLE_PIXEL_RADIUS ? hsy-PADDLE_PIXEL_RADIUS : 0;
	//LONG yMax = hsy+PADDLE_PIXEL_RADIUS < colorHeight ? hsy+PADDLE_PIXEL_RADIUS : colorHeight;
	//LONG xMin = hsx > PADDLE_PIXEL_RADIUS ? hsx-PADDLE_PIXEL_RADIUS : 0;
	//LONG xMax = hsx+PADDLE_PIXEL_RADIUS < colorWidth ? hsx+PADDLE_PIXEL_RADIUS : colorWidth;

    for (LONG y = 0; y < colorHeight; ++y)
    {
        for (LONG x = 0; x < colorWidth; ++x)
        {
			// Calculate index into depth array
            int depthIndex = x/colorToDepthDivisor + y/colorToDepthDivisor * depthWidth;

            USHORT depthPixel  = depthPixels[depthIndex];
			USHORT depthValue = depthPixel >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
            USHORT player = NuiDepthPixelToPlayerIndex(depthPixel);

            // If we're tracking a player for the current pixel, draw from the color camera
            if ( player > 0 )
            {
                // Retrieve the depth to color mapping for the current depth pixel
                LONG colorInDepthX = colorCoordinates[depthIndex * 2];
                LONG colorInDepthY = colorCoordinates[depthIndex * 2 + 1];

                // Make sure the depth pixel maps to a valid point in color space and is near the hand
                if (   colorInDepthX >= 0 && colorInDepthX < colorWidth && colorInDepthY >= 0 && colorInDepthY < colorHeight
					&& isInsideCircle(x, y, hsx, hsy, PADDLE_PIXEL_RADIUS)
					&& depthValue >= handDepth - PADDLE_MAX_DEPTH_MM && depthValue <= handDepth + PADDLE_MAX_DEPTH_MM 
					)
                {
                    // Calculate index into color array
                    LONG colorIndex = (colorInDepthX + colorInDepthY * colorWidth) * BYTES_PER_PIXEL;

					//const byte a = colorPixels[colorIndex + 3];
					const byte r = colorPixels[colorIndex + 2];
					const byte g = colorPixels[colorIndex + 1];
					const byte b = colorPixels[colorIndex];

					QColor col = QColor(r, g, b);
					if(isPixelOnPaddle(col))
					{
						Vector4 paddlePixel = NuiTransformDepthImageToSkeleton(x/colorToDepthDivisor, y/colorToDepthDivisor, depthPixel, DEPTH_RESOLUTION);
						paddlePixels.push_back(paddlePixel);

						if (SHOW_RECON_VIEWER)
						{
							reconImg.setPixel(x, y, col.rgb());
						}
						if (WRITE_TO_FILE)
						{
							ostringstream oss;
							oss << paddlePixel.x << " " << paddlePixel.y << " " << paddlePixel.z << " ";
							outStream << oss.str();
						}
					}

					//if(x == hsx && y == hsy) cout << col.hue() << " " << col.saturation() << " " << col.value() << " || " << col.red() << " " << col.green() << " " << col.blue() <<endl;
                }
            }
		}
	}
	if (WRITE_TO_FILE) outStream << std::endl;

	if (SHOW_RECON_VIEWER)
	{
		// Draw circle at hand's position
		drawCircle(&reconImg, hsx, hsy, PADDLE_PIXEL_RADIUS, 2);

		// Display frame
		reconDisplayLabel.setPixmap(QPixmap::fromImage(reconImg));
		reconDisplayLabel.adjustSize();
	}
}

void KinectManager::computePaddlePositionAndNormal()
{
	// Handle the case where there are too few pixels on the paddle
	//cout << paddlePixels.size() << endl;
	if(paddlePixels.size() < 100)	
	{
		paddle->updatePosition(player->getPlayingHand());
		paddle->updateNormal(Point3d(0,0,-1));
		paddle->updateElbowPos(player->getPlayingElbow());
		return;
	}

	typedef std::pair<double, int> myPair;
	typedef std::vector<myPair> PermutationIndices;

	// Convert vector of 3d points into matrix
	MatrixX3d mat(paddlePixels.size(), 3);
	for(size_t i = 0; i < paddlePixels.size(); i++) 
	{
		mat(i,0) = paddlePixels[i].x;
		mat(i,1) = paddlePixels[i].y;
		mat(i,2) = paddlePixels[i].z;
	}

	Vector3d mean = mat.colwise().mean();
	MatrixX3d mat_center = mat.rowwise() - mat.colwise().mean();
	MatrixX3d cov = mat_center.adjoint() * mat_center;
	
	SelfAdjointEigenSolver<MatrixX3d> eig(cov);
	Vector3d eigval = eig.eigenvalues().real();
	MatrixX3d eigvec = eig.eigenvectors().real();

	// Find the smallest eigenvalue and set the respective eigenvector as the normal of the paddle 
	PermutationIndices pi;
	for (int i = 0 ; i < 3; i++)
		pi.push_back(std::make_pair(eigval(i), i));
	sort(pi.begin(), pi.end());
	int idx_smallest_eigval = pi[0].second;
	double smallestEigval = pi[0].first;
	int idx_second_smallest_eigval = pi[1].second;
	double secondSmallestEigval = pi[1].first;

	// Set default paddle if points are too disperse to form a plane
	//cout << secondSmallestEigval - smallestEigval << endl;
	if(secondSmallestEigval - smallestEigval < 0.5)
	{
		//paddle->updatePosition(player->getPlayingHand());
		paddle->updatePosition(Point3d(mean.x(), mean.y(), mean.z()));
		paddle->updateNormal(Point3d(0,0,-1));
		paddle->updateElbowPos(player->getPlayingElbow());
		return;
	}

	Vector3d paddleNormal = eigvec.col(idx_smallest_eigval);
	paddleNormal.normalize();

	// Invert the normal to face the kinect, if necessary
	if(paddleNormal.dot(Vector3d(0,0,-1)) < 0)
		paddleNormal *= -1;

	// Correct the normal w.r.t the sensor's angle
	//LONG cameraAngle = 0;
	//nuiSensor->NuiCameraElevationGetAngle(&cameraAngle);
	//cout << cameraAngle << endl;

	if (SHOW_RECON_VIEWER)
	{
		// Draw the mean of the points
		Vector4 meanV4 = {mean(0), mean(1), mean(2), 1};
		D2D1_POINT_2F meanScreen = convertSkeletonToScreen(meanV4, colorWidth*2, colorHeight*2);
		drawCircle(&reconImg, meanScreen.x, meanScreen.y, 3, 5);

		// Draw the normal of the paddle's plane
		Vector4 eigV4 = {mean(0)+paddleNormal(0)/5, mean(1)+paddleNormal(1)/5, mean(2)+paddleNormal(2)/5, 1};
		D2D1_POINT_2F eigScreen = convertSkeletonToScreen(eigV4, colorWidth*2, colorHeight*2);
		drawLine(&reconImg, meanScreen.x, meanScreen.y, eigScreen.x, eigScreen.y, 2);

		reconDisplayLabel.setPixmap(QPixmap::fromImage(reconImg));
	}

	// Set paddle position and normal
	//paddle->updatePosition(Point3d(floor(mean.x()*1000)/1000.0, floor(mean.y()*1000)/1000.0, floor(mean.z()*1000)/1000.0));
	paddle->updatePosition(player->getPlayingHand());
	paddle->updateNormal(Point3d(paddleNormal.x(), paddleNormal.y(), paddleNormal.z()));
	paddle->updateElbowPos(player->getPlayingElbow());
}

/// <summary>
/// Converts a skeleton point to screen space
/// </summary>
/// <param name="skeletonPoint">skeleton point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F KinectManager::convertSkeletonToScreen(Vector4 skeletonPoint, int width, int height)
{
    LONG x, y;
    USHORT depth;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);

    float screenPointX = static_cast<float>(x * width) / X_RES;
    float screenPointY = static_cast<float>(y * height) / Y_RES;

    return D2D1::Point2F(screenPointX, screenPointY);
}


/// <summary>
/// Check whether x and y are inside a circle at center cx and cy with radius r
/// </summary>
bool KinectManager::isInsideCircle(float x, float y, float cx, float cy, float r)
{
	float dx = x<cx ? cx-x : x-cx;
	float dy = y<cy ? cy-y : y-cy;

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