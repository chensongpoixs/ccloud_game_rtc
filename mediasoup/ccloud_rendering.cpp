#include "ccloud_rendering.h"
#include "ccfg.h"
#include "httplib.h"
#include "chttp_mgr.h"
namespace webrtc {
	
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


		auto logLevel = mediasoupclient::Logger::LogLevel::LOG_DEBUG;
		mediasoupclient::Logger::SetLogLevel(logLevel);
		mediasoupclient::Logger::SetDefaultHandler();

		// Initilize mediasoupclient.
		mediasoupclient::Initialize();
		std::string ws_url = "ws://" + webrtc::g_cfg.get_string(webrtc::ECI_MediaSoup_Host) + ":" + std::to_string(webrtc::g_cfg.get_int32(webrtc::ECI_MediaSoup_Http_Port)) + "/?roomId=" + webrtc::g_cfg.get_string(webrtc::ECI_Room_Name) + "&peerId=" + webrtc::g_cfg.get_string(webrtc::ECI_Client_Name);//ws://127.0.0.1:8888/?roomId=chensong&peerId=xiqhlyrn", "http://127.0.0.1:8888")
		std::string origin = "http://" + webrtc::g_cfg.get_string(webrtc::ECI_MediaSoup_Host) + ":" + std::to_string(webrtc::g_cfg.get_int32(webrtc::ECI_MediaSoup_Http_Port));
		if (!webrtc::g_websocket_mgr.init(ws_url, origin))
		{
			RTC_LOG(LS_ERROR) << "weboscket connect failed !!! url = " << ws_url;
			return false;
		}
		webrtc::g_websocket_mgr.start();


		if (webrtc::g_websocket_mgr.get_status() != webrtc::CWEBSOCKET_MESSAGE)
		{
			RTC_LOG(LS_ERROR) << "weboscket mgr status = " << webrtc::g_websocket_mgr.get_status() << "failed !!! ";
			return false;
		}
		return true;
	}
	bool ccloud_rendering::Loop()
	{
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
			if (webrtc::g_websocket_mgr.get_status() != webrtc::CWEBSOCKET_MESSAGE)
			{
				RTC_LOG(LS_ERROR) << "websocket status = " << webrtc::g_websocket_mgr.get_status() << "failed !!!";
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
						if (response["peers"][i]["displayName"] != webrtc::g_cfg.get_string(webrtc::ECI_Client_Name))
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
		m_broadcaster.Stop();
		g_websocket_mgr.destroy();
		mediasoupclient::Cleanup();
	}
}