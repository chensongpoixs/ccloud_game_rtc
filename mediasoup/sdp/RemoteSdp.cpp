/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/

#include "RemoteSdp.hpp"
 
#include "../sdptransform.hpp"
#include "../clog.h"
using json = nlohmann::json;

namespace mediasoupclient
{
	/* Sdp::RemoteSdp methods */

	Sdp::RemoteSdp::RemoteSdp(
	  const json& iceParameters_,
	  const json& iceCandidates_,
	  const json& dtlsParameters_,
	  const json& sctpParameters_)
	  : iceParameters(iceParameters_), iceCandidates(iceCandidates_), dtlsParameters(dtlsParameters_),
	    sctpParameters(sctpParameters_)
	{
	 
		 
		
		// clang-format off
		this->sdpObject =
		{
			{ "version", 0 },
			{ "origin",
				{
					{ "address",        "0.0.0.0"                        },
					{ "ipVer",          4                                },
					{ "netType",        "IN"                             },
					{ "sessionId",      10000                            },
					{ "sessionVersion", 0                                },
					{ "username",       "chensong"             }
				}
			},
			{ "name", "-" },
		  { "timing",
				{
					{ "start", 0 },
					{ "stop",  0 }
				}
			},
			{ "media", json::array() }
		};
		// clang-format on

		// If ICE parameters are given, add ICE-Lite indicator.
		if (this->iceParameters.find("iceLite") != this->iceParameters.end())
			this->sdpObject["icelite"] = "ice-lite";

		// clang-format off
		this->sdpObject["msidSemantic"] =
		{
			{ "semantic", "WMS" },
			{ "token",    "*"   }
		};
		// clang-format on

		// NOTE: We take the latest fingerprint.
		auto numFingerprints = this->dtlsParameters["fingerprints"].size();

		this->sdpObject["fingerprint"] = {
			{ "type", this->dtlsParameters.at("fingerprints")[numFingerprints - 1]["algorithm"] },
			{ "hash", this->dtlsParameters.at("fingerprints")[numFingerprints - 1]["value"] }
		};

		// clang-format off
		this->sdpObject["groups"] =
		{
			{
				{ "type", "BUNDLE" },
				{ "mids", ""       }
			}
		};
		using namespace chen;
		//NORMAL_EX_LOG("dtlsParameters = %s", dtlsParameters.dump().c_str());
		// clang-format on
	}

	Sdp::RemoteSdp::~RemoteSdp()
	{
		 
		for (const auto* mediaSection : this->mediaSections)
		{
			delete mediaSection;
		}
	}

	void Sdp::RemoteSdp::UpdateIceParameters(const json& iceParameters)
	{
		 

		this->iceParameters = iceParameters;

		if (iceParameters.find("iceLite") != iceParameters.end())
			sdpObject["icelite"] = "ice-lite";

		for (auto idx{ 0u }; idx < this->mediaSections.size(); ++idx)
		{
			auto* mediaSection = this->mediaSections[idx];

			mediaSection->SetIceParameters(iceParameters);

			// Update SDP media section.
			this->sdpObject["media"][idx] = mediaSection->GetObject();
		}
	}

	void Sdp::RemoteSdp::UpdateDtlsRole(const std::string& role)
	{
		 
		{
			using namespace chen;
			NORMAL_EX_LOG("dtlsParameters = %s", dtlsParameters.dump().c_str());
		}
		dtlsParameters["role"] = role;

		if (iceParameters.find("iceLite") != iceParameters.end())
			sdpObject["icelite"] = "ice-lite";

		for (auto idx{ 0u }; idx < this->mediaSections.size(); ++idx)
		{
			auto* mediaSection = this->mediaSections[idx];

			mediaSection->SetDtlsRole(role);

			// Update SDP media section.
			this->sdpObject["media"][idx] = mediaSection->GetObject();
		}
	}

	Sdp::RemoteSdp::MediaSectionIdx Sdp::RemoteSdp::GetNextMediaSectionIdx()
	{
		 

		// If a closed media section is found, return its index.
		for (auto idx{ 0u }; idx < this->mediaSections.size(); ++idx)
		{
			auto* mediaSection = this->mediaSections[idx];

			if (mediaSection->IsClosed())
				return { idx, mediaSection->GetMid() };
		}

		// If no closed media section is found, return next one.
		return { this->mediaSections.size() };
	}

	void Sdp::RemoteSdp::Send(
	  json& offerMediaObject,
	  const std::string& reuseMid,
	  json& offerRtpParameters,
	  json& answerRtpParameters,
	  const json* codecOptions)
	{
	 

		auto* mediaSection = new AnswerMediaSection(
		  this->iceParameters,
		  this->iceCandidates,
		  this->dtlsParameters,
		  this->sctpParameters,
		  offerMediaObject,
		  offerRtpParameters,
		  answerRtpParameters,
		  codecOptions);

		// Closed media section replacement.
		if (!reuseMid.empty())
		{
			this->ReplaceMediaSection(mediaSection, reuseMid);
		}
		else
		{
			this->AddMediaSection(mediaSection);
		}
	}

