#include "stdafx.h"
#include "HiddenWindow.h"
#include "CRDFPlugin.h"


CRDFPlugin* rdfPlugin;

LRESULT CALLBACK HiddenWindowRDF(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {
		rdfPlugin = reinterpret_cast<CRDFPlugin*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		return TRUE;
	}
	case WM_COPYDATA: {
		COPYDATASTRUCT* data = reinterpret_cast<COPYDATASTRUCT*>(lParam);

		if (data != nullptr && data->dwData == 666 && data->lpData != nullptr && rdfPlugin != nullptr) {
			rdfPlugin->HiddenWndProcessRDFMessage(reinterpret_cast<const char*>(data->lpData));
		}
		return TRUE;
	}
	case WM_RDF_TRACKAUDIO_EVENTS: {
		if (rdfPlugin != nullptr) {
			rdfPlugin->HiddenWndProcessTrackAudioEvents();
		}
		return 0;
	}
	case WM_TIMER: {
		if (wParam == TIMER_RDF_VIS_CENTER) {
			KillTimer(hwnd, TIMER_RDF_VIS_CENTER); // one shot
			if (rdfPlugin != nullptr) {
				rdfPlugin->HiddenWndProcessVisCenterTimeout();
			}
		}
		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK HiddenWindowAFV(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {
		rdfPlugin = reinterpret_cast<CRDFPlugin*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		return TRUE;
	}
	case WM_COPYDATA: {
		COPYDATASTRUCT* data = reinterpret_cast<COPYDATASTRUCT*>(lParam);

		if (data != nullptr && data->dwData == 666 && data->lpData != nullptr && rdfPlugin != nullptr) {
			rdfPlugin->HiddenWndProcessAFVMessage(reinterpret_cast<const char*>(data->lpData));
		}
		return TRUE;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


