#include "Broadcaster.hpp"
//#include "MediaStreamTrackFactory.hpp"
#include "mediasoupclient.hpp"
#include "json.hpp"
#include <chrono>
//#include <cpr/cpr.h>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include "httplib.h"
#include "ccfg.h"
#include "peerConnectionUtils.hpp"
#include "desktop_capture.h"
#include "cwindow_util.h"
#include "chttp_mgr.h"
using json = nlohmann::json;


enum EACTION_MOUSE_TYPE
{
	EACTION_MOUSE_MOVE = 0,
	EACTION_MOUSE_DOWNUP,
	EACTION_MOUSE_DOWN,
	EACTION_MOUSE_UP,
	EACTION_MOUSE_BIG,
	EACTION_MOUSE_SMALL,
	EACTION_KEY
};


Broadcaster::~Broadcaster()
{
	 //this->Stop();
}

void Broadcaster::OnTransportClose(mediasoupclient::Producer* /*producer*/)
{
	std::cout << "[INFO] Broadcaster::OnTransportClose()" << std::endl;
}

void Broadcaster::OnTransportClose(mediasoupclient::DataProducer* /*dataProducer*/)
{
	std::cout << "[INFO] Broadcaster::OnTransportClose()" << std::endl;
}

/* Transport::Listener::OnConnect
 *
 * Fired for the first Transport::Consume() or Transport::Produce().
 * Update the already created remote transport with the local DTLS parameters.
 */
std::future<void> Broadcaster::OnConnect(mediasoupclient::Transport* transport, const json& dtlsParameters)
{
	std::cout << "[INFO] Broadcaster::OnConnect()" << std::endl;
	// std::cout << "[INFO] dtlsParameters: " << dtlsParameters.dump(4) << std::endl;

	if (transport->GetId() == this->sendTransport->GetId())
	{
		return this->OnConnectSendTransport(dtlsParameters);
	}
	else if (transport->GetId() == this->recvTransport->GetId())
	{
		return this->OnConnectRecvTransport(dtlsParameters);
	}
	else
	{
		std::promise<void> promise;

		promise.set_exception(std::make_exception_ptr("Unknown transport requested to connect"));

		return promise.get_future();
	}
}

std::future<void> Broadcaster::OnConnectSendTransport(const json& dtlsParameters)
{
	std::promise<void> promise;

	/* clang-format off */
	json body =
	{
		{ "dtlsParameters", dtlsParameters }
	};
	/* clang-format on */
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_connect_transport(id, sendTransport->GetId() , body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup create transport failed !!!";
		promise.set_value();
		return promise.get_future();
	}
	promise.set_value();
	return promise.get_future();
}

std::future<void> Broadcaster::OnConnectRecvTransport(const json& dtlsParameters)
{
	std::promise<void> promise;

	/* clang-format off */
	json body =
	{
		{ "dtlsParameters", dtlsParameters }
	};
	/* clang-format on */
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_connect_transport(id, recvTransport->GetId(), body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup connect transport failed !!!";
		return promise.get_future();
		
	} 
	promise.set_value();
	return promise.get_future();
}

/*
 * Transport::Listener::OnConnectionStateChange.
 */
void Broadcaster::OnConnectionStateChange(
  mediasoupclient::Transport* /*transport*/, const std::string& connectionState)
{
	std::cout << "[INFO] Broadcaster::OnConnectionStateChange() [connectionState:" << connectionState
	          << "]" << std::endl;

	if (connectionState == "failed")
	{
		Stop();
		//std::exit(0);
	}
}

/* Producer::Listener::OnProduce
 *
 * Fired when a producer needs to be created in mediasoup.
 * Retrieve the remote producer ID and feed the caller with it.
 */