	void Sdp::RemoteSdp::SendSctpAssociation(json& offerMediaObject)
	{
		nlohmann::json emptyJson;
		auto* mediaSection = new AnswerMediaSection(
		  this->iceParameters,
		  this->iceCandidates,
		  this->dtlsParameters,
		  this->sctpParameters,
		  offerMediaObject,
		  emptyJson,
		  emptyJson,
		  nullptr);

		this->AddMediaSection(mediaSection);
	}

	void Sdp::RemoteSdp::RecvSctpAssociation()
	{
		nlohmann::json emptyJson;
		auto* mediaSection = new OfferMediaSection(
		  this->iceParameters,
		  this->iceCandidates,
		  this->dtlsParameters,
		  this->sctpParameters,
		  "datachannel", // mid
		  "application", // kind
		  emptyJson,     // offerRtpParameters
		  "",            // streamId
		  ""             // trackId
		);
		this->AddMediaSection(mediaSection);
	}

	void Sdp::RemoteSdp::Receive(
	  const std::string& mid,
	  const std::string& kind,
	  const json& offerRtpParameters,
	  const std::string& streamId,
	  const std::string& trackId)
	{
		 

		auto* mediaSection = new OfferMediaSection(
		  this->iceParameters,
		  this->iceCandidates,
		  this->dtlsParameters,
		  nullptr, // sctpParameters must be null here.
		  mid,
		  kind,
		  offerRtpParameters,
		  streamId,
		  trackId);

		this->AddMediaSection(mediaSection);
	}

	void Sdp::RemoteSdp::DisableMediaSection(const std::string& mid)
	{
		 

		const auto idx     = this->midToIndex[mid];
		auto* mediaSection = this->mediaSections[idx];

		mediaSection->Disable();
	}

	void Sdp::RemoteSdp::CloseMediaSection(const std::string& mid)
	{
		 

		const auto idx     = this->midToIndex[mid];
		auto* mediaSection = this->mediaSections[idx];

		// NOTE: Closing the first m section is a pain since it invalidates the
		// bundled transport, so let's avoid it.
		if (mid == this->firstMid)
			mediaSection->Disable();
		else
			mediaSection->Close();

		// Update SDP media section.
		this->sdpObject["media"][idx] = mediaSection->GetObject();

		// Regenerate BUNDLE mids.
		this->RegenerateBundleMids();
	}

	std::string Sdp::RemoteSdp::GetSdp()
	{
		 

		// Increase SDP version.
		auto version = this->sdpObject["origin"]["sessionVersion"].get<uint32_t>();

		this->sdpObject["origin"]["sessionVersion"] = ++version;

		return sdptransform::write(this->sdpObject);
	}

	void Sdp::RemoteSdp::AddMediaSection(MediaSection* newMediaSection)
	{
		 

		if (this->firstMid.empty())
			this->firstMid = newMediaSection->GetMid();

		// Add it in the vector.
		this->mediaSections.push_back(newMediaSection);

		// Add to the map.
		this->midToIndex[newMediaSection->GetMid()] = this->mediaSections.size() - 1;

		// Add to the SDP object.
		this->sdpObject["media"].push_back(newMediaSection->GetObject());

		this->RegenerateBundleMids();
	}

	void Sdp::RemoteSdp::ReplaceMediaSection(MediaSection* newMediaSection, const std::string& reuseMid)
	{
		 

		// Store it in the map.
		if (!reuseMid.empty())
		{
			const auto idx              = this->midToIndex[reuseMid];
			auto* const oldMediaSection = this->mediaSections[idx];

			// Replace the index in the vector with the new media section.
			this->mediaSections[idx] = newMediaSection;

			// Update the map.
			this->midToIndex.erase(oldMediaSection->GetMid());
			this->midToIndex[newMediaSection->GetMid()] = idx;

			// Delete old MediaSection.
			delete oldMediaSection;

			// Update the SDP object.
			this->sdpObject["media"][idx] = newMediaSection->GetObject();

			// Regenerate BUNDLE mids.
			this->RegenerateBundleMids();
		}
		else
		{
			const auto idx              = this->midToIndex[newMediaSection->GetMid()];
			auto* const oldMediaSection = this->mediaSections[idx];

			// Replace the index in the vector with the new media section.
			this->mediaSections[idx] = newMediaSection;

			// Delete old MediaSection.
			delete oldMediaSection;

			// Update the SDP object.
			this->sdpObject["media"][this->mediaSections.size() - 1] = newMediaSection->GetObject();
		}
	}

	void Sdp::RemoteSdp::RegenerateBundleMids()
	{
		 

		std::string mids;

		for (const auto* mediaSection : this->mediaSections)
		{
			if (!mediaSection->IsClosed())
			{
				if (mids.empty())
					mids = mediaSection->GetMid();
				else
					mids.append(" ").append(mediaSection->GetMid());
			}
		}

		this->sdpObject["groups"][0]["mids"] = mids;
	}
} // namespace mediasoupclient
