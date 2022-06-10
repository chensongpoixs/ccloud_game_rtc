/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_LOG_DEFINE_H
#define C_LOG_DEFINE_H

namespace chen
{
	enum ELogLevelType
	{
		ELogLevel_None = 0,
		ELogLevel_System,
		ELogLevel_Fatal,
		ELogLevel_Error,
		ELogLevel_Warn,
		ELogLevel_Info,
		ELogLevel_Debug,
		ELogLevel_Num,
	};

	enum ELogNameType
	{
		ELogName_None = 0,
		ELogName_Date,
		ELogName_DateTime,
		ELogName_AutoDate,
	};


	enum ELogStorageType
	{
		ELogStoragePost				= 0X01, 
		ELogStorageFile				= 0X02, 
		ELogStorageFilePost			= 0X03, 
		ELogStorageScreen			= 0X04, 
		ELogStorageScreenPost		= 0X05, 
		ELogStorageScreenFile		= 0X06, 
		ELogStorageScreenFilePost	= 0X07, 
	};

}//namespace chen



#endif  // !#define C_LOG_DEFINE_H_