std::future<std::string> Broadcaster::OnProduce(
  mediasoupclient::SendTransport* /*transport*/,
  const std::string& kind,
  json rtpParameters,
  const json& /*appData*/)
{
	std::cout << "[INFO] Broadcaster::OnProduce()" << std::endl;
	// std::cout << "[INFO] rtpParameters: " << rtpParameters.dump(4) << std::endl;

	std::promise<std::string> promise;

	/* clang-format off */
	json body =
	{
		{ "kind",          kind          },
		{ "rtpParameters", rtpParameters }
	};
	/* clang-format on */
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_producers_transport(id, sendTransport->GetId(), body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup create transport failed !!!";
		return promise.get_future();
	}
	  
	json response = json::parse(result);

	auto it = response.find("id");
	if (it == response.end() || !it->is_string())
	{
		promise.set_exception(std::make_exception_ptr("'id' missing in response"));
	}

	promise.set_value((*it).get<std::string>());

	return promise.get_future();
}

/* Producer::Listener::OnProduceData
 *
 * Fired when a data producer needs to be created in mediasoup.
 * Retrieve the remote producer ID and feed the caller with it.
 */
std::future<std::string> Broadcaster::OnProduceData(
  mediasoupclient::SendTransport* /*transport*/,
  const json& sctpStreamParameters,
  const std::string& label,
  const std::string& protocol,
  const json& /*appData*/)
{
	std::cout << "[INFO] Broadcaster::OnProduceData()" << std::endl;
	// std::cout << "[INFO] rtpParameters: " << rtpParameters.dump(4) << std::endl;

	std::promise<std::string> promise;

	/* clang-format off */
	json body =
    {
        { "label"                , label },
        { "protocol"             , protocol },
        { "sctpStreamParameters" , sctpStreamParameters }
		// { "appData"				 , "someAppData" }
	};
	/* clang-format on */
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_produce_data_transport(id, sendTransport->GetId(), body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup producers transport failed !!!";
		return promise.get_future();
	}
	
	
	json response = json::parse(result);

	auto it = response.find("id");
	if (it == response.end() || !it->is_string())
	{
		promise.set_exception(std::make_exception_ptr("'id' missing in response"));
	}
	else
	{
		auto dataProducerId = (*it).get<std::string>();
		promise.set_value(dataProducerId);
	}
	return promise.get_future();
}

bool Broadcaster::Start(const json& routerRtpCapabilities)
{
	std::cout << "[INFO] Broadcaster::Start()" << std::endl;

	m_wight = GetSystemMetrics(SM_CXSCREEN);
	m_height = GetSystemMetrics(SM_CYSCREEN);

	  

	// Load the device.
	this->device.Load(routerRtpCapabilities);

	std::cout << "[INFO] creating Broadcaster..." << std::endl;

	/* clang-format off */
	json body =
	{
		{ "id",           id          },
		{ "displayName", webrtc:: g_cfg.get_string(webrtc::ECI_Client_Name)     },
		{ "device",
			{
				{ "name",    "version"       },
				{ "version", "1.0" }
			}
		},
		{ "rtpCapabilities", this->device.GetRtpCapabilities() }
	};

	 
	if (!webrtc::g_http_mgr.sync_mediasoup_broadcasters(body.dump()))
	{
		RTC_LOG(LS_ERROR) << " mediasoup broadcasters failed !!!";
		return false;
	} 
	this->CreateSendTransport();
	this->CreateRecvTransport();
}

