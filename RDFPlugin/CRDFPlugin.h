#pragma once

#ifndef CRDFPLUGIN_H
#define CRDFPLUGIN_H

#include "stdafx.h"
#include "HiddenWindow.h"
#include "RDFCommon.h"
#include "CRDFScreen.h"

class CRDFPlugin : public EuroScopePlugIn::CPlugIn, public std::enable_shared_from_this<CRDFPlugin>
{
private:
	friend class CRDFScreen;

	// held down to draw the previous transmission again, see .RDF PREVTRANS
	std::atomic<int> prevTransButton = VK_XBUTTON1; // a side mouse button by default

	// directory
	std::filesystem::path dllPath;

	// screen controls and drawing params
	std::vector<std::shared_ptr<CRDFScreen>> vecScreen; // index is screen ID (incremental int)
	std::shared_mutex mtxDrawSettings;
	std::shared_ptr<RDFCommon::draw_settings> currentDrawSettings;
	std::string currentDrawStyle;

	// drawing center picked with .RDF VIS, shared by all screens
	std::shared_mutex mtxVisCenter;
	bool visPickMode = false; // true while waiting for the user to click a point on a radar screen
	std::optional<EuroScopePlugIn::CPosition> visCenter; // overrides the controller position as drawing center
	std::chrono::steady_clock::time_point visShowUntil; // .RDF SHOWVIS marks the center until then

	// drawing records and transmitting frequency records
	std::shared_mutex mtxTransmission;
	RDFCommon::callsign_position curTransmission;
	RDFCommon::callsign_position preTransmission;
	std::shared_mutex mtxTxFrequencies;
	std::set<int> curTxFrequencies; // in kHz

	// TrackAudio WebSocket
	std::string addressTrackAudio;
	ix::WebSocket socketTrackAudio;
	auto TrackAudioMessageHandler(const ix::WebSocketMessagePtr& msg) -> void;
	// IXWebSocket calls TrackAudioMessageHandler on its own thread and the EuroScope API is not thread safe,
	// so the handler only queues events and HiddenWndProcessTrackAudioEvents handles them on EuroScope's thread
	enum class TrackAudioEventType { Message, DisplaySilent, DisplayDebug, DisplayUnread };
	static constexpr size_t MaxQueuedTrackAudioEvents = 256;
	std::mutex mtxTrackAudioEvents;
	std::queue<std::pair<TrackAudioEventType, std::string>> trackAudioEvents;
	auto QueueTrackAudioEvent(TrackAudioEventType type, std::string payload) -> void;

	// AFV standalone client controls
	HWND hiddenWindowRDF = NULL;
	HWND hiddenWindowAFV = NULL;
	WNDCLASS windowClassRDF = {
	   NULL,
	   HiddenWindowRDF,
	   NULL,
	   NULL,
	   GetModuleHandle(NULL),
	   NULL,
	   NULL,
	   NULL,
	   NULL,
	   TEXT("RDFHiddenWindowClass")
	};
	WNDCLASS windowClassAFV = {
	   NULL,
	   HiddenWindowAFV,
	   NULL,
	   NULL,
	   GetModuleHandle(NULL),
	   NULL,
	   NULL,
	   NULL,
	   NULL,
	   TEXT("AfvBridgeHiddenWindowClass")
	};

	// settings related functions
	auto LoadTrackAudioSettings(void) -> void;
	auto LoadPrevTransSettings(void) -> void;
	auto LoadDrawingSettings(const std::optional<std::shared_ptr<CRDFScreen>>& screenPtr) -> void;
	auto LoadDrawingStyle(const std::string& styleName) -> bool;

	// functional things 
	auto GetBridgeMode(void) -> bool;
	auto GenerateDrawPosition(const std::string& callsign) -> RDFCommon::draw_position;
	auto TrackAudioTransmissionHandler(const nlohmann::json& data, const bool& rxEnd) -> void;
	auto TrackAudioStationStatesHandler(const nlohmann::json& data) -> void;
	auto TrackAudioStationStateUpdateHandler(const nlohmann::json& data) -> void;
	auto SelectGroundToAirChannel(const std::optional<std::string>& callsign, const std::optional<int>& frequency) -> EuroScopePlugIn::CGrountToAirChannel;
	auto UpdateChannel(const std::optional<std::string>& callsign, const std::optional<RDFCommon::chnl_state>& channelState) -> void;
	auto ToggleChannel(EuroScopePlugIn::CGrountToAirChannel Channel, const std::optional<bool>& rx, const std::optional<bool>& tx) -> void;

	// drawing center (.RDF VIS)
	auto GetVisPickMode(void) -> bool;
	auto SetVisPickMode(const bool& enabled) -> void;
	auto GetVisCenter(void) -> std::optional<EuroScopePlugIn::CPosition>;
	auto SetVisCenter(const std::optional<EuroScopePlugIn::CPosition>& position) -> void;
	auto ShowVisCenter(void) -> void;
	auto IsVisCenterShown(void) -> bool;
	auto RefreshScreens(void) -> void;

	// messages
	inline auto DisplayMessageDebug(const std::string& msg) -> void {
#ifdef _DEBUG
		DisplayUserMessage("RDF-DEBUG", "", msg.c_str(), true, true, true, false, false);
#endif // _DEBUG
	}
	inline auto DisplayMessageSilent(const std::string& msg) -> void {
		DisplayUserMessage("Message", "RDF Plugin", msg.c_str(), false, false, false, false, false);
	}
	inline auto DisplayMessageUnread(const std::string& msg) -> void {
		DisplayUserMessage("Message", "RDF Plugin", msg.c_str(), true, true, true, false, false);
	}

public:
	CRDFPlugin();
	~CRDFPlugin();
	auto GetDrawStations(void) -> RDFCommon::callsign_position;
	auto HiddenWndProcessRDFMessage(const std::string& message) -> void;
	auto HiddenWndProcessAFVMessage(const std::string& message) -> void;
	auto HiddenWndProcessTrackAudioEvents(void) -> void;
	auto HiddenWndProcessVisCenterTimeout(void) -> void;
	virtual auto OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated) -> EuroScopePlugIn::CRadarScreen*;
	virtual auto OnCompileCommand(const char* sCommandLine) -> bool;
	virtual auto OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, EuroScopePlugIn::CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize) -> void;
};

#endif // !CRDFPLUGIN_H
