#include "ccloud_rendering.h"
#include "ccfg.h"
#include "httplib.h"
#include "chttp_mgr.h"
#include "rtc_base/logging.h"
#include "rtc_base/log_sinks.h"
#include "peerConnectionUtils.hpp"
#include "cinput_device.h"
namespace chen {
	
	ccloud_rendering::~ccloud_rendering() {}
 
	bool ccloud_rendering::init(const char * config_name)
	{
		bool init =  g_cfg.init(config_name);
		if (!init)
		{
			RTC_LOG(LS_ERROR) << "config init failed !!!" << config_name;
			return false;
		}
		if (!g_http_mgr.init())
		{
			RTC_LOG(LS_ERROR) << " http_mgr init failed  !!!";
			return false;
		}

		if (!chen::g_input_device_mgr.init())
		{
			RTC_LOG(LS_ERROR) << "input_device mgr init failed !!!";
			return false;
		}

		auto logLevel = mediasoupclient::Logger::LogLevel::LOG_DEBUG;
		mediasoupclient::Logger::SetLogLevel(logLevel);
		mediasoupclient::Logger::SetDefaultHandler();
		/*日志默认是输出到标准错误的，不调用也是可以的。*/
		//rtc::LogMessage::SetLogToStderr(true);
		//rtc::LogMessage::LogToDebug(rtc::LS_NONE);
		/*创建日志文件*/
		// static rtc::FileRotatingLogSink m_frls("./log", "webrtc_log", 10240, 1);
		//m_frls.Init();

		///*将日志输出到日志文件中，接收WARNING及以上级别的日志。*/
		//rtc::LogMessage::AddLogToStream(&m_frls, rtc::WARNING);
	
		// Initilize mediasoupclient.
		mediasoupclient::Initialize();
		
		return true;
	}
	bool ccloud_rendering::Loop()
	{

		std::string ws_url = "ws://" + g_cfg.get_string(ECI_MediaSoup_Host) + ":" + std::to_string(g_cfg.get_int32(ECI_MediaSoup_Http_Port)) + "/?roomId=" + g_cfg.get_string(ECI_Room_Name) + "&peerId=" + g_cfg.get_string(ECI_Client_Name);//ws://127.0.0.1:8888/?roomId=chensong&peerId=xiqhlyrn", "http://127.0.0.1:8888")
		std::string origin = "http://" + g_cfg.get_string(ECI_MediaSoup_Host) + ":" + std::to_string(g_cfg.get_int32(ECI_MediaSoup_Http_Port));
		if (!g_websocket_mgr.init(ws_url, origin))
		{
			RTC_LOG(LS_ERROR) << "weboscket connect failed !!! url = " << ws_url;
			return false;
		}
		g_websocket_mgr.start();


		if (g_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
		{
			RTC_LOG(LS_ERROR) << "weboscket mgr status = " << g_websocket_mgr.get_status() << "failed !!! ";
			return false;
		}

		std::string result;
		if (!g_http_mgr.sync_mediasoup_router_rtpcapabilities(result))
		{
			RTC_LOG(LS_ERROR) << "http router rtpcapabilities failed !!!";
			return false;
		}
		


		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" <<result << "]";
		json response = nlohmann::json::parse(result);
		if (!m_broadcaster.Start(response))
		{
			RTC_LOG(LS_ERROR) << "broadcaster start failed !!!";
			return false;
		}
		 
		std::set<std::string> dataProduceIds;
		while (!m_stoped)
		{
			if (g_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
			{
				RTC_LOG(LS_ERROR) << "websocket status = " << g_websocket_mgr.get_status() << "failed !!!";
				break;
			}
			//std::string url = baseUrl + "/broadcasters/" + this->id + "/transports";
			//
			if (!g_http_mgr. sync_mediasoup_all_dataproducers(result))
			{
				RTC_LOG(LS_ERROR) << "[ERROR]sync_mediasoup_all_dataproducers failed !!!";
				break;
			}
			 
			{
				//RTC_LOG(INFO)  << __FUNCTION__ << __LINE__ <<"[" << res->body << "]" ;
				json response = nlohmann::json::parse(result);
				std::set<std::string> temp_dataProducerIds;
				if (response["peers"].is_array())
				{
					for (int i = 0; i < response["peers"].size(); ++i)
					{
						if (response["peers"][i]["displayName"] != g_cfg.get_string(ECI_Client_Name))
						{
							std::string displayName = response["peers"][i]["displayName"];
							temp_dataProducerIds.insert(displayName);
							auto iter = dataProduceIds.find(displayName);
							if (iter != dataProduceIds.end())
							{
								continue;
							}
							

							for (int j = 0; j < response["peers"][i]["dataProducers"].size(); ++j)
							{
								if ("chat" == response["peers"][i]["dataProducers"][j]["label"])
								{
									//dataProduceIds;
									//std::string id = response["peers"][i]["dataProducers"][j]["id"];
									std::string dataProducerId = response["peers"][i]["dataProducers"][j]["id"];
									//uint32_t streamId = response["peers"][i]["dataConsumers"][j]["sctpStreamParameters"]["streamId"];
									//json AppData = response["peers"][i]["dataConsumers"][j]["sctpStreamParameters"]["AppData"];;
									json body =
									{
										{ "dataProducerId", dataProducerId }
									};
									m_broadcaster.CreateDataConsumer(body);
									RTC_LOG(LS_INFO) << "  dataProducerId = " << dataProducerId;
									//dataProduceIds.insert(displayName);
								}
							}
						}
					}
					dataProduceIds.swap(temp_dataProducerIds);
				}
			}
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		return true;
	}
	void ccloud_rendering::Destroy()
	{
		g_websocket_mgr.destroy();
		m_broadcaster.Stop();
		all_stop();
		mediasoupclient::Cleanup();
	}
}