void Broadcaster::CreateDataConsumer(const json& body)
{
	
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_consume_data_transport(id, recvTransport->GetId(), body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup consume data transport failed !!!";
		return;
	}
	
	auto response = json::parse(result);
	if (response.find("id") == response.end())
	{
		std::cerr << "[ERROR] 'id' missing in response" << std::endl;
		return;
	}
	auto dataConsumerId = response["id"].get<std::string>();

	if (response.find("streamId") == response.end())
	{
		std::cerr << "[ERROR] 'streamId' missing in response" << std::endl;
		return;
	}
	
	//uint16_t streamId = response["streamId"].get<uint16_t>();

	//// Create client consumer.
	//this->dataConsumer = this->recvTransport->ConsumeData(
	//  this, dataConsumerId, body["dataProducerId"]/*dataProducerId*/, std::to_string(streamId), "chat", "stcp");
}
//void Broadcaster::createDataConsumer(std::string dataConsumerId, std::string dataProducerId, std::string streamId, const nlohmann::json& appData)
//{
//	this->recvTransport->ConsumeData(
//		this, dataConsumerId, dataProducerId, "chat", "stcp", appData);
//}
void Broadcaster::CreateSendTransport( )
{
	std::cout << "[INFO] creating mediasoup send WebRtcTransport..." << std::endl;

	json sctpCapabilities = this->device.GetSctpCapabilities();
	 
	json body =
	{
		{ "type",    "webrtc" },
		{ "rtcpMux", true     },
		{ "sctpCapabilities", sctpCapabilities }
	};
	 
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_create_transport(id, body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup create transport failed !!!";
		return;
	}

	 
	json response = json::parse(result);

	if (response.find("id") == response.end())
	{
		std::cerr << "[ERROR] 'id' missing in response" << std::endl;

		return;
	}
	else if (response.find("iceParameters") == response.end())
	{
		std::cerr << "[ERROR] 'iceParametersd' missing in response" << std::endl;

		return;
	}
	else if (response.find("iceCandidates") == response.end())
	{
		std::cerr << "[ERROR] 'iceCandidates' missing in response" << std::endl;

		return;
	}
	else if (response.find("dtlsParameters") == response.end())
	{
		std::cerr << "[ERROR] 'dtlsParameters' missing in response" << std::endl;

		return;
	}
	else if (response.find("sctpParameters") == response.end())
	{
		std::cerr << "[ERROR] 'sctpParameters' missing in response" << std::endl;

		return;
	}

	std::cout << "[INFO] creating SendTransport..." << std::endl;
	/************************************************************************/
	/* 
	
	{
	"id":"be6b189d-c29b-4f3b-a3a2-3f809fc9e069",
	"iceParameters":{
		"iceLite":true,
		"password":"qw6duz437j2wve493dvuz4xsc475wrlu",
		"usernameFragment":"cy0v81uhh58at8un"
	},
	"iceCandidates":[
		{
			"foundation":"udpcandidate",
			"ip":"192.168.0.78",
			"port":41397,
			"priority":1076302079,
			"protocol":"udp",
			"type":"host"
		}
	],
	"dtlsParameters":{
		"fingerprints":[
			{
				"algorithm":"sha-1",
				"value":"C3:DD:71:5F:F6:89:38:8E:EB:FA:DF:74:2E:13:CA:C6:79:1D:16:2E"
			},
			{
				"algorithm":"sha-224",
				"value":"20:FB:45:D9:DE:1C:09:1A:6D:6D:0B:98:7A:55:34:0F:96:97:B5:A2:37:33:2B:56:B9:08:2F:7F"
			},
			{
				"algorithm":"sha-256",
				"value":"04:9C:D5:BB:82:18:70:E1:F4:59:81:E1:D5:F9:BB:0F:1F:C8:F9:92:00:7E:A0:07:C7:42:5B:F4:16:3F:0B:A8"
			},
			{
				"algorithm":"sha-384",
				"value":"FD:28:C3:88:BB:DB:85:77:58:D5:3C:04:A0:FF:FA:52:3B:57:DA:2B:86:F2:31:73:CC:D5:73:09:42:30:67:A2:B9:72:70:31:2F:7A:CA:9A:1B:18:92:24:1F:AE:39:55"
			},
			{
				"algorithm":"sha-512",
				"value":"73:94:C7:0A:88:FB:37:F4:6F:49:33:DA:4B:5E:30:4B:48:4F:FB:E8:C5:10:5B:DA:7F:8A:32:03:6A:63:38:CF:A8:73:3D:BD:A6:B1:5F:32:C2:D5:9C:C2:FB:E8:9F:A9:3D:D5:56:25:3F:E2:52:02:10:D7:53:A5:72:4D:DC:78"
			}
		],
		"role":"auto"
	},
	"sctpParameters":{
		"MIS":1024,
		"OS":1024,
		"isDataChannel":true,
		"maxMessageSize":262144,
		"port":5000,
		"sctpBufferedAmount":0,
		"sendBufferSize":262144
	}
}
	*/
	/************************************************************************/
	auto sendTransportId = response["id"].get<std::string>();
	// 创建offer 的流程 回调 onConnect函数
	this->sendTransport = this->device.CreateSendTransport(
	  this,
	  sendTransportId,
	  response["iceParameters"],
	  response["iceCandidates"],
	  response["dtlsParameters"],
	  response["sctpParameters"]);

 
	///////////////////////// Create Video Producer //////////////////////////

	if (this->device.CanProduce("video"))
	{
		auto videoTrack = createVideoTrack(std::to_string(rtc::CreateRandomId()));
		sendTransport->Produce(this, videoTrack, nullptr, nullptr);
	}
	else
	{
		RTC_LOG(WARNING)  << "[WARN] cannot produce video" ;
	}

	
	///////////////////////// Create Data Producer //////////////////////////

	this->dataProducer = sendTransport->ProduceData(this, "chat", "stcp");
}

