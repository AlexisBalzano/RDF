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
	int rise = screenPos.y - center.y;
	int run = screenPos.x - center.x;

	int xedge = radarArea.left;
	int yedge = radarArea.bottom;
	int xsteps = 0;
	int ysteps = 0;

	if (run > 0) {
		xedge = radarArea.right;
		xsteps = ((xedge - center.x) / run);
		if (xsteps < 0) xsteps = -xsteps;
	}

	if (rise > 0) {
		yedge = radarArea.top;
		ysteps = ((yedge - center.y) / rise);
		if (ysteps < 0) ysteps = -ysteps;
	}

	if (xsteps == 0 && ysteps == 0) {
		return; // invalid coordinates or already at center
	}


	int steps = xsteps;

	if (ysteps > 0 && (ysteps < xsteps || xsteps == 0)) {
		steps = ysteps;
	}

	xedge = center.x + (run * steps);
	yedge = center.y + (rise * steps);

	screenPos.x = xedge;
	screenPos.y = yedge;
}
