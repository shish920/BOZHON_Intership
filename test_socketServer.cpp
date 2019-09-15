#pragma comment(lib,"ws2_32.lib")
#include<opencv2/opencv.hpp>
#include <stdio.h>
#include <winsock2.h>
using namespace std;

int main(void)
{
	//初始化WSA
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);

	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = PF_INET;
	sin.sin_addr.s_addr = inet_addr("192.168.5.200");  //具体的IP地址
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	::bind(slisten, (SOCKADDR*)& sin, sizeof(SOCKADDR));

	//进入监听状态
	listen(slisten, 20);


	//if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	//{
	//	printf("bind error !");
	//}

	////开始监听
	//if (listen(slisten, 5) == SOCKET_ERROR)
	//{
	//	printf("listen error !");
	//	return 0;
	//}

	//循环接收数据
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	std::cout << "bxguqshicxoew" << std::endl;
	printf("等待连接...\n");
	sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
	if (sClient == INVALID_SOCKET)
	{
		printf("accept error !");
		//continue;
	}
	printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

	while (true)
	{
		//接收数据
		int ret = recv(sClient, revData, 255, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		//发送数据
		std::string   sendData = "1,35.818901, 270.692871,8888\r\n";
		const char *Str = sendData.c_str();
		send(sClient, Str, strlen(Str), 0);

		closesocket(sClient);

		for (int i = 0; i < 255; i++)
		{
			revData[i] = 0;
		}

		int n = 50000;
		if (!n--) { break; }

		/*if (getchar()=='\n')
		{
		break;
		}*/

	}
	closesocket(slisten);
	WSACleanup();
	return 0;
}
