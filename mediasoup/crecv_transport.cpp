/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#include "crecv_transport.h"
#include "cclient.h"
#include "cassertion_macros.h"
#include "cdataconsumer.h"
#include "csession_description.h"
namespace chen {
	bool crecv_transport::init(const std::string &transport_id, const nlohmann::json& extendedRtpCapabilities, const nlohmann::json& iceParameters,
		const nlohmann::json& iceCandidates,
		const nlohmann::json& dtlsParameters,
		const nlohmann::json& sctpParameters)
	{
		return ctransport::init(transport_id, extendedRtpCapabilities, iceParameters, iceCandidates, dtlsParameters, sctpParameters);
	}
	void crecv_transport::Destroy()
	{
		SYSTEM_LOG("recv transport destroy !!!");
		//std::map<std::string, std::shared_ptr<cdataconsumer>>
		for (std::pair<std::string, std::shared_ptr<cdataconsumer>> p : m_data_consumsers)
		{
			std::shared_ptr<cdataconsumer> data_ptr = p.second;
			if (!data_ptr)
			{
				WARNING_EX_LOG("datacosumsers  id = %s , ptr = nullptr ", p.first.c_str());
				continue;
			}
		//	data_ptr->Destroy();
		}
		SYSTEM_LOG("recv transport stop !!!");
		//m_data_consumsers.clear();
		if (m_peer_connection_factory)
		{
			m_peer_connection_factory->StopAecDump();
		}
		m_peer_connection_factory = nullptr;
		m_peer_connection = nullptr;
		SYSTEM_LOG("recv factory ok null !!!");
		/*	if (m_networkThread)
			{
				m_networkThread->Stop();
				SYSTEM_LOG("m_networkThread ok  !!!");
			}
			if (m_signalingThread)
			{
				m_signalingThread->Stop();
				SYSTEM_LOG("m_signalingThread ok  !!!");
			}
			if (m_workerThread)
			{
				m_workerThread->Stop();
				SYSTEM_LOG("m_workerThread ok  !!!");
			}*/
		 
			 
	}
	void crecv_transport::close_dataconsumer(const std::string & dataconsumer_id)
	{
		
		std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator iter = m_data_consumsers.find(dataconsumer_id);
		if (iter == m_data_consumsers.end())
		{
			WARNING_EX_LOG("not find dataconsumer id = %s failed !!!", dataconsumer_id.c_str());
			return;
		}
		//std::vector<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>> RtpTransceiver_vec = m_peer_connection->GetTransceivers();
		//iter->second->Destroy();
		// waring 
		m_data_consumsers.erase(iter);
	}
	// CreateSessionDescriptionObserver implementation.
	void crecv_transport::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	{
		std::string sdp;

		desc->ToString(&sdp);
		m_offer = sdp;
		//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
		m_client_ptr->transportofferasner(false, true);
	}
	void crecv_transport::OnFailure(webrtc::RTCError error)
	{
		m_client_ptr->transportofferasner(false, false);
	}

	bool crecv_transport::webrtc_connect_recv_setup_call()
	{
		std::string offer = m_offer;

		webrtc::SdpParseError error;
		webrtc::SessionDescriptionInterface* sessionDescription;
		rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
			new rtc::RefCountedObject<cSetSessionDescriptionObserver>());

		std::string  typeStr = "answer";
		auto future = observer->GetFuture();

		sessionDescription = webrtc::CreateSessionDescription(typeStr, offer, &error);
		if (sessionDescription == nullptr)
		{
			/*MSC_WARN(
			"webrtc::CreateSessionDescription failed [%s]: %s",
			error.line.c_str(),
			error.description.c_str());*/

			observer->Reject(error.description);
			future.get();
			return false;
		}

		m_peer_connection->SetLocalDescription(observer, sessionDescription);
		//m_client_ptr->async_produce();
		//return true;
		if (m_dataconsmers.empty())
		{
			m_client_ptr->async_produce();
			m_recving = ERecv_Success;
			return true;
		}
		{
			cDataConsmer cdata = m_dataconsmers.front();
			m_dataconsmers.pop_front();
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_consumsers.insert(std::make_pair(cdata.m_id, std::make_shared<cdataconsumer>(this)));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				return false;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", cdata.m_id    },
			{ "ordered",  false }
			};
			(pi.first->second)->init(cdata.m_id, cdata.m_dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");


		}

