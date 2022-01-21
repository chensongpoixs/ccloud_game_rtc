﻿#include "Broadcaster.hpp"
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
#include "cinput_device.h"
#include "clog.h"
using json = nlohmann::json;

namespace chen {
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
		//std::cout << "[INFO] Broadcaster::OnTransportClose()" << std::endl;
		NORMAL_EX_LOG("");
		//ERROR_EX_LOG
	}

	void Broadcaster::OnTransportClose(mediasoupclient::DataProducer* /*dataProducer*/)
	{
		//std::cout << "[INFO] Broadcaster::OnTransportClose()" << std::endl;
		NORMAL_EX_LOG("");
	}

	/* Transport::Listener::OnConnect
	 *
	 * Fired for the first Transport::Consume() or Transport::Produce().
	 * Update the already created remote transport with the local DTLS parameters.
	 */
	std::future<void> Broadcaster::OnConnect(mediasoupclient::Transport* transport, const json& dtlsParameters)
	{
		NORMAL_EX_LOG("");
		//std::cout << "[INFO] Broadcaster::OnConnect()" << std::endl;
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
		if (!chen::g_http_mgr.sync_mediasoup_connect_transport(id, sendTransport->GetId(), body.dump(), result))
		{
			//RTC_LOG(LS_ERROR) << "mediasoup create transport failed !!!";
			ERROR_EX_LOG("mediasoup create transport  [id = %s][sendTransport->GetId() = %s][body.dump() = %s ]failed !!!", id.c_str(), sendTransport->GetId().c_str(), body.dump().c_str());
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
		if (!chen::g_http_mgr.sync_mediasoup_connect_transport(id, recvTransport->GetId(), body.dump(), result))
		{
			//RTC_LOG(LS_ERROR) << "mediasoup connect transport failed !!!";
			ERROR_EX_LOG("mediasoup connect transport [id = %s][recvTransport->GetId() = %s][body.dump() = %s] failed !!!", id.c_str(), recvTransport->GetId().c_str(), body.dump().c_str());
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
		/*std::cout << "[INFO] Broadcaster::OnConnectionStateChange() [connectionState:" << connectionState
			<< "]" << std::endl;*/
		NORMAL_EX_LOG("[connectionState = %s]", connectionState.c_str());
		if (connectionState == "failed")
		{
			// TODO@chensong 2022-01-21  close webrtc -->> ok 
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
		NORMAL_EX_LOG("[INFO] Broadcaster::OnProduce()" );
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
		if (!chen::g_http_mgr.sync_mediasoup_producers_transport(id, sendTransport->GetId(), body.dump(), result))
		{
			RTC_LOG(LS_ERROR) << "mediasoup create transport failed !!!";
			ERROR_EX_LOG("mediasoup create transport [id = %s][sendTransport->GetId() = %s][body.dump()= %s] failed !!!", id.c_str(), sendTransport->GetId().c_str(), body.dump().c_str());
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
		//std::cout << "[INFO] Broadcaster::OnProduceData()" << std::endl;
		// std::cout << "[INFO] rtpParameters: " << rtpParameters.dump(4) << std::endl;
		NORMAL_EX_LOG("");
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
		if (!chen::g_http_mgr.sync_mediasoup_produce_data_transport(id, sendTransport->GetId(), body.dump(), result))
		{
			//RTC_LOG(LS_ERROR) << "mediasoup producers transport failed !!!";
			ERROR_EX_LOG("mediasoup producers transport [id = %s][sendTransport->GetId() = %s][body.dump() = %s]failed !!!", id.c_str(), sendTransport->GetId().c_str(), body.dump().c_str());
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
			const std::string & dataProducerId = (*it).get<std::string>();
			promise.set_value(dataProducerId);
		}
		return promise.get_future();
	}

	bool Broadcaster::Start(const json& routerRtpCapabilities)
	{
		//std::cout << "[INFO] Broadcaster::Start()" << std::endl;
		NORMAL_EX_LOG("");
		id = std::to_string(rtc::CreateRandomId());
		m_wight = GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);



		// Load the device.
		this->device.Load(routerRtpCapabilities);

		//std::cout << "[INFO] creating Broadcaster..." << std::endl;

		/* clang-format off */
		json body =
		{
			{ "id",           id          },
			{ "displayName", chen::g_cfg.get_string(chen::ECI_Client_Name)     },
			{ "device",
				{
					{ "name",    "version"       },
					{ "version", "1.0" }
				}
			},
			{ "rtpCapabilities", this->device.GetRtpCapabilities() }
		};


		if (!chen::g_http_mgr.sync_mediasoup_broadcasters(body.dump()))
		{
			RTC_LOG(LS_ERROR) << " mediasoup broadcasters failed !!!";
			ERROR_EX_LOG("mediasoup broadcasters [body.dump() = %s]failed !!!", body.dump().c_str());
			return false;
		}
		this->CreateSendTransport();
		this->CreateRecvTransport();
		return true;
	}

	void Broadcaster::CreateDataConsumer(const json& body)
	{

		std::string result;
		if (!chen::g_http_mgr.sync_mediasoup_consume_data_transport(id, recvTransport->GetId(), body.dump(), result))
		{
			//RTC_LOG(LS_ERROR) << "mediasoup consume data transport failed !!!";
			ERROR_EX_LOG("mediasoup consume data transport [id = %s][recvTransport->GetId() = %s][body.dump()= %s] failed !!!", id.c_str(), recvTransport->GetId().c_str(), body.dump().c_str());
			return;
		}

		auto response = json::parse(result);
		if (response.find("id") == response.end())
		{
			ERROR_EX_LOG("[ERROR] 'id' missing in response = [%s]", result.c_str());
			return;
		}
		const std::string & dataConsumerId = response["id"].get<std::string>();

		if (response.find("streamId") == response.end())
		{
			//std::cerr << "[ERROR] 'streamId' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'streamId' missing in response [%s]", result.c_str());
			return;
		}

		uint16_t streamId = response["streamId"].get<uint16_t>();

		//// Create client consumer.
		this->dataConsumer = this->recvTransport->ConsumeData(
			this, dataConsumerId, body["dataProducerId"]/*dataProducerId*/, std::to_string(streamId), "chat", "stcp");
	}

	void Broadcaster::CreateSendTransport()
	{
		//std::cout << "[INFO] creating mediasoup send WebRtcTransport..." << std::endl;
		NORMAL_EX_LOG("creating mediasoup send WebRtcTransport...");
		json sctpCapabilities = this->device.GetSctpCapabilities();

		json body =
		{
			{ "type",    "webrtc" },
			{ "rtcpMux", true     },
			{ "sctpCapabilities", sctpCapabilities }
		};

		std::string result;
		if (!chen::g_http_mgr.sync_mediasoup_create_transport(id, body.dump(), result))
		{
		//	RTC_LOG(LS_ERROR) << "mediasoup create transport failed !!!";
			ERROR_EX_LOG("mediasoup create transport [id= %s][body.dump() = %s] failed !!!", id.c_str(), body.dump().c_str());
			return;
		}


		json response = json::parse(result);

		if (response.find("id") == response.end())
		{
			//std::cerr << "[ERROR] 'id' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'id' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("iceParameters") == response.end())
		{
			//std::cerr << "[ERROR] 'iceParametersd' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'iceParametersd' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("iceCandidates") == response.end())
		{
			//std::cerr << "[ERROR] 'iceCandidates' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'iceCandidates' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("dtlsParameters") == response.end())
		{
			std::cerr << "[ERROR] 'dtlsParameters' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'dtlsParameters' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("sctpParameters") == response.end())
		{
			//std::cerr << "[ERROR] 'sctpParameters' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'sctpParameters' missing in response [%s]", result.c_str());
			return;
		}

		NORMAL_EX_LOG("[INFO] creating SendTransport...");
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
			WARNING_EX_LOG("[WARN] cannot produce video");
		}


		///////////////////////// Create Data Producer //////////////////////////

		this->dataProducer = sendTransport->ProduceData(this, "chat", "stcp");
	}

	void Broadcaster::CreateRecvTransport()
	{
		NORMAL_EX_LOG( "[INFO] creating mediasoup recv WebRtcTransport..." );

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
		if (!chen::g_http_mgr.sync_mediasoup_create_transport(id, body.dump(), result))
		{
			//RTC_LOG(LS_ERROR) << "mediasoup create recv transport failed !!!";
			ERROR_EX_LOG("mediasoup create recv transport [id = %s][body.dump() = %s] failed !!!", id.c_str(), body.dump().c_str());
			return;
		}



		json response = json::parse(result);

		if (response.find("id") == response.end())
		{
			//std::cerr << "[ERROR] 'id' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'id' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("iceParameters") == response.end())
		{
			//std::cerr << "[ERROR] 'iceParameters' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'iceParameters' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("iceCandidates") == response.end())
		{
			//std::cerr << "[ERROR] 'iceCandidates' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'iceCandidates' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("dtlsParameters") == response.end())
		{
			//std::cerr << "[ERROR] 'dtlsParameters' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'dtlsParameters' missing in response [%s]", result.c_str());
			return;
		}
		else if (response.find("sctpParameters") == response.end())
		{
			//std::cerr << "[ERROR] 'sctpParameters' missing in response" << std::endl;
			ERROR_EX_LOG("[ERROR] 'sctpParameters' missing in response [%s]", result.c_str());
			return;
		}

		std::string recvTransportId = response["id"].get<std::string>();

		NORMAL_EX_LOG("creating RecvTransport...");
		//std::cout << "[INFO] creating RecvTransport..." << std::endl;

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
		chen::g_input_device_mgr.OnMessage(buffer);
	}

	void Broadcaster::Stop()
	{
		NORMAL_EX_LOG("[INFO] Broadcaster::Stop()");

		//this->timerKiller.Kill();

		stopTrack();
		if (this->recvTransport)
		{
			recvTransport->Close();
			//recvTransport->all_close();
		}

		if (this->sendTransport)
		{
			sendTransport->Close();
		}
		if (dataConsumer)
		{
			dataConsumer->Close();
		}
		if (dataProducer)
		{
			dataProducer->Close();
		}
		device.reset();

		if (!chen::g_http_mgr.sync_mediasoup_exit(id))
		{
			ERROR_EX_LOG( "mediasoup [id = %s] exit failed !!!", id.c_str());
			return;
		}
	}

	void Broadcaster::OnOpen(mediasoupclient::DataProducer* /*dataProducer*/)
	{
		NORMAL_EX_LOG("");
		//std::cout << "[INFO] Broadcaster::OnOpen()" << std::endl;
	}
	void Broadcaster::OnClose(mediasoupclient::DataProducer* /*dataProducer*/)
	{
		NORMAL_EX_LOG("");
		//std::cout << "[INFO] Broadcaster::OnClose()" << std::endl;
	}
	void Broadcaster::OnBufferedAmountChange(mediasoupclient::DataProducer* /*dataProducer*/, uint64_t /*size*/)
	{
		NORMAL_EX_LOG("");
		//std::cout << "[INFO] Broadcaster::OnBufferedAmountChange()" << std::endl;
	}


}