void Broadcaster::CreateRecvTransport()
{
	std::cout << "[INFO] creating mediasoup recv WebRtcTransport..." << std::endl;

	json sctpCapabilities = this->device.GetSctpCapabilities();
	/* clang-format off */
	json body =
	{
		{ "type",    "webrtc" },
		{ "rtcpMux", true     },
		{ "sctpCapabilities", sctpCapabilities }
	};
	/* clang-format on */
	std::string result;
	if (!webrtc::g_http_mgr.sync_mediasoup_create_transport(id, body.dump(), result))
	{
		RTC_LOG(LS_ERROR) << "mediasoup create recv transport failed !!!";
		return;
	}

	  

	json response = json::parse(result);

	if (response.find("id") == response.end())
	{
		std::cerr << "[ERROR] 'id' missing in response" << std::endl;

		return;
	}
	else if (response.find("iceParameters") == response.end())
	{
		std::cerr << "[ERROR] 'iceParameters' missing in response" << std::endl;

		return;
	}
	else if (response.find("iceCandidates") == response.end())
	{
		std::cerr << "[ERROR] 'iceCandidates' missing in response" << std::endl;

		return;
	}
	else if (response.find("dtlsParameters") == response.end())
	{
		std::cerr << "[ERROR] 'dtlsParameters' missing in response" << std::endl;

		return;
	}
	else if (response.find("sctpParameters") == response.end())
	{
		std::cerr << "[ERROR] 'sctpParameters' missing in response" << std::endl;

		return;
	}

	std::string recvTransportId = response["id"].get<std::string>();

	std::cout << "[INFO] creating RecvTransport..." << std::endl;

	json sctpParameters = response["sctpParameters"];

	this->recvTransport = this->device.CreateRecvTransport(
	  this,
	  recvTransportId,
	  response["iceParameters"],
	  response["iceCandidates"],
	  response["dtlsParameters"],
	  sctpParameters);
	const std::string& dataProducerId = this->dataProducer->GetId();

	/* clang-format off */
 	json body_json =
	{
		{ "dataProducerId", dataProducerId }
	};
	this->CreateDataConsumer(body_json);
}