		while (!m_dataconsmers.empty())
		{
			cDataConsmer cdata = m_dataconsmers.front();
			m_dataconsmers.pop_front();

			webrtc::DataChannelInit dataChannelInit;
			dataChannelInit.protocol = "chat";
			dataChannelInit.negotiated = true;
			dataChannelInit.id = ++m_streamId;
			webrtcDataChannel = m_peer_connection->CreateDataChannel(cdata.m_lable, &dataChannelInit);
			if (!webrtcDataChannel.get())
			{
				ERROR_EX_LOG("create data channel failed !!!");
				continue;;
			}
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_consumsers.insert(std::make_pair(cdata.m_id, std::make_shared<cdataconsumer>(this)));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				continue;;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", dataChannelInit.id                },
			{ "ordered",  dataChannelInit.ordered }
			};
			(pi.first->second)->init(cdata.m_id, cdata.m_dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");
		}
		//
		//m_client_ptr->async_produce();
		m_recving = ERecv_Success;
		return true;
	}



	/////////////////////////////////////////////////////////////////

	bool  crecv_transport::webrtc_create_consumer(const std::string & id, const std::string & dataconsumerId, const std::string & label)
	{
		NORMAL_EX_LOG("id = %s, datacnsumerid = %s, label = %s", id.c_str(), dataconsumerId.c_str(), label.c_str());

		if (m_recving == ERecv_Success)
		{
			webrtc::DataChannelInit dataChannelInit;
			dataChannelInit.protocol = "chat";
			dataChannelInit.negotiated = true;
			dataChannelInit.id = ++m_streamId;
			webrtcDataChannel = m_peer_connection->CreateDataChannel(label, &dataChannelInit);
			if (!webrtcDataChannel.get())
			{
				ERROR_EX_LOG("create data channel failed !!!");
				return false;
			}
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_consumsers.insert(std::make_pair(id, std::make_shared<cdataconsumer>(this)));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				return false;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", dataChannelInit.id                },
			{ "ordered",  dataChannelInit.ordered }
			};
			(pi.first->second)->init(id, dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");
			return true;
		}
		else if (m_recving == ERecv_Recving)
		{
			cDataConsmer Datacs;
			Datacs.m_id = id;
			Datacs.m_lable = label;
			Datacs.m_dataconsumerId = dataconsumerId;
			m_dataconsmers.push_back(Datacs);
			return true;
		}
		cDataConsmer Datacs;
		Datacs.m_id = id;
		Datacs.m_lable = label;
		Datacs.m_dataconsumerId = dataconsumerId;
		m_dataconsmers.push_back(Datacs);
		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.protocol = "chat";
		dataChannelInit.negotiated = true;
		dataChannelInit.id = ++m_streamId;
		bool setremotesdp = false;
		if (!webrtcDataChannel)
		{
			setremotesdp = true;
		}
		webrtcDataChannel = m_peer_connection->CreateDataChannel(label, &dataChannelInit);
		if (!webrtcDataChannel.get())
		{
			ERROR_EX_LOG("create data channel failed !!!");
			return false;
		}
		webrtc::PeerConnectionInterface::SignalingState state = m_peer_connection->signaling_state();
		m_remote_sdp->RecvSctpAssociation();
		std::string sdpoffer = m_remote_sdp->GetSdp();
		{
			webrtc::SdpParseError error;
			webrtc::SessionDescriptionInterface* sessionDescription;
			rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
				new rtc::RefCountedObject<cSetSessionDescriptionObserver>());

			std::string typeStr = "offer";
			auto future = observer->GetFuture();

			sessionDescription = webrtc::CreateSessionDescription(typeStr, sdpoffer, &error);
			if (sessionDescription == nullptr)
			{
				/*MSC_WARN(
				"webrtc::CreateSessionDescription failed [%s]: %s",
				error.line.c_str(),
				error.description.c_str());*/

				observer->Reject(error.description);
				future.get();
				return false;
			}


			m_peer_connection->SetRemoteDescription(observer, sessionDescription);
			future.get();
			webrtc::PeerConnectionInterface::SignalingState state = m_peer_connection->signaling_state();
		}

		{
			webrtc::PeerConnectionInterface::SignalingState state = m_peer_connection->signaling_state();
			//m_peer_connection->ChangeSignalingState(webrtc::PeerConnectionInterface::kHaveRemotePrAnswer);
			chen::CreateSessionDescriptionObserver* sessionDescriptionObserver =
				new rtc::RefCountedObject<chen::CreateSessionDescriptionObserver>();
			rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
				new rtc::RefCountedObject<cSetSessionDescriptionObserver>());
			webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
			auto future = sessionDescriptionObserver->GetFuture();
			m_peer_connection->CreateAnswer(sessionDescriptionObserver, options);
			std::string offer = future.get();
			m_offer = offer;
			{
				webrtc::PeerConnectionInterface::SignalingState statew = m_peer_connection->signaling_state();
			}
			m_recving = ERecv_Recving;
			NORMAL_EX_LOG("recv create answer !!!");
			//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
			m_client_ptr->transportofferasner(false, true);
		}
		//m_peer_connection->SetRemoteDescription()


		//std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_cosumsers.insert(std::make_pair(id, std::make_shared<cdataconsumer>()));
		//if (!pi.second)
		//{
		//	ERROR_EX_LOG("create data channel insert failed !!!");
		//	return false;
		//}
		///* clang-format off */
		//nlohmann::json sctpStreamParameters =
		//{
		//	{ "streamId", dataChannelInit.id                },
		//{ "ordered",  dataChannelInit.ordered }
		//};
		//(pi.first->second)->init(id, dataconsumerId, webrtcDataChannel, sctpStreamParameters,  "stcp");
		// m_data_cosumsers.insert(std::make_pair(id, cdataconsumer())
		return true;
	}
	bool crecv_transport::webrtc_consumer_wait(const std::string & id, const std::string & dataconsumerId, const std::string & label)
	{
		cDataConsmer Datacs;
		Datacs.m_id = id;
		Datacs.m_lable = label;
		Datacs.m_dataconsumerId = dataconsumerId;
		m_dataconsmers.push_back(Datacs);
		return true;
	}
	void crecv_transport::webrtc_create_all_wait_consumer()
	{
		std::list<cDataConsmer>	temp_data = std::move(m_dataconsmers);
		m_dataconsmers.clear();
		while (!temp_data.empty())
		{
			cDataConsmer cdata = temp_data.front();
			temp_data.pop_front();
			webrtc_create_consumer(cdata.m_id, cdata.m_dataconsumerId, cdata.m_lable);
		}
	}

	void crecv_transport::add_webrtc_consmer_transport()
	{
		if (m_recving == ERecv_Success)
		{
			return;
		}
		if (m_dataconsmers.empty())
		{
			m_recving = ERecv_Success;
			return;
		}
		{
			cDataConsmer cdata = m_dataconsmers.front();
			m_dataconsmers.pop_front();
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_consumsers.insert(std::make_pair(cdata.m_id, std::make_shared<cdataconsumer>(this)));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");

			}
			else
			{
				/* clang-format off */
				nlohmann::json sctpStreamParameters =
				{
					{ "streamId", cdata.m_id    },
				{ "ordered",  false }
				};
				(pi.first->second)->init(cdata.m_id, cdata.m_dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");

			}

		}

		while (!m_dataconsmers.empty())
		{
			cDataConsmer cdata = m_dataconsmers.front();
			m_dataconsmers.pop_front();

			webrtc::DataChannelInit dataChannelInit;
			dataChannelInit.protocol = "chat";
			dataChannelInit.negotiated = true;
			dataChannelInit.id = ++m_streamId;
			webrtcDataChannel = m_peer_connection->CreateDataChannel(cdata.m_lable, &dataChannelInit);
			if (!webrtcDataChannel.get())
			{
				ERROR_EX_LOG("create data channel failed !!!");
				continue;
			}
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_consumsers.insert(std::make_pair(cdata.m_id, std::make_shared<cdataconsumer>(this)));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				continue; ;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", dataChannelInit.id                },
				{ "ordered",  dataChannelInit.ordered }
			};
			(pi.first->second)->init(cdata.m_id, cdata.m_dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");
		}
		m_recving = ERecv_Success;
	}

}
