// FingerPrintTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FingerPrintTest.h"
#include "ZxLog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

LONG m_lUserID = -1;
LONG m_lRemoteCfgHandle = -1;
BOOL m_bGetCardCfgFinish = FALSE;
BOOL m_bGetFingerCfgFinish = FALSE;
BOOL m_bSetCardCfgFinish = FALSE;
BOOL m_bSetFingerCfgFinish = FALSE;

using namespace std;

/*==================
获取卡参数回调
====================*/

void CALLBACK GetCardCfgCallback(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
	LOG_INFO("GetCardCfgCallback --> dwType = " << dwType << ", dwBufLen = " << dwBufLen);
	//cout << "GetCardCfgCallback --> dwType = " << dwType << ", dwBufLen = " << dwBufLen << endl;

	if(dwType == NET_SDK_CALLBACK_TYPE_DATA)
	{
		//cout << "GetCardCfgCallback >>> NET_SDK_CALLBACK_TYPE_DATA, dwBufLen = " << dwBufLen << endl;
		LOG_INFO("GetCardCfgCallback --> NET_SDK_CALLBACK_TYPE_DATA, dwBufLen = " << dwBufLen);

		LPNET_DVR_CARD_CFG lpCardCfg = new NET_DVR_CARD_CFG;

		memcpy(lpCardCfg, lpBuffer, dwBufLen);
/*
		printf("byCardNo = %s\n", lpCardCfg->byCardNo);
		printf("byCardType = %d\n", lpCardCfg->byCardType);
		printf("byCardPassword = %s\n", lpCardCfg->byCardPassword);
		printf("byCardValid = %d\n", lpCardCfg->byCardValid);
		printf("byLeaderCard = %d\n", lpCardCfg->byLeaderCard);
		printf("dwBelongGroup = %d\n", lpCardCfg->dwBelongGroup);
		printf("dwDoorRight = %d\n", lpCardCfg->dwDoorRight);
		printf("byCardRightPlan[0][0] = %d\n", lpCardCfg->byCardRightPlan[0][0]);
		printf("byCardRightPlan[0][1] = %d\n", lpCardCfg->byCardRightPlan[0][1]);
		printf("byCardRightPlan[0][2] = %d\n", lpCardCfg->byCardRightPlan[0][2]);
		printf("byCardRightPlan[0][3] = %d\n", lpCardCfg->byCardRightPlan[0][3]);
*/
		LOG_INFO("NET_DVR_CARD_CFG:" \
			<< "\nbyCardNo = " << lpCardCfg->byCardNo \
			<< "\nbyCardType = " << (int)lpCardCfg->byCardType \
			<< "\nbyCardPassword = " << lpCardCfg->byCardPassword \
			<< "\nbyCardValid = " << (int)lpCardCfg->byCardValid \
			<< "\nbyLeaderCard = " << (int)lpCardCfg->byLeaderCard \
			<< "\ndwBelongGroup = " << (int)lpCardCfg->dwBelongGroup \
			<< "\ndwDoorRight = " << (int)lpCardCfg->dwDoorRight \
			<< "\nbyCardRightPlan[0][0] = " << (int)lpCardCfg->byCardRightPlan[0][0] \
			<< "\nbyCardRightPlan[0][1] = " << (int)lpCardCfg->byCardRightPlan[0][1] \
			<< "\nbyCardRightPlan[0][2] = " << (int)lpCardCfg->byCardRightPlan[0][2] \
			<< "\nbyCardRightPlan[0][3] = " << (int)lpCardCfg->byCardRightPlan[0][3] \
			<< "\n");

		delete lpCardCfg;
	}
	else if (dwType == NET_SDK_CALLBACK_TYPE_STATUS)
	{
		//cout << "GetCardCfgCallback >>> NET_SDK_CALLBACK_TYPE_STATUS, dwBufLen = " << dwBufLen << endl;
		LOG_INFO("GetCardCfgCallback --> NET_SDK_CALLBACK_TYPE_STATUS, dwBufLen = " << dwBufLen);

		DWORD dwStatus = *(DWORD*)lpBuffer;
		//printf("dwStatus = %d\n", dwStatus);
		LOG_INFO("dwStatus = " << dwStatus);

		if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
		{
			m_bGetCardCfgFinish = TRUE;
			//cout << "dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS" << endl;
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS");
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
		{
			m_bGetCardCfgFinish = TRUE;
			//cout << "dwStatus == NET_SDK_CALLBACK_STATUS_FAILED" << endl;
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_FAILED");

			char szCardNo[ACS_CARD_NO_LEN] = "\0";
			DWORD dwErrorCode = *(DWORD*)((char*)lpBuffer + 4);
			strncpy(szCardNo, (char*)(lpBuffer)+8, ACS_CARD_NO_LEN);
			//printf("Error code: %d, Card No: %s\n", dwErrorCode, szCardNo);
			LOG_INFO("Error code: " << dwErrorCode << ", Card No: " << szCardNo);
		}

	}

}

/*==================
获取指纹参数回调
====================*/

void CALLBACK GetFingerCfgCallback(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
	LOG_INFO("GetFingerCfgCallback --> dwType = " << dwType << ", dwBufLen = " << dwBufLen);

	if (dwType == NET_SDK_CALLBACK_TYPE_DATA)
	{
		//cout << "GetFingerPrintCfgCallback --> NET_SDK_CALLBACK_TYPE_DATA, dwBufLen = " << dwBufLen << endl;
		LOG_INFO("GetFingerPrintCfgCallback --> NET_SDK_CALLBACK_TYPE_DATA, dwBufLen = " << dwBufLen);

		char szHex[8] = {0};
		string strCardNo = "";
		string strData = "";

		LPNET_DVR_FINGER_PRINT_CFG lpFingerPrintCfg = new NET_DVR_FINGER_PRINT_CFG;
		cout << "sizeof(NET_DVR_FINGER_PRINT_CFG) = " << sizeof(NET_DVR_FINGER_PRINT_CFG) << endl;
		cout << "dwBufLen = " << dwBufLen << endl;
		memcpy(lpFingerPrintCfg, lpBuffer, dwBufLen);
/*		
		printf("byCardNo = %s\n", lpFingerPrintCfg->byCardNo);
		printf("byCardNo:\n");
*/
		for (int i = 0; i < 32; i++)
		{
			//printf("%02x ", lpFingerPrintCfg->byCardNo[i]);
			sprintf(szHex, "%02x ", lpFingerPrintCfg->byCardNo[i]);
			strCardNo += szHex;
		}
/*
		printf("\n\n");
		printf("dwFingerPrintLen = %d\n", lpFingerPrintCfg->dwFingerPrintLen);
		printf("byFingerPrintID = %d\n", lpFingerPrintCfg->byFingerPrintID);
		printf("byFingerType = %d\n", lpFingerPrintCfg->byFingerType);
		printf("byEnableCardReader[0] = %d\n", lpFingerPrintCfg->byEnableCardReader[0]);
		printf("byEnableCardReader[1] = %d\n", lpFingerPrintCfg->byEnableCardReader[1]);
		printf("byEnableCardReader[2] = %d\n", lpFingerPrintCfg->byEnableCardReader[2]);
		printf("byEnableCardReader[3] = %d\n", lpFingerPrintCfg->byEnableCardReader[3]);
		printf("byFingerData:\n");
*/
		int nDataLen = (int)lpFingerPrintCfg->dwFingerPrintLen;
		for (int i = 0; i < nDataLen; i++)
		{
			//printf("%02x ", lpFingerPrintCfg->byFingerData[i]);
			sprintf(szHex, "%02x ", lpFingerPrintCfg->byFingerData[i]);
			strData += szHex;
		}
//		printf("\n\n");

		LOG_INFO("NET_DVR_FINGER_PRINT_CFG:\n" \
			<< "byCardNo: \n" << strCardNo \
			<< "\ndwFingerPrintLen = " << (int)lpFingerPrintCfg->dwFingerPrintLen \
			<< "\nbyFingerPrintID = " << (int)lpFingerPrintCfg->byFingerPrintID \
			<< "\nbyFingerType = " << (int)lpFingerPrintCfg->byFingerType \
			<< "\nbyEnableCardReader[0] = " << (int)lpFingerPrintCfg->byEnableCardReader[0] \
			<< "\nbyEnableCardReader[1] = " << (int)lpFingerPrintCfg->byEnableCardReader[1] \
			<< "\nbyEnableCardReader[2] = " << (int)lpFingerPrintCfg->byEnableCardReader[2] \
			<< "\nbyEnableCardReader[3] = " << (int)lpFingerPrintCfg->byEnableCardReader[3] \
			<< "\nbyFingerData:\n" << strData \
			<< "\n\n");

		delete lpFingerPrintCfg;

	}
	else if (dwType == NET_SDK_CALLBACK_TYPE_STATUS)
	{
		cout << "GetFingerPrintCfgCallback --> NET_SDK_CALLBACK_TYPE_STATUS, dwBufLen = " << dwBufLen << endl;

		DWORD dwStatus = *(DWORD*)lpBuffer;
		//printf("dwStatus = %d\n", dwStatus);
		LOG_INFO("dwStatus = " << dwStatus);

		if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
		{
			m_bGetFingerCfgFinish = TRUE;
			//cout << "dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS" << endl;
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS");
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
		{
			m_bGetFingerCfgFinish = TRUE;
			//cout << "dwStatus == NET_SDK_CALLBACK_STATUS_FAILED" << endl;
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_FAILED");

			char szCardNo[ACS_CARD_NO_LEN + 1] = "\0";
			DWORD dwErrorCode = *(DWORD*)((char*)lpBuffer + 4);
			strncpy(szCardNo, (char*)(lpBuffer)+8, ACS_CARD_NO_LEN);
			//printf("GetFingerPrintCfg FAILED, Error code %d, Card No %s\n", dwErrorCode, szCardNo);
			LOG_INFO("Error code: " << dwErrorCode << ", Card No: " << szCardNo);

		}
	}
}

/*==================
设置卡参数回调
====================*/

void CALLBACK SetCardCfgCallback(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
	LOG_INFO("SetCardCfgCallback --> dwType = " << dwType << ", dwBufLen = " << dwBufLen);

	if (dwType != NET_SDK_CALLBACK_TYPE_STATUS)
	{
		LOG_INFO("SetCardCfgCallback --> dwType != NET_SDK_CALLBACK_TYPE_STATUS !");
	}
	else
	{
		DWORD dwStatus = *(DWORD*)lpBuffer;
		LOG_INFO("dwStatus = " << dwStatus);

		if (dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING");

			char szCardNo[ACS_CARD_NO_LEN + 1] = "\0";
			strncpy(szCardNo, (char*)lpBuffer + 4, ACS_CARD_NO_LEN);
			LOG_INFO("SetCard processing, cardno: " << szCardNo);
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_FAILED");

			//char szCardNo[ACS_CARD_NO_LEN + 1] = "\0";
			//DWORD dwErrCode = *((DWORD*)lpBuffer + 1);
			//strncpy(szCardNo, (char*)lpBuffer + 8, ACS_CARD_NO_LEN);
			//printf("SetCard failed, error: %d, cardno: %s\n", dwErrCode, szCardNo);
			//printf("SetCard failed!!!\n");
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS");
			m_bSetCardCfgFinish = TRUE;
			//printf("SetCard success\n");
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION");
			m_bSetCardCfgFinish = TRUE;
			//printf("SetCard exception!\n");
		}
	}
}

/*==================
设置指纹参数回调
====================*/

void CALLBACK SetFingerCfgCallback(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
	LOG_INFO("SetFingerPrintCfgCallback >>> dwType = " << dwType << ", dwBufLen = " << dwBufLen);

	if (dwType != NET_SDK_CALLBACK_TYPE_STATUS)
	{
		LOG_INFO("dwType != NET_SDK_CALLBACK_TYPE_STATUS !!!");
	}
	else
	{
		DWORD dwStatus = *(DWORD*)lpBuffer;
		LOG_INFO("dwStatus = " << dwStatus);

		if (dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING");
			char szCardNo[ACS_CARD_NO_LEN + 1] = "\0";
			strncpy(szCardNo, (char*)lpBuffer + 4, ACS_CARD_NO_LEN);
			LOG_INFO("SetFinger processing, cardno: " << szCardNo);
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_FAILED !!!");
			LOG_INFO("Error code: " << NET_DVR_GetLastError());
			m_bSetFingerCfgFinish = TRUE;

			//char szCardNo[ACS_CARD_NO_LEN + 1] = "\0";
			//DWORD dwErrCode = *((DWORD*)lpBuffer + 1);
			//strncpy(szCardNo, (char*)lpBuffer + 8, ACS_CARD_NO_LEN);
			//printf("SetFingerPrintCfg failed, error: %d, cardno: %s\n", dwErrCode, szCardNo);
			//printf("SetFingerPrintCfg failed!!!\n");
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS");
			m_bSetFingerCfgFinish = TRUE;
		}
		else if (dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION)
		{
			LOG_INFO("dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION !!!");
			m_bSetFingerCfgFinish = TRUE;
		}

	}
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		CZxLogManager::Init("testlog.properties");

		//cout << "DS-K2601 Finger Print Demo Application" << endl;
		LOG_INFO("==================================================");
		LOG_INFO("DS-K2601 Finger Print Demo Application Starting...");
		LOG_INFO("==================================================");

		string strIP = "10.72.7.210";
		WORD wPort = (WORD)8000;
		string strUserName = "admin";
		string strPassword = "admin123";
		NET_DVR_DEVICEINFO_V30 struDeviceInfo = {0};

		if(!NET_DVR_Init())
		{
			LOG_ERROR("NET_DVR_Init >>> false, error code: " << NET_DVR_GetLastError());
			nRetCode = 1;
			return nRetCode;
		}

		char cmdkey;
		LONG nRtn = -1;
		BOOL bRtn = FALSE;
		BOOL bCleanup = FALSE;

		//设置连接时间与重连时间
		//NET_DVR_SetConnectTime(2000, 1);
		// 每隔10s重连
		//NET_DVR_SetReconnect(10000, true);
		DWORD dwLevel = 3;
		char *szLogDir = "\log";
		NET_DVR_SetLogToFile(dwLevel, szLogDir, FALSE);

		for(;;)
		{
			cout << "\nTest Options: \
				\n0 - exit \
				\n1 - Login \
				\n2 - Get card config \
				\n3 - Get finger config \
				\n4 - Set card config \
				\n5 - Set finger config \
				\n6 - delete finger config \
				\n7 - Get card right config \
				\n8 - Logout \
				\nPlease input: ";
			cin >> cmdkey;

			if(cmdkey == '0')
			{
				LOG_INFO("===> Test option: 0 - exit");
				if(m_lUserID >= 0)
				{
					bRtn = NET_DVR_Logout(m_lUserID);
					//cout << "NET_DVR_Logout >>> " << (bRtn ? "true" : "false") << endl;
					LOG_INFO("NET_DVR_Logout >>> " << (bRtn ? "true" : "false"));
				}
				break;
			}
			else if(cmdkey == '8')
			{
				LOG_INFO("===> Test option: 8 - Logout");
				if(m_lUserID >= 0)
				{
					bRtn = NET_DVR_Logout(m_lUserID);
					LOG_INFO("NET_DVR_Logout >>> " << (bRtn ? "true" : "false"));
					m_lUserID = -1;
				}
			}
			else if(cmdkey == '1')
			{
				LOG_INFO("===> Test option: 1 - Login");

				char szDeviceip[129];
				int nPort = 0;
				char szUserName[64];
				char szPassword[64];

				cout << "Device IP: ";
				cin >> szDeviceip;

				cout << "Device Port: ";
				cin >> nPort;

				cout << "User Name: ";
				cin >> szUserName;

				cout << "Password: ";
				cin >> szPassword;

				LOG_INFO("Device ip = " << szDeviceip << ", port = " << nPort << ", username = " << szUserName << ", password = " << szPassword);
				m_lUserID = NET_DVR_Login_V30(szDeviceip, (WORD)nPort, szUserName, szPassword, &struDeviceInfo);
/*
				LOG_INFO("Device ip = " << strIP.c_str() << ", port = " << wPort << ", username = " << strUserName.c_str() << ", password = " << strPassword.c_str());
				m_lUserID = NET_DVR_Login_V30((char*)strIP.c_str(), wPort, (char*)strUserName.c_str(), (char*)strPassword.c_str(), &struDeviceInfo);
*/
				if(m_lUserID < 0)
				{
					LOG_ERROR("NET_DVR_Login_V30 >>> " << m_lUserID << "\nerror code: " << NET_DVR_GetLastError());
					nRetCode = 1;
					break;
				}
				else
				{
					LOG_INFO("NET_DVR_Login_V30 >>> lUserID = " << m_lUserID);
					LOG_INFO("NET_DVR_DEVICEINFO_V30:\nsSerialNumber: " << struDeviceInfo.sSerialNumber << "\n\n");
					LOG_INFO("Login success!");
				}
			}
			else if(cmdkey == '2')
			{
				LOG_INFO("===> Test option: 2 - Get card config");

				if(m_lUserID < 0)
				{
					LOG_INFO("You must login device first!");
				}
				else
				{
					if (m_lRemoteCfgHandle >= 0)
					{
						bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
						LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
						if(bRtn)
						{
							m_lRemoteCfgHandle = -1;
						}
						else
						{
							LOG_INFO("Error code: " << NET_DVR_GetLastError());
						}
					}

					char szCardNo[ACS_CARD_NO_LEN];
					
					cout << "Card No(10 digits): ";
					cin >> szCardNo;

					NET_DVR_CARD_CFG_COND struCond = {0};
					struCond.dwSize = sizeof(struCond);
					struCond.dwCardNum = 1;
					struCond.byCheckCardNo = 1;

					NET_DVR_CARD_CFG_SEND_DATA struSendData = {0};
					struSendData.dwSize = sizeof(struSendData);
					strncpy((char*)struSendData.byCardNo, szCardNo, ACS_CARD_NO_LEN);

					LOG_INFO("NET_DVR_GET_CARD_CFG - 2116, CardNo = " << struSendData.byCardNo);

					m_bGetCardCfgFinish = FALSE;

					m_lRemoteCfgHandle = NET_DVR_StartRemoteConfig(m_lUserID, NET_DVR_GET_CARD_CFG, &struCond, sizeof(struCond), GetCardCfgCallback, NULL);
					LOG_INFO("NET_DVR_StartRemoteConfig >>> lHandle = " << m_lRemoteCfgHandle);

					if (m_lRemoteCfgHandle < 0)
					{
						LOG_INFO("Error code: " << NET_DVR_GetLastError());
					}
					else
					{
						bRtn = NET_DVR_SendRemoteConfig(m_lRemoteCfgHandle, ENUM_ACS_SEND_DATA, (char*)&struSendData, sizeof(struSendData));
						LOG_INFO("NET_DVR_SendRemoteConfig >>> " << (bRtn ? "true" : "false"));

						if (bRtn)
						{
							Sleep(3000);

							if (m_bGetCardCfgFinish)
							{
								bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
								LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
								if(bRtn)
								{
									m_lRemoteCfgHandle = -1;
								}
								else
								{
									LOG_INFO("Error code: " << NET_DVR_GetLastError());
								}
								
							}
							
							LOG_INFO("NET_DVR_GET_CARD_CFG : 2116 >>> finish!");
						}
						else
						{
							LOG_INFO("Error code: " << NET_DVR_GetLastError());
						}

					}
				}
			}
			else if(cmdkey == '3')
			{
				LOG_INFO("===> Test option: 3 - Get finger config");

				if(m_lUserID < 0)
				{
					LOG_INFO("You must login device first!");
				}
				else
				{
					if (m_lRemoteCfgHandle >= 0)
					{
						bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
						LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
						if(bRtn)
						{
							m_lRemoteCfgHandle = -1;
						}
						else
						{
							LOG_INFO("Error code: " << NET_DVR_GetLastError());
						}
					}

					char szCardNo[ACS_CARD_NO_LEN];
					cout << "Card No (10 digits): ";
					cin >> szCardNo;

					NET_DVR_FINGER_PRINT_INFO_COND struCond = { 0 };
					struCond.dwSize = sizeof(struCond);
					strncpy((char*)struCond.byCardNo, szCardNo, ACS_CARD_NO_LEN);
					struCond.dwFingerPrintNum = 1;
					struCond.byEnableCardReader[0] = 1;
					struCond.byFingerPrintID = 0xff; //1

					LOG_INFO("NET_DVR_GET_FINGERPRINT_CFG - 2150, CardNo = " << struCond.byCardNo);

					m_bGetFingerCfgFinish = FALSE;

					m_lRemoteCfgHandle = NET_DVR_StartRemoteConfig(m_lUserID, NET_DVR_GET_FINGERPRINT_CFG, &struCond, sizeof(struCond), GetFingerCfgCallback, NULL);
					LOG_INFO("NET_DVR_StartRemoteConfig >>> lHandle = " << m_lRemoteCfgHandle);

					if (m_lRemoteCfgHandle < 0)
					{
						LOG_INFO("Error code: " << NET_DVR_GetLastError());
					}
					else
					{
						Sleep(5000);

						if (m_bGetFingerCfgFinish)
						{
							bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
							LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
							if(bRtn)
							{
								m_lRemoteCfgHandle = -1;
							}
							else
							{
								LOG_INFO("Error code: " << NET_DVR_GetLastError());
							}

						}

						LOG_INFO("NET_DVR_GET_FINGERPRINT_CFG : 2150 >>> finish!");
					}
				}

			}
			else if (cmdkey == '4')
			{
				LOG_INFO("===> Test option: 4 - Set card config");

				if(m_lUserID < 0)
				{
					LOG_INFO("You must login device first!");
				}
				else
				{
					if (m_lRemoteCfgHandle >= 0)
					{
						bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
						LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
						if(bRtn)
						{
							m_lRemoteCfgHandle = -1;
						}
						else
						{
							LOG_INFO("Error code: " << NET_DVR_GetLastError());
						}
					}

					char szCardNo[ACS_CARD_NO_LEN];
					cout << "Card No (10 digits): ";
					cin >> szCardNo;

					char szCardPassword[9];
					cout << "Card password (8 digits max): ";
					cin >> szCardPassword;

					int nCardValid = 1;
					cout << "set card(1) or delete card(0): ";
					cin >> nCardValid;

					NET_DVR_CARD_CFG_COND struCond = { 0 };
					struCond.dwSize = sizeof(struCond);
					struCond.dwCardNum = 1;
					struCond.byCheckCardNo = 1;
					//struCond.wLocalControllerID = 0;

					LOG_INFO("NET_DVR_SET_CARD_CFG - 2117, CardNo = " << szCardNo);

					m_bSetCardCfgFinish = FALSE;

					m_lRemoteCfgHandle = NET_DVR_StartRemoteConfig(m_lUserID, NET_DVR_SET_CARD_CFG, &struCond, sizeof(struCond), SetCardCfgCallback, NULL);
					LOG_INFO("NET_DVR_StartRemoteConfig >>> lHandle = " << m_lRemoteCfgHandle);
					if (m_lRemoteCfgHandle < 0)
					{
						LOG_INFO("Error code: " << NET_DVR_GetLastError());
					}
					else
					{
						LPNET_DVR_CARD_CFG lpCardCfg = new NET_DVR_CARD_CFG;
						lpCardCfg->dwSize = sizeof(NET_DVR_CARD_CFG);
						lpCardCfg->dwModifyParamType = 0x0000003ff;
						strncpy((char*)lpCardCfg->byCardNo, szCardNo, ACS_CARD_NO_LEN);
						if (nCardValid == 0)
						{
							lpCardCfg->byCardValid = 0;
						}
						else
						{
							lpCardCfg->byCardValid = 1;
						}
						lpCardCfg->byCardType = 1;
						lpCardCfg->byLeaderCard = 0;
						lpCardCfg->dwDoorRight = 0x00000001;
						//lpCardCfg->dwBelongGroup = 0x00000001;
						lpCardCfg->dwBelongGroup = 0;
						strncpy((char*)lpCardCfg->byCardPassword, szCardPassword, CARD_PASSWORD_LEN);
						lpCardCfg->byCardRightPlan[0][0] = 1;
						lpCardCfg->dwMaxSwipeTime = 0;
						lpCardCfg->dwSwipeTime = 0;
						lpCardCfg->struValid.byEnable = 1;
						lpCardCfg->struValid.struBeginTime.wYear = 2019;
						lpCardCfg->struValid.struBeginTime.byMonth = 1;
						lpCardCfg->struValid.struBeginTime.byDay = 1;
						lpCardCfg->struValid.struBeginTime.byHour = 0;
						lpCardCfg->struValid.struBeginTime.byMinute = 0;
						lpCardCfg->struValid.struBeginTime.bySecond = 0;
						lpCardCfg->struValid.struEndTime.wYear = 2024;
						lpCardCfg->struValid.struEndTime.byMonth = 1;
						lpCardCfg->struValid.struEndTime.byDay = 1;
						lpCardCfg->struValid.struEndTime.byHour = 0;
						lpCardCfg->struValid.struEndTime.byMinute = 0;
						lpCardCfg->struValid.struEndTime.bySecond = 0;

						bRtn = NET_DVR_SendRemoteConfig(m_lRemoteCfgHandle, ENUM_ACS_SEND_DATA, (char*)lpCardCfg, sizeof(NET_DVR_CARD_CFG));
						LOG_INFO("NET_DVR_SendRemoteConfig >>> " << (bRtn ? "true" : "false"));

						if (bRtn)
						{
							Sleep(3000);

							if (m_bSetCardCfgFinish)
							{
								bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
								LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
								if(bRtn)
								{
									m_lRemoteCfgHandle = -1;
								}
								else
								{
									LOG_INFO("Error code: " << NET_DVR_GetLastError());
								}
							}

							LOG_INFO("NET_DVR_SET_CARD_CFG : 2117 >>> finish!");
						}
						else
						{
							LOG_INFO("Error code: " << NET_DVR_GetLastError());
						}

						delete lpCardCfg;
						lpCardCfg = NULL;
					}
				}
			}
			else if (cmdkey == '5')
			{
				LOG_INFO("===> Test option: 5 - Set finger config");

				if(m_lUserID < 0)
				{
					LOG_INFO("You must login device first!");
				}
				else
				{
					if (m_lRemoteCfgHandle >= 0)
					{
						bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
						LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
						if(bRtn)
						{
							m_lRemoteCfgHandle = -1;
						}
						else
						{
							LOG_INFO("Error code: " << NET_DVR_GetLastError());
						}
					}

					char szCardNo[ACS_CARD_NO_LEN];
					cout << "Card No (10 digits): ";
					cin >> szCardNo;

					int nFingerID = 0;
					cout << "Finger print id (1 - 10): ";
					cin >> nFingerID;
					printf("nFingerID = %d\n", nFingerID);

					FILE* fp = NULL;

					char szFingerFile[256];
					sprintf(szFingerFile, "fp%d.bin", nFingerID);
					printf("szFingerFile = %s\n", szFingerFile);
					
					fp = fopen(szFingerFile, "rb");
					if (fp == NULL)
					{
						LOG_INFO("open fingerprint file " << szFingerFile << " failed!!!");
						continue;
					}
					else
					{
						fclose(fp);
						LOG_INFO("open fingerprint file " << szFingerFile << " success!");
					}

					NET_DVR_FINGER_PRINT_INFO_COND struCond = { 0 };
					struCond.dwSize = sizeof(struCond);
					struCond.dwFingerPrintNum = 1;
					struCond.byCallbackMode = 0;
/*
					strncpy((char*)struCond.byCardNo, szCardNo, ACS_CARD_NO_LEN);
					struCond.dwFingerPrintNum = 1;
					struCond.byEnableCardReader[0] = 1;
					struCond.byFingerPrintID = (BYTE)nFingerID;
*/
					LOG_INFO("NET_DVR_SET_FINGERPRINT_CFG : 2151, Card No: " << szCardNo << ", Finger ID: " << nFingerID);

					m_bSetFingerCfgFinish = FALSE;

					m_lRemoteCfgHandle = NET_DVR_StartRemoteConfig(m_lUserID, NET_DVR_SET_FINGERPRINT_CFG, &struCond, sizeof(struCond), SetFingerCfgCallback, NULL);
					LOG_INFO("NET_DVR_StartRemoteConfig >>> lHandle = " << m_lRemoteCfgHandle);
					if (m_lRemoteCfgHandle < 0)
					{
						LOG_INFO("Error code: " << NET_DVR_GetLastError());
					}
					else
					{
						LPNET_DVR_FINGER_PRINT_CFG lpFingerCfg = new NET_DVR_FINGER_PRINT_CFG;
						memset(lpFingerCfg, 0, sizeof(NET_DVR_FINGER_PRINT_CFG));
						lpFingerCfg->dwSize = sizeof(NET_DVR_FINGER_PRINT_CFG);
						strncpy((char*)lpFingerCfg->byCardNo, szCardNo, ACS_CARD_NO_LEN);
						lpFingerCfg->byEnableCardReader[0] = 1;
						lpFingerCfg->byFingerType = 0;
						lpFingerCfg->byFingerPrintID = (BYTE)nFingerID;
						lpFingerCfg->dwFingerPrintLen = 512;

						//fp = fopen("fp3.bin", "rb");
						fp = fopen(szFingerFile, "rb");
						if (fp == NULL)
						{
							LOG_INFO("open fingerprint file " << szFingerFile << " failed!!!");
						}
						else
						{
							fseek(fp, 0L, SEEK_END);
							int len = ftell(fp);
							LOG_INFO("file length = " << len);
							rewind(fp);
							fread(lpFingerCfg->byFingerData, 1, 512, fp);
							fclose(fp);

							string strFingerData = "";
							char szHex[4];
							for (int i = 0; i < 512; i++)
							{
								sprintf(szHex, "%02x ", lpFingerCfg->byFingerData[i]);
								strFingerData += szHex;
							}
							LOG_INFO("byFingerData:\n" << strFingerData << "\n\n");
							
							bRtn = NET_DVR_SendRemoteConfig(m_lRemoteCfgHandle, ENUM_ACS_SEND_DATA, (char*)lpFingerCfg, sizeof(NET_DVR_FINGER_PRINT_CFG));
							LOG_INFO("NET_DVR_SendRemoteConfig >>> " << (bRtn ? "true" : "false"));
							
							if (bRtn)
							{

								Sleep(5000);

								if (m_bSetFingerCfgFinish)
								{
									bRtn = NET_DVR_StopRemoteConfig(m_lRemoteCfgHandle);
									LOG_INFO("NET_DVR_StopRemoteConfig >>> " << (bRtn ? "true" : "false"));
									
									if(bRtn)
									{
										m_lRemoteCfgHandle = -1;
									}
									else
									{
										LOG_INFO("Error code: " << NET_DVR_GetLastError());
									}
								}
								
								LOG_INFO("NET_DVR_SET_FINGERPRINT_CFG : 2151 >>> finish!");
							}
							else
							{
								LOG_INFO("Error code: " << NET_DVR_GetLastError());
							}
						}

						delete lpFingerCfg;
						lpFingerCfg = NULL;
					}
				}
			}
			else if (cmdkey == '6')
			{
				LOG_INFO("===> Test option: 6 - delete finger config");

				if(m_lUserID < 0)
				{
					LOG_INFO("You must login device first!");
				}
				else
				{
					char szCardNo[ACS_CARD_NO_LEN];
					cout << "Card No (10 digits): ";
					cin >> szCardNo;

					NET_DVR_FINGER_PRINT_INFO_CTRL struFPInfoCtrl = { 0 };
					struFPInfoCtrl.dwSize = sizeof(struFPInfoCtrl);
					struFPInfoCtrl.byMode = 0;
					memset(&struFPInfoCtrl.struProcessMode.struByCard, 0, sizeof(NET_DVR_FINGER_PRINT_BYCARD));
					strncpy((char*)struFPInfoCtrl.struProcessMode.struByCard.byCardNo, szCardNo, ACS_CARD_NO_LEN);
					struFPInfoCtrl.struProcessMode.struByCard.byEnableCardReader[0] = 1;
					struFPInfoCtrl.struProcessMode.struByCard.byFingerPrintID[0] = 1;

					LOG_INFO("NET_DVR_DEL_FINGERPRINT_CFG : 2152, Card No: " << szCardNo);


					bRtn = NET_DVR_RemoteControl(m_lUserID, NET_DVR_DEL_FINGERPRINT_CFG, &struFPInfoCtrl, sizeof(struFPInfoCtrl));
					LOG_INFO("NET_DVR_RemoteControl >>> " << (bRtn ? "true" : "false"));
					if (bRtn)
					{
						LOG_INFO("NET_DVR_DEL_FINGERPRINT_CFG : 2152 >>> success!");
					}
					else
					{
						LOG_INFO("NET_DVR_DEL_FINGERPRINT_CFG : 2152 >>> failed!!!");
						LOG_INFO("Error code: " << NET_DVR_GetLastError());
					}
				}
			}
			else if (cmdkey == '7')
			{
				LOG_INFO("===> Test option: 7 - Get card right config");

				if(m_lUserID < 0)
				{
					LOG_INFO("You must login device first!");
				}
				else
				{
					DWORD dwReturned = 0;
					LONG lChannel = 1;

					NET_DVR_PLAN_TEMPLATE struPlanTemp = { 0 };
					struPlanTemp.dwSize = sizeof(struPlanTemp);

					bRtn = NET_DVR_GetDVRConfig(m_lUserID, NET_DVR_GET_CARD_RIGHT_PLAN_TEMPLATE, lChannel, &struPlanTemp, sizeof(struPlanTemp), &dwReturned);
					LOG_INFO("NET_DVR_GetDVRConfig >>> " << (bRtn ? "true" : "false"));
					if (bRtn)
					{
/*
						printf("dwReturned = %d\n", dwReturned);
						printf("dwSize = %d\n", struPlanTemp.dwSize);
						printf("byTemplateName = %s\n", struPlanTemp.byTemplateName);
						for (int i = 0; i < 32; i++)
						{
							printf("%02x ", struPlanTemp.byTemplateName[i]);
						}
						printf("\n");
						printf("byEnable = %d\n", struPlanTemp.byEnable);
						printf("dwWeekPlanNo = %d\n", struPlanTemp.dwWeekPlanNo);
						printf("dwHolidayGroupNo[0] = %d\n", struPlanTemp.dwHolidayGroupNo[0]);
						printf("dwHolidayGroupNo[1] = %d\n", struPlanTemp.dwHolidayGroupNo[1]);
						printf("dwHolidayGroupNo[2] = %d\n", struPlanTemp.dwHolidayGroupNo[2]);
						printf("dwHolidayGroupNo[3] = %d\n", struPlanTemp.dwHolidayGroupNo[3]);
						printf("NET_DVR_GetDVRConfig -- NET_DVR_GET_CARD_RIGHT_PLAN_TEMPLATE success!\n");
*/
						string strTemplateName = "";
						char szHex[3];
						for (int i = 0; i < 32; i++)
						{
							sprintf(szHex, "%02x", struPlanTemp.byTemplateName[i]);
							strTemplateName += szHex;
						}

						LOG_INFO("NET_DVR_PLAN_TEMPLATE:" \
							<< "\ndwReturned = " << dwReturned \
							<< "\ndwSize = " << struPlanTemp.dwSize \
							<< "\nbyTemplateName = " << strTemplateName \
							<< "\nbyEnable = " << (int)struPlanTemp.byEnable \
							<< "\ndwWeekPlanNo = " << struPlanTemp.dwWeekPlanNo \
							<< "\ndwHolidayGroupNo[0] = " << struPlanTemp.dwHolidayGroupNo[0] \
							<< "\ndwHolidayGroupNo[1] = " << struPlanTemp.dwHolidayGroupNo[1] \
							<< "\ndwHolidayGroupNo[2] = " << struPlanTemp.dwHolidayGroupNo[2] \
							<< "\ndwHolidayGroupNo[3] = " << struPlanTemp.dwHolidayGroupNo[3] \
							<< "\n\n");
						LOG_INFO("NET_DVR_GET_CARD_RIGHT_PLAN_TEMPLATE : 2138 >>> success!");
					}
					else
					{
						LOG_INFO("NET_DVR_GET_CARD_RIGHT_PLAN_TEMPLATE : 2138 >>> failed!!!");
						LOG_INFO("Error code: " << NET_DVR_GetLastError());
					}
				}

			}
			else
			{
				LOG_INFO("unknown test option: " << cmdkey);
			}

		}

		bRtn = NET_DVR_Cleanup();
		LOG_INFO("NET_DVR_Cleanup >>> " << (bRtn ? "true" : "false"));

		LOG_INFO("DS-K2601 Finger Print Demo Application exit!");
		CZxLogManager::DeInit();
	}

	return nRetCode;
}
