#pragma once

#include "stdafx.h"
#include "RDFCommon.h"

auto RDFCommon::GetRGB(COLORREF& color, const std::string& settingValue) -> bool
{
	try {
		PLOGV << settingValue;
		std::regex rxRGB(R"(^(\d{1,3}):(\d{1,3}):(\d{1,3})$)");
		std::smatch match;
		if (std::regex_match(settingValue, match, rxRGB)) {
			UINT r = std::stoi(match[1].str());
			UINT g = std::stoi(match[2].str());
			UINT b = std::stoi(match[3].str());
			if (r <= 255 && g <= 255 && b <= 255) {
				color = RGB(r, g, b);
				return true;
			}
		}
	}
	catch (...) {
		PLOGE << "invalid RGB value";
	}
	return false;
}

auto RDFCommon::GetSettingOnOff(bool& on, const std::string& settingValue) -> bool
{
	// settingValue is uppercase
	if (settingValue == "ON" || settingValue == "1") {
		on = true;
		return true;
	}
	else if (settingValue == "OFF" || settingValue == "0") {
		on = false;
		return true;
	}
	return false;
}

// Virtual key names accepted by .RDF PREVTRANS. The first name of a code is the one written back
// to the settings file, the ones after it are aliases. Letters, digits, F1 to F24 and NUM0 to NUM9
// are not listed, GetKeyCode derives those from the name itself.
static const std::vector<std::pair<std::string, int>> keyNames = {
	// mouse, the side buttons are labelled 4 and 5 on most mice
	{ "XBUTTON1", VK_XBUTTON1 },
	{ "XBUTTON2", VK_XBUTTON2 },
	{ "LBUTTON", VK_LBUTTON },
	{ "RBUTTON", VK_RBUTTON },
	{ "MBUTTON", VK_MBUTTON },
	{ "MOUSE1", VK_LBUTTON },
	{ "MOUSE2", VK_RBUTTON },
	{ "MOUSE3", VK_MBUTTON },
	{ "MOUSE4", VK_XBUTTON1 },
	{ "MOUSE5", VK_XBUTTON2 },
	// modifiers
	{ "SHIFT", VK_SHIFT },
	{ "CTRL", VK_CONTROL },
	{ "ALT", VK_MENU },
	{ "CONTROL", VK_CONTROL },
	{ "MENU", VK_MENU },
	{ "LSHIFT", VK_LSHIFT },
	{ "RSHIFT", VK_RSHIFT },
	{ "LCTRL", VK_LCONTROL },
	{ "RCTRL", VK_RCONTROL },
	{ "LALT", VK_LMENU },
	{ "RALT", VK_RMENU },
	{ "LWIN", VK_LWIN },
	{ "RWIN", VK_RWIN },
	// editing and navigation
	{ "BACKSPACE", VK_BACK },
	{ "TAB", VK_TAB },
	{ "ENTER", VK_RETURN },
	{ "RETURN", VK_RETURN },
	{ "ESC", VK_ESCAPE },
	{ "ESCAPE", VK_ESCAPE },
	{ "SPACE", VK_SPACE },
	{ "PAGEUP", VK_PRIOR },
	{ "PAGEDOWN", VK_NEXT },
	{ "END", VK_END },
	{ "HOME", VK_HOME },
	{ "LEFT", VK_LEFT },
	{ "UP", VK_UP },
	{ "RIGHT", VK_RIGHT },
	{ "DOWN", VK_DOWN },
	{ "INSERT", VK_INSERT },
	{ "DELETE", VK_DELETE },
	{ "PRINTSCREEN", VK_SNAPSHOT },
	{ "PAUSE", VK_PAUSE },
	{ "CAPSLOCK", VK_CAPITAL },
	{ "NUMLOCK", VK_NUMLOCK },
	{ "SCROLLLOCK", VK_SCROLL },
	// numeric keypad, NUM0 to NUM9 are derived in GetKeyCode
	{ "NUMADD", VK_ADD },
	{ "NUMSUB", VK_SUBTRACT },
	{ "NUMMUL", VK_MULTIPLY },
	{ "NUMDIV", VK_DIVIDE },
	{ "NUMDEC", VK_DECIMAL },
};

