
// Include files to use the pylon API.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define USE_SOCKET
//#define USE_UPCAMERA
#include <stdio.h>
#include <winsock2.h>
#include<E:/Basler/Development/include/pylon/PylonIncludes.h>
#include<E:/Basler/Development/include/pylon/PylonGUI.h>
#include<opencv2/opencv.hpp>
#include<chrono>
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll

//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif


// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using std::cout.
//using namespace std;
using namespace cv;
// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 1000;

int main(int argc, char* argv[])
{
	// The exit code of the sample application.
	int exitCode = 0;

	// Before using any pylon methods, the pylon runtime must be initialized. 
	PylonInitialize();


	float transMat[2][3] = { {0,0.84,-142},{0.5, 0, 182} };//ת������
	//float inputPoint[2] = { 0 };//����ͼ������
	float outputPoint[2] = { 0 };//�����������ϵ����


	try
	{
		// Get the transport layer factory.
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		// Get all attached devices and exit application if no device is found.
		DeviceInfoList_t devices;
		if (tlFactory.EnumerateDevices(devices) == 0)
		{
			throw RUNTIME_EXCEPTION("No camera present.");
		}

		// Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
		CInstantCameraArray cameras(2);

		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
		cameras[0].Attach(tlFactory.CreateDevice(devices[0]));//�����
		//waitKey(1000);
		cameras[1].Attach(tlFactory.CreateDevice(devices[1]));//�����

		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration d = t2 - t1;
		std::cout << "attach��ʱ"<<std::chrono::duration_cast<std::chrono::milliseconds>(d).count()<< "ms"<< std::endl;
		std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
		cameras[0].StartGrabbing(GrabStrategy_LatestImageOnly);
		cameras[1].StartGrabbing(GrabStrategy_LatestImageOnly);
		std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration d1 = t4 - t3;
		std::cout << "Grab��ʱ" << std::chrono::duration_cast<std::chrono::milliseconds>(d1).count() << "ms" << std::endl;


		//CGrabResultPtr ptrGrabResult;
		CGrabResultPtr ptrGrabResult0;//�����ץȡ���
		CGrabResultPtr ptrGrabResult1;//�����ץȡ���
		Mat pylonImg;//�������ȡ��matͼƬ
		Mat pylonImg1;//�������ȡ��matͼƬ
		SOCKET slisten;//����
		WSADATA wsaData;//����
		SOCKET sClient;//�ͻ���
		char revData[255];//���յ�����
		int imgWidth;//ͼ���
		int imgHeight;//ͼ���
		int num = 1;//���ڱ���ͼƬ�ļ���
		bool isUpCamera = false;//������ʹ������������������һ��ϵͳ״̬
		bool isReady = false;//�����ʹ�õı������ж��Ƿ�׼�������е�ֽ���ͨѶ
		bool isConnect = false;//������Ƿ����е�����ӳɹ�
		bool isTimeToExit = false;//�Ƿ���������е����̣�����Ӧ���˳���
		std::vector<std::vector<Point>> contours;//��¼findContoursѰ�ҵ�����

		// Camera.StopGrabbing() is called automatically by the RetrieveResult() method
		// when c_countOfImagesToGrab images have been retrieved.
		//while (1)
		for (uint32_t i = 0; i < c_countOfImagesToGrab; ++i)
		{

#ifdef USE_SOCKET//ͨѶ����
			if (!isConnect)
			{
				//��ʼ��WSA
				WORD sockVersion = MAKEWORD(2, 2);

				if (WSAStartup(sockVersion, &wsaData) != 0)
				{
					return 0;
				}

				//�����׽���
				slisten = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (slisten == INVALID_SOCKET)
				{
					printf("socket error !");
					return 0;
				}

				//��IP�Ͷ˿�
				sockaddr_in sin;
				sin.sin_family = PF_INET;
				sin.sin_addr.s_addr = inet_addr("192.168.5.200");  //�����IP��ַ
				sin.sin_port = htons(8888);
				sin.sin_addr.S_un.S_addr = INADDR_ANY;

				bind(slisten, (SOCKADDR*)& sin, sizeof(SOCKADDR));

				//�������״̬
				listen(slisten, 20);
				// ѭ����������

				sockaddr_in remoteAddr;
				int nAddrlen = sizeof(remoteAddr);


				printf("�ȴ�����...\n");
				sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
				if (sClient == INVALID_SOCKET)
				{
					printf("accept error !");
					//continue;
				}
				printf("���ܵ�һ�����ӣ�%s \r\n", inet_ntoa(remoteAddr.sin_addr));

			}
#endif // USE_SOCKET

#ifdef USE_UPCAMERA
			 //Wait for an image and then retrieve it. A timeout of 5000 ms is used.
			std::chrono::steady_clock::time_point t7 = std::chrono::steady_clock::now();
			cameras[1].RetrieveResult(5000, ptrGrabResult1, TimeoutHandling_ThrowException);
			std::chrono::steady_clock::time_point t8 = std::chrono::steady_clock::now();
			std::chrono::steady_clock::duration d4 = t8 - t7;
			std::cout << "�����retrive��ʱ" << std::chrono::duration_cast<std::chrono::milliseconds>(d4).count() << "ms" << std::endl;


			intptr_t cameraContextValue1 = ptrGrabResult1->GetCameraContext();
			//std::cout << cameraContextValue << std::endl;
			//Pylon::DisplayImage(cameraContextValue, ptrGrabResult);
			// Image grabbed successfully?
			//if (ptrGrabResult->GrabSucceeded())


			if (ptrGrabResult1->GrabSucceeded() && isUpCamera)//�����
			{
				std::chrono::steady_clock::time_point t9 = std::chrono::steady_clock::now();

				const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult1->GetBuffer();
				imgHeight = (int)ptrGrabResult1->GetHeight();
				imgWidth = (int)ptrGrabResult1->GetWidth();
				pylonImg = Mat(imgHeight, imgWidth, CV_8UC1, (void*)pImageBuffer);
				resize(pylonImg, pylonImg, Size(imgWidth / 3, imgHeight / 3));
				imshow("srcImg", pylonImg);
				//if (waitKey(30) == 'c')
				//	imwrite(std::to_string(num++) + ".bmp", pylonImg);
				threshold(pylonImg, pylonImg, 100, 255, CV_THRESH_TRIANGLE);
				findContours(pylonImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

				std::chrono::steady_clock::time_point t10 = std::chrono::steady_clock::now();
				std::chrono::steady_clock::duration d5 = t10 - t9;
				std::cout << "�����ת��Mat��Ѱ��������ʱ" << std::chrono::duration_cast<std::chrono::milliseconds>(d5).count() << "ms" << std::endl;

				Rect rect;
				Mat rectImg;
				Point centerPoint;
				Point TriCenterPoint;
				Mat showImg = Mat::zeros(Size(pylonImg.cols, pylonImg.rows), CV_8UC1);
				for (int n = 0; n < contours.size(); ++n)
				{
					rect = boundingRect(contours[n]);
					centerPoint = (rect.tl() + rect.br()) / 2;
					
					TriCenterPoint.x = centerPoint.x;
					TriCenterPoint.y = rect.tl().y * 2 / 3 + rect.br().y * 1 / 3;
					//std::cout << "x:" << centerPoint.x << "  y:" << centerPoint.y << std::endl;
					if (centerPoint.x< pylonImg.cols / 4 || centerPoint.x > pylonImg.cols * 0.75 ||
						centerPoint.y < pylonImg.rows / 3 || centerPoint.y >  pylonImg.rows * 0.67 ||
						rect.area() < 300)
					{
						contours.erase(contours.begin() + n);
						n--;
					}
					else
					{
						
						circle(showImg, centerPoint, 2, Scalar(255), 3);
						putText(showImg, "(" + std::to_string(centerPoint.x) + "," + std::to_string(centerPoint.y) + ")",
							centerPoint, FONT_HERSHEY_SIMPLEX, 1, Scalar(255));
						outputPoint[0] = transMat[0][0] * centerPoint.x + transMat[0][1] * centerPoint.y + transMat[0][2];
						outputPoint[1] = transMat[1][0] * centerPoint.x + transMat[1][1] * centerPoint.y + transMat[1][2];

						//circle(showImg, TriCenterPoint, 2, Scalar(255), 3);
						//putText(showImg, "(" + std::to_string(TriCenterPoint.x) + "," + std::to_string(TriCenterPoint.y) + ")",
						//	TriCenterPoint, FONT_HERSHEY_SIMPLEX, 1, Scalar(255));

						//outputPoint[0] = transMat[0][0] * TriCenterPoint.x + transMat[0][1] * TriCenterPoint.y + transMat[0][2];
						//outputPoint[1] = transMat[1][0] * TriCenterPoint.x + transMat[1][1] * TriCenterPoint.y + transMat[1][2];

						std::cout << "x:" << outputPoint[0] << "   y:" << outputPoint[1] << std::endl;

						//��������
						if (outputPoint[0] != 0 || outputPoint[1] != 0)
						{
							std::string  str1 = std::to_string(outputPoint[0]);
							std::string  str2 = std::to_string(outputPoint[1]);
							std::string sendString = "1," + str1 + "," + str2 + ",8888\r\n";
							const char *Str = sendString.c_str();
							//send(sClient, Str, strlen(Str), 0);
							outputPoint[0] = 0;
							outputPoint[1] = 0;
							//isUpCamera = false;
						}


					}
					drawContours(showImg, contours, 0, Scalar(255));
					imshow("result", showImg);
						

				}
				if (waitKey(30) == 'p' /*|| outputPoint[0] !=0*/)
				{
					break;
				}
				else if (waitKey(30) == 'q')
				{
					isUpCamera = false;
					continue;
				}
			}

#endif // USE_UPCAMERA

#ifdef USE_SOCKET
			if (!isReady)
			{
				int ret = recv(sClient, revData, 255, 0);
				if (ret > 0)
				{
					revData[ret] = 0x00;
					printf(revData);
					isReady = true;
				}
				else
				{
					if (waitKey(30) == 'p')
					{
						break;
					}
					//count++;
					continue;
				}
			}
			//��������

#endif // USE_SOCKET
			std::chrono::steady_clock::time_point t5 = std::chrono::steady_clock::now();
			cameras[0].RetrieveResult(5000, ptrGrabResult0, TimeoutHandling_ThrowException);
			std::chrono::steady_clock::time_point t6 = std::chrono::steady_clock::now();
			std::chrono::steady_clock::duration d2 = t6 - t5;
			std::cout << "�����retrive��ʱ" << std::chrono::duration_cast<std::chrono::milliseconds>(d2).count() << "ms" << std::endl;

			intptr_t cameraContextValue0 = ptrGrabResult0->GetCameraContext();

			if (ptrGrabResult0->GrabSucceeded() && !isUpCamera)
			{

				std::chrono::steady_clock::time_point t7 = std::chrono::steady_clock::now();

				const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult0->GetBuffer();
				imgHeight = (int)ptrGrabResult0->GetHeight();
				imgWidth = (int)ptrGrabResult0->GetWidth();
				pylonImg1 = Mat(imgHeight, imgWidth, CV_8UC1, (void*)pImageBuffer);
				resize(pylonImg1, pylonImg1, Size(imgWidth / 3, imgHeight / 3));
				//threshold(pylonImg1, pylonImg1, 100, 255, CV_THRESH_TRIANGLE);
				imshow("srcImg1", pylonImg1);
				threshold(pylonImg1, pylonImg1, 100, 255, CV_THRESH_OTSU);
				findContours(pylonImg1, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

				std::chrono::steady_clock::time_point t8 = std::chrono::steady_clock::now();
				std::chrono::steady_clock::duration d3 = t8 - t7;
				std::cout << "�����ת��Mat��Ѱ��������ʱ" << std::chrono::duration_cast<std::chrono::milliseconds>(d3).count() << "ms" << std::endl;

				int flag = 0;//2���� 3���������� 4������ 5����� 6������ >10Բ
				Rect2f rect;
				RotatedRect rotateRect;
				std::vector<Point> point;
				Point2f corners[4];
				Mat rectImg;
				Point2f centerPoint;
				Point2f imgCenterPoint = Point(pylonImg1.cols / 2, pylonImg1.rows / 2);
				Mat showImg1 = Mat::zeros(Size(pylonImg1.cols, pylonImg1.rows), CV_8UC1);
				float theta = 0;
				//Mat showImg2 = pylonImg1.clone();
				float angle = 0;

				for (int n = 0; n < contours.size(); ++n)
				{
					rotateRect = minAreaRect(contours[n]);
					rotateRect.points(corners);
					
					rect = boundingRect(contours[n]);
					//rectangle(showImg1, rect, Scalar(255));
					//angle = rotateRect.angle;
					//for (int i = 0; i < 3; i++)
					//{
					//	
					//	line(showImg1, corners[i], corners[i + 1], Scalar(255));
					//	line(showImg1, corners[i], corners[i + 1], Scalar(255));
					//													 
					//	if (i == 2)										 
					//	{												 
					//		line(showImg1, corners[3], corners[0], Scalar(255));
					//		line(showImg1, corners[3], corners[0], Scalar(255));
					//	}
					//}
					centerPoint = (rect.tl() + rect.br()) / 2;
					//std::cout << "x:" << centerPoint.x << "  y:" << centerPoint.y << std::endl;
					if (centerPoint.x< pylonImg1.cols / 4 || centerPoint.x > pylonImg1.cols * 0.75 ||
						centerPoint.y < pylonImg1.rows / 4 || centerPoint.y >  pylonImg1.rows * 0.75 ||
						rect.area() < 1000)
					{
						contours.erase(contours.begin() + n);
						n--;
					}
					else
					{
						int epsilon = 0.01*arcLength(contours[n], true);
						approxPolyDP(contours[n], point, epsilon, true);
						std::cout << "�ߵ�����Ϊ"<<point.size() << std::endl;
						sort(point.begin(), point.end(), [](const Point &p1, const Point &p2) {return p1.y < p2.y; });
						theta = atan2f(point[1].y - point[0].y, point[1].x - point[0].x) / 3.14159 * 180;
						if (theta < -90)theta += 180;
						else if (theta > 90)theta -= 180;
						std::cout << "���߳��Ϳ�:   " << point[1].y - point[0].y << "��" << point[1].x - point[0].x <<  std::endl;

						angle = -theta;
						//std::cout << point << point.size() << std::endl;
						std::cout << "������ο�ͳ�:   " <<rect.width << "��" << rect.height << std::endl;
						flag = point.size();
						if (flag == 4 && (rect.width / rect.height > 1.34 || rect.width / rect.height < 0.75))
						{
							flag = 2;
						}
						if (flag == 0) std::cout << "δ��⵽ͼ��" << std::endl;
						else if (flag == 2)
						{
							std::cout << "��⵽����" << std::endl;
						}
						else if (flag == 3)
						{
							std::cout << "��⵽������" << std::endl;
							centerPoint.y = rect.tl().y * 2 / 3 + rect.br().y * 1 / 3;
							isTimeToExit = true;
						}
						else if (flag == 4)
						{
							std::cout << "��⵽������" << std::endl;
						}
						else if (flag == 5)
						{
							centerPoint.y = rect.tl().y * 0.5528 + rect.br().y * 0.4472;
							std::cout << "��⵽�����" << std::endl;

						}
						else if (flag == 6)
						{ 
							std::cout << "��⵽������" << std::endl; 
						}
						else if (flag > 10)
						{
							std::cout << "��⵽Բ" << std::endl;
							angle = 0;
						}



						circle(showImg1, centerPoint, 2, Scalar(255), 3);
						putText(showImg1, "(" + std::to_string((int)centerPoint.x) + "," + std::to_string((int)centerPoint.y) + ")",
							centerPoint, FONT_HERSHEY_SIMPLEX, 1, Scalar(255));
						outputPoint[0] = (centerPoint.x - imgCenterPoint.x) / (4.25);
						outputPoint[1] = (centerPoint.y - imgCenterPoint.y) / (4.25);
						//angle -= 2.8;//����Լ���ƫ��
						std::cout << "x: " << outputPoint[0] << "   y:  " << outputPoint[1] << std::endl;
						std::cout << "angle:  " << angle <<"  img angle:  "<<rotateRect.angle << std::endl << std::endl;
					}

					drawContours(showImg1, contours, 0, Scalar(255));
					imshow("result", showImg1);


					//��������
					if (outputPoint[0] != 0 || outputPoint[1] != 0)
					{
						std::string  str1 = std::to_string(outputPoint[0]);
						std::string  str2 = std::to_string(outputPoint[1]);
						std::string sendString = "0," + str1 + "," + str2 + "," + std::to_string(angle) + ",6666\r\n";
						const char *Str = sendString.c_str();
#ifdef USE_SOCKET
						send(sClient, Str, strlen(Str), 0);
#endif // USE_SOCKET
						outputPoint[0] = 0;	
						outputPoint[1] = 0;
						angle = 0;
						flag = 0;
						isReady = false;
						//break;
						//isUpCamera = true;
						imwrite(std::to_string(num++) + ".bmp", showImg1);
					}
				}
				
				if (waitKey(30) == 'p')
				{
					break;
				}
				else if (waitKey(30) == 'q')
				{
					//isUpCamera = true;
					continue;
				}

			}
#ifdef USE_SOCKET
			if (!isConnect)
			{

				isConnect = true;
			}
			else
			{
				closesocket(slisten);
				WSACleanup();
				isConnect = false;
				if (isTimeToExit) break;
			}
#endif // USE_SOCKET
		}
	}
		
	catch (const GenericException &e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
		exitCode = 1;

	}


	//// Releases all pylon resources. 


	PylonTerminate();
	system("pause");
	return exitCode;
}