void Broadcaster::OnMessage(mediasoupclient::DataConsumer* dataConsumer, const webrtc::DataBuffer& buffer)
{
	std::string s = std::string(buffer.data.data<char>(), buffer.data.size());


	RTC_LOG(LS_INFO)<< "[INFO] Broadcaster::OnMessage()"  << "dataConsumer->GetLabel() = " << dataConsumer->GetLabel()<< "[s = " << s << "]";
	if (dataConsumer->GetLabel() == "chat")
	{
		std::cout << "[INFO] received chat data: " + s << std::endl;
	}
	 
	json response;
	try
	{
		response = json::parse(s);
	}
	catch (const std::exception&)
	{
		RTC_LOG(LS_ERROR) << "json parse error !!!!";
		return;
	}
	if (response.find("event") == response.end())
	{
		RTC_LOG(LS_ERROR) << "[ERROR] 'event' missing in response" ;

		return;
	}
	if (response.find("wight") == response.end())
	{
		RTC_LOG(LS_ERROR) << "[ERROR] 'wight' missing in response" ;

		return;
	}
	if (response.find("height") == response.end())
	{
		RTC_LOG(LS_ERROR) << "[ERROR] 'height' missing in response";

		return;
	}
	if (response.find("windowwidth") == response.end())
	{
		RTC_LOG(LS_ERROR) << "[ERROR] 'windowwidth' missing in response";

		return;
	}
	if (response.find("windowheight") == response.end())
	{
		RTC_LOG(LS_ERROR) << "[ERROR] 'windowheight' missing in response";

		return;
	}
	EACTION_MOUSE_TYPE event = static_cast<EACTION_MOUSE_TYPE>(response["event"]);
	double wight = response["wight"];
	double height = response["height"];
	double windowwidth =  response["windowwidth"];
	double windowheight =  response["windowheight"];
	if (wight < 0 || height < 0 || windowwidth < 0 || windowheight < 0)
	{
		RTC_LOG(LS_ERROR) << "tail small  wight = " << wight << ", hieght = " << height << ", windowwidth = " << windowwidth << ", windowheight = " << windowheight << " failed !!!";
		return;
	}
	static int wheel = 0;
	DWORD action = 0;

	UINT action_type = 0;

	static bool mainwidows = false;
	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	static HWND wnd;
	//if (!mainwidows)
	{
		wnd =  webrtc::FindMainWindow(); // GetActiveWindow();;
		//mainwidows = true;
	}
	
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();				\
		std::chrono::steady_clock::duration diff = end_time - start_time;								\
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);		
	RTC_LOG(LS_INFO) << "findmainwindow " << ms.count() << " ms ";
	//::PostMessage(wnd, WM_MOUSEWHEEL, MAKEWPARAM(0, 120) /* ascii码 */, MAKELPARAM(600, 300));
	 

	//::PostMessage(j2, WM_LBUTTONDOWN, 1, 17 + 34 * 65536);//坐标是用spy++看真是鼠标双击得出来的第一行某点的坐标
	//::PostMessage(j2, WM_LBUTTONUP, NULL, 17 + 34 * 65536);
	//::PostMessage(j2, WM_LBUTTONDOWN, 1, 17 + 34 * 65536);
	//::PostMessage(j2, WM_LBUTTONUP, NULL, 17 + 34 * 65536);
	g_width = (int32_t)wight;
	g_height = (int32_t)height;
	static bool move = false;
	//return;
	if (event == EACTION_MOUSE_MOVE)
	{
		action = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
		action_type = WM_MOUSEMOVE;
		if (move)
		{
			::PostMessage(wnd, action_type,    MAKEWPARAM(0, 0), MAKEWPARAM(wight, height));
		}
		
		return;
	}
	else if (event == EACTION_MOUSE_DOWNUP)
	{
		return;
		action = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
		action_type = WM_LBUTTONUP;
	}
	else if (event == EACTION_MOUSE_DOWN)
	{
		action =MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN ;
		action_type = WM_LBUTTONDOWN;
		move = true;
		::PostMessage(wnd, action_type,  MAKEWPARAM(0, 0), MAKEWPARAM(wight, height));
		//std::this_thread::sleep_for(std::chrono::milliseconds(3));
		return;
		
	}
	else if (event == EACTION_MOUSE_UP)
	{
		action =MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE  | MOUSEEVENTF_LEFTUP;
		action_type = WM_LBUTTONUP;
		move = false;
		::PostMessage(wnd, action_type,  MAKEWPARAM(0, 0), MAKEWPARAM(wight, height));
		return;
	}
	else if (event == EACTION_MOUSE_BIG)
	{
		::PostMessage(wnd, WM_MOUSEWHEEL, MAKEWPARAM(0, 100) /* ascii码 */, MAKELPARAM(wight, height));
		return;
		action =MOUSEEVENTF_ABSOLUTE |MOUSEEVENTF_WHEEL;
		if (wheel < 0)
		{
			wheel = 0;
		}
		wheel += 10;
		mouse_event(action,0, 0, wheel, 0 );
		return;
	}
	else if (event == EACTION_MOUSE_SMALL)
	{
		::PostMessage(wnd, WM_MOUSEWHEEL, MAKEWPARAM(0, -100) /* ascii码 */, MAKELPARAM(wight, height));
		/*action =MOUSEEVENTF_ABSOLUTE |MOUSEEVENTF_WHEEL;
		if (wheel > 0)
		{
			wheel = 0;
		}
		wheel -= 10;
		mouse_event(action,0, 0, wheel, 0 );*/
		return;
	}
	else if (event == EACTION_KEY)
	{
		// key down 
		if (response.find("key") == response.end())
		{
			RTC_LOG(LS_ERROR) << "[ERROR] 'key' missing in response";

			return;
		}
		int key = response["key"];
		::PostMessage(wnd, action_type,  key, 0);
		return;
	}
	else
	{
		RTC_LOG(LS_ERROR) << " event = " << event << " failed !!!";
		return;
	}
	wheel = 0;
	static uint64_t HT = 100000;
	double wx = (wight / windowwidth) * HT ;
	double hy = height / windowheight * HT;
	double  x = (wx * m_wight)/HT;
	double y = (hy * m_height)/HT;
	double new_x = (wight / windowwidth)  ;;
	double new_y =  (height / windowheight)  ;
	RTC_LOG(LS_INFO) << "wight = " << wight << ", height = " << height << ", windowwidth = " << windowwidth << ", windowheight = " << windowheight;
	RTC_LOG(LS_INFO) << "wx = " << wx << ", hy = " << hy <<"x = " << x << ", y = " << y;
	//mouse_event(action, x * 65535 / m_wight, y * 65535 / m_height, 0, 0 );
	::PostMessage(wnd, action_type,  MAKEWPARAM(0, 0), MAKEWPARAM(wight, height));

}

void Broadcaster::Stop()
{
	std::cout << "[INFO] Broadcaster::Stop()" << std::endl;

	//this->timerKiller.Kill();

	stopTrack();
	/*if (this->recvTransport)
	{
		recvTransport->Close();
	}

	if (this->sendTransport)
	{
		sendTransport->Close();
	}*/
	device.reset();
	 
	if (!webrtc::g_http_mgr.sync_mediasoup_exit(id))
	{
		RTC_LOG(LS_ERROR) << "mediasoup exit failed !!!";
		return;
	} 
}

void Broadcaster::OnOpen(mediasoupclient::DataProducer* /*dataProducer*/)
{
	std::cout << "[INFO] Broadcaster::OnOpen()" << std::endl;
}
void Broadcaster::OnClose(mediasoupclient::DataProducer* /*dataProducer*/)
{
	std::cout << "[INFO] Broadcaster::OnClose()" << std::endl;
}
void Broadcaster::OnBufferedAmountChange(mediasoupclient::DataProducer* /*dataProducer*/, uint64_t /*size*/)
{
	std::cout << "[INFO] Broadcaster::OnBufferedAmountChange()" << std::endl;
}
