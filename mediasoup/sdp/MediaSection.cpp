﻿/***********************************************************************************************
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
#include "MediaSection.hpp"
 
#include <algorithm> // ::transform
#include <cctype>    // ::tolower
#include <regex>
#include <sstream>
#include <utility>
#include "../json.hpp"
#include "../clog.h"
using json = nlohmann::json;

// Static functions declaration.
static std::string getCodecName(const json& codec);

namespace mediasoupclient
{
	namespace Sdp
	{
		MediaSection::MediaSection(const json& iceParameters, const json& iceCandidates)
		{
			 

			// Set ICE parameters.
			SetIceParameters(iceParameters);

			// Set ICE candidates.
			this->mediaObject["candidates"] = json::array();

			for (auto& candidate : iceCandidates)
			{
				auto candidateObject = json::object();

				// mediasoup does mandate rtcp-mux so candidates component is always
				// RTP (1).
				candidateObject["component"]  = 1;
				candidateObject["foundation"] = candidate["foundation"];
				candidateObject["ip"]         = candidate["ip"];
				candidateObject["port"]       = candidate["port"];
				candidateObject["priority"]   = candidate["priority"];
				candidateObject["transport"]  = candidate["protocol"];
				candidateObject["type"]       = candidate["type"];

				if (candidate.find("tcpType") != candidate.end())
					candidateObject["tcptype"] = candidate["tcpType"];

				this->mediaObject["candidates"].push_back(candidateObject);
			}

			this->mediaObject["endOfCandidates"] = "end-of-candidates";
			this->mediaObject["iceOptions"]      = "renomination";
		}

		std::string MediaSection::GetMid() const
		{
			

			return this->mediaObject["mid"].get<std::string>();
		}

		bool MediaSection::IsClosed() const
		{
			 
			return this->mediaObject["port"] == 0;
		}

		json MediaSection::GetObject() const
		{
			 
			return this->mediaObject;
		}

		void MediaSection::SetIceParameters(const json& iceParameters)
		{
			 
			this->mediaObject["iceUfrag"] = iceParameters["usernameFragment"];
			this->mediaObject["icePwd"]   = iceParameters["password"];
		}

		void MediaSection::Disable()
		{
			 
			this->mediaObject["direction"] = "inactive";

			this->mediaObject.erase("ext");
			this->mediaObject.erase("ssrcs");
			this->mediaObject.erase("ssrcGroups");
			this->mediaObject.erase("simulcast");
			this->mediaObject.erase("rids");
		}

		void MediaSection::Close()
		{
			 
			this->mediaObject["direction"] = "inactive";
			this->mediaObject["port"]      = 0;

			this->mediaObject.erase("ext");
			this->mediaObject.erase("ssrcs");
			this->mediaObject.erase("ssrcGroups");
			this->mediaObject.erase("simulcast");
			this->mediaObject.erase("rids");
			this->mediaObject.erase("extmapAllowMixed");
		}

		AnswerMediaSection::AnswerMediaSection(
		  const json& iceParameters,
		  const json& iceCandidates,
		  const json& dtlsParameters,
		  const json& sctpParameters,
		  const json& offerMediaObject,
		  json& offerRtpParameters,
		  json& answerRtpParameters,
		  const json* codecOptions)
		  : MediaSection(iceParameters, iceCandidates)
		{
			 
			auto type = offerMediaObject["type"].get<std::string>();

			this->mediaObject["mid"]        = offerMediaObject["mid"];
			this->mediaObject["type"]       = type;
			this->mediaObject["protocol"]   = offerMediaObject["protocol"];
			this->mediaObject["connection"] = { { "ip", "127.0.0.1" }, { "version", 4 } };
			this->mediaObject["port"]       = 7;

			// Set DTLS role.
			auto dtlsRole = dtlsParameters["role"].get<std::string>();

			if (dtlsRole == "client")
				this->mediaObject["setup"] = "active";
			else if (dtlsRole == "server")
				this->mediaObject["setup"] = "passive";
			else if (dtlsRole == "auto")
				this->mediaObject["setup"] = "actpass";

			if (type == "audio" || type == "video")
			{
				this->mediaObject["direction"] = "recvonly";
				this->mediaObject["rtp"]       = json::array();
				this->mediaObject["rtcpFb"]    = json::array();
				this->mediaObject["fmtp"]      = json::array();

				for (auto& codec : answerRtpParameters["codecs"])
				{
					// clang-format off
					json rtp =
					{
						{ "payload", codec["payloadType"] },
						{ "codec",   getCodecName(codec)  },
						{ "rate",    codec["clockRate"]   }
					};
					// clang-format on

					/*if (codec.contains("channels"))
					{
						auto channels = codec["channels"].get<uint8_t>();

						if (channels > 1)
							rtp["encoding"] = channels;
					}*/
					auto jsonChannelsIt = codec.find("channels");
					if (jsonChannelsIt != codec.end() && jsonChannelsIt->is_number_unsigned())
					{
						auto channels = jsonChannelsIt->get<uint8_t>();
						if (channels > 1)
							rtp["encoding"] = channels;
					}
					this->mediaObject["rtp"].push_back(rtp);

					json codecParameters = codec["parameters"];

					if (codecOptions != nullptr && !codecOptions->empty())
					{
						auto& offerCodecs = offerRtpParameters["codecs"];
						auto codecIt =
						  find_if(offerCodecs.begin(), offerCodecs.end(), [&codec](json& offerCodec) {
							  return offerCodec["payloadType"] == codec["payloadType"];
						  });

						auto& offerCodec = *codecIt;
						auto mimeType    = codec["mimeType"].get<std::string>();
						std::transform(mimeType.begin(), mimeType.end(), mimeType.begin(), ::tolower);

						if (mimeType == "audio/opus")
						{
							auto opusStereoIt = codecOptions->find("opusStereo");
							if (opusStereoIt != codecOptions->end())
							{
								auto opusStereo                          = opusStereoIt->get<bool>();
								offerCodec["parameters"]["sprop-stereo"] = opusStereo ? 1 : 0;
								codecParameters["stereo"]                = opusStereo ? 1 : 0;
							}

							auto opusFecIt = codecOptions->find("opusFec");
							if (opusFecIt != codecOptions->end())
							{
								auto opusFec                             = opusFecIt->get<bool>();
								offerCodec["parameters"]["useinbandfec"] = opusFec ? 1 : 0;
								codecParameters["useinbandfec"]          = opusFec ? 1 : 0;
							}

							auto opusDtxIt = codecOptions->find("opusDtx");
							if (opusDtxIt != codecOptions->end())
							{
								auto opusDtx                       = opusDtxIt->get<bool>();
								offerCodec["parameters"]["usedtx"] = opusDtx ? 1 : 0;
								codecParameters["usedtx"]          = opusDtx ? 1 : 0;
							}

							auto opusMaxPlaybackRateIt = codecOptions->find("opusMaxPlaybackRate");
							if (opusMaxPlaybackRateIt != codecOptions->end())
							{
								auto opusMaxPlaybackRate           = opusMaxPlaybackRateIt->get<uint32_t>();
								codecParameters["maxplaybackrate"] = opusMaxPlaybackRate;
							}

							auto opusPtimeIt = codecOptions->find("opusPtime");
							if (opusPtimeIt != codecOptions->end())
							{
								auto opusPtime           = opusPtimeIt->get<uint32_t>();
								codecParameters["ptime"] = opusPtime;
							}
						}
						else if (mimeType == "video/vp8" || mimeType == "video/vp9" || mimeType == "video/h264" || mimeType == "video/h265")
						{
							auto videoGoogleStartBitrateIt = codecOptions->find("videoGoogleStartBitrate");
							if (videoGoogleStartBitrateIt != codecOptions->end())
							{
								auto videoGoogleStartBitrate = videoGoogleStartBitrateIt->get<uint32_t>();
								codecParameters["x-google-start-bitrate"] = videoGoogleStartBitrate;
							}

							auto videoGoogleMaxBitrateIt = codecOptions->find("videoGoogleMaxBitrate");
							if (videoGoogleMaxBitrateIt != codecOptions->end())
							{
								auto videoGoogleMaxBitrate              = videoGoogleMaxBitrateIt->get<uint32_t>();
								codecParameters["x-google-max-bitrate"] = videoGoogleMaxBitrate;
							}

							auto videoGoogleMinBitrateIt = codecOptions->find("videoGoogleMinBitrate");
							if (videoGoogleMinBitrateIt != codecOptions->end())
							{
								auto videoGoogleMinBitrate              = videoGoogleMinBitrateIt->get<uint32_t>();
								codecParameters["x-google-min-bitrate"] = videoGoogleMinBitrate;
							}
						}
					}

					// clang-format off
					json fmtp =
					{
						{ "payload", codec["payloadType"] }
					};
					// clang-format on

					std::ostringstream config;

					for (auto& item : codecParameters.items())
					{
						if (!config.str().empty())
							config << ";";

						config << item.key();
						config << "=";
						if (item.value().is_string())
							config << item.value().get<std::string>();
						else if (item.value().is_number_float())
							config << item.value().get<float>();
						else if (item.value().is_number())
							config << item.value().get<int64_t>();
					}

					if (!config.str().empty())
					{
						fmtp["config"] = config.str();
						this->mediaObject["fmtp"].push_back(fmtp);
					}

					for (auto& fb : codec["rtcpFeedback"])
					{
						// clang-format off
						this->mediaObject["rtcpFb"].push_back(
							{
								{ "payload", codec["payloadType"] },
								{ "type",    fb["type"]           },
								{ "subtype", fb["parameter"]      }
							});
						// clang-format on
					}
				}

				std::string payloads;

				for (auto& codec : answerRtpParameters["codecs"])
				{
					auto payloadType = codec["payloadType"].get<uint8_t>();

					if (!payloads.empty())
						payloads.append(" ");

					payloads.append(std::to_string(payloadType));
				}

				this->mediaObject["payloads"] = payloads;
				this->mediaObject["ext"]      = json::array();

				// Don't add a header extension if not present in the offer.
				for (auto& ext : answerRtpParameters["headerExtensions"])
				{
					const auto& localExts = offerMediaObject["ext"];
					auto localExtIt = find_if(localExts.begin(), localExts.end(), [&ext](const json& localExt) {
						return localExt["uri"] == ext["uri"];
					});

					if (localExtIt == localExts.end())
						continue;

					// clang-format off
					this->mediaObject["ext"].push_back(
						{
							{ "uri",   ext["uri"] },
							{ "value", ext["id"]  }
						});
					// clang-format on
				}

				// Allow both 1 byte and 2 bytes length header extensions.
				auto extmapAllowMixedIt = offerMediaObject.find("extmapAllowMixed");

				// clang-format off
				if (
					extmapAllowMixedIt != offerMediaObject.end() &&
					extmapAllowMixedIt->is_string()
				)
				// clang-format on
				{
					this->mediaObject["extmapAllowMixed"] = "extmap-allow-mixed";
				}

				// Simulcast.
				auto simulcastId = offerMediaObject.find("simulcast");
				auto ridsIt      = offerMediaObject.find("rids");

				// clang-format off
				if (
					simulcastId != offerMediaObject.end() &&
					simulcastId->is_object() &&
					ridsIt->is_array()
				)
				// clang-format off
				{
					this->mediaObject["simulcast"] = json::object();
					this->mediaObject["simulcast"]["dir1"] = "recv";
					this->mediaObject["simulcast"]["list1"] = (*simulcastId)["list1"];

					this->mediaObject["rids"] = json::array();

					for (const auto& rid : *ridsIt)
					{
						if (rid["direction"] != "send")
							continue;

						// clang-format off
						this->mediaObject["rids"].push_back(
							{
								{ "id", rid["id"] },
								{ "direction", "recv" }
							});
						// clang-format on
					}
				}

				this->mediaObject["rtcpMux"]   = "rtcp-mux";
				this->mediaObject["rtcpRsize"] = "rtcp-rsize";
			}
			else if (type == "application")
			{
				this->mediaObject["payloads"]       = "webrtc-datachannel";
				this->mediaObject["sctpPort"]       = sctpParameters["port"];
				this->mediaObject["maxMessageSize"] = sctpParameters["maxMessageSize"];
			}
		}

		void AnswerMediaSection::SetDtlsRole(const std::string& role)
		{
			 
			if (role == "client")
				this->mediaObject["setup"] = "active";
			else if (role == "server")
				this->mediaObject["setup"] = "passive";
			else if (role == "auto")
				this->mediaObject["setup"] = "actpass";
		}

		OfferMediaSection::OfferMediaSection(
		  const json& iceParameters,
		  const json& iceCandidates,
		  const json& /*dtlsParameters*/,
		  const json& sctpParameters,
		  const std::string& mid,
		  const std::string& kind,
		  const json& offerRtpParameters,
		  const std::string& streamId,
		  const std::string& trackId)
		  : MediaSection(iceParameters, iceCandidates)
		{
			
			this->mediaObject["mid"]  = mid;
			this->mediaObject["type"] = kind;

			if (sctpParameters == nullptr)
				this->mediaObject["protocol"] = "UDP/TLS/RTP/SAVPF";
			else
				this->mediaObject["protocol"] = "UDP/DTLS/SCTP";

			this->mediaObject["connection"] = { { "ip", "127.0.0.1" }, { "version", 4 } };
			this->mediaObject["port"]       = 7;

			// Set DTLS role.
			this->mediaObject["setup"] = "actpass";

			if (kind == "audio" || kind == "video")
			{
				this->mediaObject["direction"] = "sendonly";
				this->mediaObject["rtp"]       = json::array();
				this->mediaObject["rtcpFb"]    = json::array();
				this->mediaObject["fmtp"]      = json::array();

				for (const auto& codec : offerRtpParameters["codecs"])
				{
					// clang-format off
					json rtp =
					{
						{ "payload", codec["payloadType"] },
						{ "codec",   getCodecName(codec)  },
						{ "rate",    codec["clockRate"]   }
					};
					// clang-format on

					if (codec.contains("channels"))
					{
						auto channels = codec["channels"].get<uint8_t>();

						if (channels > 1)
							rtp["encoding"] = channels;
					}

					this->mediaObject["rtp"].push_back(rtp);

					const json& codecParameters = codec["parameters"];

					// clang-format off
					json fmtp =
					{
						{ "payload", codec["payloadType"] }
					};
					// clang-format on

					std::ostringstream config;

					for (auto& item : codecParameters.items())
					{
						if (!config.str().empty())
							config << ";";

						config << item.key();
						config << "=";
						if (item.value().is_string())
							config << item.value().get<std::string>();
						else if (item.value().is_number_float())
							config << item.value().get<float>();
						else if (item.value().is_number())
							config << item.value().get<int64_t>();
					}

					if (!config.str().empty())
					{
						fmtp["config"] = config.str();
						this->mediaObject["fmtp"].push_back(fmtp);
					}

					for (const auto& fb : codec["rtcpFeedback"])
					{
						// clang-format off
						this->mediaObject["rtcpFb"].push_back(
							{
								{ "payload", codec["payloadType"] },
								{ "type",    fb["type"]           },
								{ "subtype", fb["parameter"]      }
							});
						// clang-format on
					}
				}

				std::string payloads;

				for (const auto& codec : offerRtpParameters["codecs"])
				{
					auto payloadType = codec["payloadType"].get<uint8_t>();

					if (!payloads.empty())
						payloads.append(" ");

					payloads.append(std::to_string(payloadType));
				}

				this->mediaObject["payloads"] = payloads;
				this->mediaObject["ext"]      = json::array();

				for (const auto& ext : offerRtpParameters["headerExtensions"])
				{
					// clang-format off
					this->mediaObject["ext"].push_back(
						{
							{ "uri",   ext["uri"] },
							{ "value", ext["id"]  }
						});
					// clang-format on
				}

				this->mediaObject["rtcpMux"]   = "rtcp-mux";
				this->mediaObject["rtcpRsize"] = "rtcp-rsize";

				const auto& encoding = offerRtpParameters["encodings"][0];
				auto ssrc            = encoding["ssrc"].get<uint32_t>();
				uint32_t rtxSsrc;

				auto rtxIt = encoding.find("rtx");
				if ((rtxIt != encoding.end()) && ((*rtxIt).find("ssrc") != (*rtxIt).end()))
					rtxSsrc = encoding["rtx"]["ssrc"].get<uint32_t>();
				else
					rtxSsrc = 0u;

				this->mediaObject["ssrcs"]      = json::array();
				this->mediaObject["ssrcGroups"] = json::array();

				auto cnameIt = offerRtpParameters["rtcp"].find("cname");
				if (cnameIt != offerRtpParameters["rtcp"].end() && cnameIt->is_string())
				{
					auto cname = (*cnameIt).get<std::string>();

					std::string msid(streamId);
					msid.append(" ").append(trackId);

					this->mediaObject["ssrcs"].push_back(
					  { { "id", ssrc }, { "attribute", "cname" }, { "value", cname } });

					this->mediaObject["ssrcs"].push_back(
					  { { "id", ssrc }, { "attribute", "msid" }, { "value", msid } });

					if (rtxSsrc != 0u)
					{
						std::string ssrcs = std::to_string(ssrc).append(" ").append(std::to_string(rtxSsrc));

						this->mediaObject["ssrcs"].push_back(
						  { { "id", rtxSsrc }, { "attribute", "cname" }, { "value", cname } });

						this->mediaObject["ssrcs"].push_back(
						  { { "id", rtxSsrc }, { "attribute", "msid" }, { "value", msid } });

						// Associate original and retransmission SSRCs.
						this->mediaObject["ssrcGroups"].push_back({ { "semantics", "FID" }, { "ssrcs", ssrcs } });
					}
				}
			}
			else if (kind == "application")
			{
				this->mediaObject["payloads"]       = "webrtc-datachannel";
				this->mediaObject["sctpPort"]       = sctpParameters["port"];
				this->mediaObject["maxMessageSize"] = sctpParameters["maxMessageSize"];
			}
		}

		void OfferMediaSection::SetDtlsRole(const std::string& /* role */)
		{
			 
			// The SDP offer must always have a=setup:actpass.
			this->mediaObject["setup"] = "actpass";
		}
	} // namespace Sdp
} // namespace mediasoupclient

// Private helpers used in this file.

static std::string getCodecName(const json& codec)
{
	static const std::regex MimeTypeRegex(
	  "^(audio|video)/", std::regex_constants::ECMAScript | std::regex_constants::icase);

	auto mimeType = codec["mimeType"].get<std::string>();

	return std::regex_replace(mimeType, MimeTypeRegex, "");
}
