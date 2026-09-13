#pragma once

#include "stdafx.h"

LRESULT CALLBACK HiddenWindowRDF(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK HiddenWindowAFV(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Posted to the RDF hidden window from the TrackAudio WebSocket thread, so queued events are handled on EuroScope's thread
constexpr UINT WM_RDF_TRACKAUDIO_EVENTS = WM_APP + 1;