auto RDFCommon::GetKeyCode(int& keyCode, const std::string& settingValue) -> bool
{
	// settingValue is uppercase and trimmed
	try {
		PLOGV << settingValue;
		if (settingValue.empty()) {
			return false;
		}
		if (settingValue.size() == 1) {
			// a letter or a digit is its own virtual key code
			char character = settingValue.front();
			if ((character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9')) {
				keyCode = (int)character;
				return true;
			}
			return false;
		}
		for (const auto& [name, code] : keyNames) {
			if (name == settingValue) {
				keyCode = code;
				return true;
			}
		}
		std::smatch match;
		std::regex rxFunction(R"(^F(\d{1,2})$)");
		if (std::regex_match(settingValue, match, rxFunction)) {
			int index = std::stoi(match[1].str());
			if (index >= 1 && index <= 24) {
				keyCode = VK_F1 + index - 1;
				return true;
			}
			return false;
		}
		std::regex rxNumpad(R"(^NUM(\d)$)");
		if (std::regex_match(settingValue, match, rxNumpad)) {
			keyCode = VK_NUMPAD0 + std::stoi(match[1].str());
			return true;
		}
		// raw virtual key code, the way out for anything without a name here
		std::regex rxCode(R"(^0X([0-9A-F]{1,2})$)");
		if (std::regex_match(settingValue, match, rxCode)) {
			int code = std::stoi(match[1].str(), nullptr, 16);
			if (code > 0) {
				keyCode = code;
				return true;
			}
		}
	}
	catch (...) {
		PLOGE << "invalid key value";
	}
	return false;
}

auto RDFCommon::GetKeyName(const int& keyCode) -> std::string
{
	if (keyCode >= VK_F1 && keyCode <= VK_F24) {
		return std::format("F{}", keyCode - VK_F1 + 1);
	}
	if (keyCode >= VK_NUMPAD0 && keyCode <= VK_NUMPAD9) {
		return std::format("NUM{}", keyCode - VK_NUMPAD0);
	}
	if ((keyCode >= 'A' && keyCode <= 'Z') || (keyCode >= '0' && keyCode <= '9')) {
		return std::string(1, (char)keyCode);
	}
	for (const auto& [name, code] : keyNames) {
		if (code == keyCode) {
			return name;
		}
	}
	return std::format("0X{:02X}", keyCode);
}

auto RDFCommon::AddOffset(EuroScopePlugIn::CPosition& position, const double& heading, const double& distance) -> void
{
	// from ES internal void CEuroScopeCoord :: Move ( double heading, double distance )
	if (distance < 0.000001)
		return;

	double m_Lat = position.m_Latitude;
	double m_Lon = position.m_Longitude;

	double distancePerR = distance / EarthRadius;
	double cosDistancePerR = cos(distancePerR);
	double sinDistnacePerR = sin(distancePerR);

	double fi2 = asin(sin(GEOM_RAD_FROM_DEG(m_Lat)) * cosDistancePerR + cos(GEOM_RAD_FROM_DEG(m_Lat)) * sinDistnacePerR * cos(GEOM_RAD_FROM_DEG(heading)));
	double lambda2 = GEOM_RAD_FROM_DEG(m_Lon) + atan2(sin(GEOM_RAD_FROM_DEG(heading)) * sinDistnacePerR * cos(GEOM_RAD_FROM_DEG(m_Lat)),
		cosDistancePerR - sin(GEOM_RAD_FROM_DEG(m_Lat)) * sin(fi2));

	position.m_Latitude = GEOM_DEG_FROM_RAD(fi2);
	position.m_Longitude = GEOM_DEG_FROM_RAD(lambda2);
}

// Extrapolate a point to the edge of the radar screen, given the radar area and the center point
auto RDFCommon::ExtrapolateToEdgeOfScreen(const RECT& radarArea, const POINT& center, POINT& screenPos) -> void
{
	const double run = screenPos.x - center.x;
	const double rise = screenPos.y - center.y;
	if (run == 0.0 && rise == 0.0) {
		return; // already at center, no direction to extrapolate
	}

	// Scale factor along (run, rise) that lands on each edge; the nearer crossing wins.
	double scaleX = 0.0, scaleY = 0.0;
	if (run > 0.0) scaleX = (radarArea.right - center.x) / run;
	else if (run < 0.0) scaleX = (radarArea.left - center.x) / run;
	if (rise > 0.0) scaleY = (radarArea.bottom - center.y) / rise;
	else if (rise < 0.0) scaleY = (radarArea.top - center.y) / rise;

	const double scale = (scaleX > 0.0 && scaleY > 0.0) ? min(scaleX, scaleY) : max(scaleX, scaleY);

	screenPos.x = center.x + (LONG)lround(run * scale);
	screenPos.y = center.y + (LONG)lround(rise * scale);
}
