/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/**
 * @file WebViewLoveSMS.cpp
 * @author Mikael Kindborg
 *
 * Application for sending Love SMSs.
 *
 * This program illustrates how to use WebView for the
 * user interface of a MoSync C++ application.
 *
 * The HTML/CSS/JavaScript layer is rather thin, and the
 * application logic is implemented in C++. An alternative
 * design is to keep the C++ layer as thin as possible,
 * and implemented the application logic in JavaScript.
 *
 * By "application logic" is meant
 */

#include <ma.h>
#include <maheap.h>
#include <mastring.h>
#include <mavsprintf.h>
#include <MAUtil/String.h>
#include <IX_WIDGET.h>
#include <conprint.h>
#include "MAHeaders.h"
#include "WebViewUtil.h"

using namespace MoSync::UI;

class WebViewLoveSMSApp
{
private:
	MAWidgetHandle mScreen;
	MAWidgetHandle mWebView;
	PlatformUtil* mPlatformUtil;
	MAUtil::String mLoveMessage;
	MAUtil::String mKissMessage;

public:
	WebViewLoveSMSApp()
	{
		createUI();
		createMessageStrings();
	}

	virtual ~WebViewLoveSMSApp()
	{
		destroyUI();
	}

	void createMessageStrings()
	{
		// Create a message string with lots of
		// heart smileys!
		mLoveMessage = "";
		while (mLoveMessage.length() < 150)
		{
			mLoveMessage += "<3 ";
		}

		// Create a message string with lots of
		// kiss smileys!
		mKissMessage = "";
		while (mKissMessage.length() < 150)
		{
			mKissMessage += ":-* ";
		}
	}

	void createUI()
	{
		PlatformUtil::checkNativeUISupport();

		if (PlatformUtil::isAndroid())
		{
			mPlatformUtil = new PlatformUtilAndroid();
		}
		else if (PlatformUtil::isIOS())
		{
			mPlatformUtil = new PlatformUtilIOS();
		}

		MAUtil::String html =
			mPlatformUtil->getTextFromDataHandle(MainPage_html);

		// Create screen.
		mScreen = maWidgetCreate(MAW_SCREEN);
		widgetShouldBeValid(mScreen, "Could not create screen");

		// Create web view.
		mWebView = createWebView(html);

		// TODO: Use save phone no. insert via javascript: call
		// when page is loaded.

		// Compose objects.
		maWidgetAddChild(mScreen, mWebView);

		// Show the screen.
		maWidgetScreenShow(mScreen);
	}

	MAWidgetHandle createWebView(const MAUtil::String& html)
	{
		// Create web view
		MAWidgetHandle webView = maWidgetCreate(MAW_WEB_VIEW);
		widgetShouldBeValid(webView, "Could not create web view");

		// Set size of vew view to fill the parent.
		maWidgetSetProperty(webView, "width", "-1");
		maWidgetSetProperty(webView, "height", "-1");

		// Enable zooming.
		maWidgetSetProperty(webView, "enableZoom", "true");

		// Set the HTML the web view displays.
		maWidgetSetProperty(webView, "html", html.c_str());

		WebViewMessage::getMessagesFor(webView);

		return webView;
	}

	void destroyUI()
	{
		maWidgetDestroy(mScreen);
		delete mPlatformUtil;
	}

	void runEventLoop()
	{
		MAEvent event;

		bool isRunning = true;
		while (isRunning)
		{
			maWait(0);
			maGetEvent(&event);
			switch (event.type)
			{
				case EVENT_TYPE_CLOSE:
					isRunning = false;
					break;

				case EVENT_TYPE_KEY_PRESSED:
					if (event.key == MAK_MENU)
					{
						// TODO: Show menu in HTML/JS.
					}
					if (event.key == MAK_BACK)
					{
						isRunning = false;
					}
					break;

				case EVENT_TYPE_WIDGET:
					handleWidgetEvent((MAWidgetEventData*) event.data);
					break;
			}
		}
	}

	void handleWidgetEvent(MAWidgetEventData* widgetEvent)
	{
		// Handle messages from the WebView widget.
		if (MAW_EVENT_WEB_VIEW_HOOK_INVOKED == widgetEvent->eventType &&
			MAW_CONSTANT_HARD == widgetEvent->hookType)
		{
			// Get message.
			WebViewMessage message(widgetEvent->urlData);

			if (message.is("SendLoveSMS"))
			{
				// Save phone no and send SMS.
				savePhoneNoAndSendSMS(
					message.getParam(0),
					mLoveMessage);
			}

			if (message.is("SendKissSMS"))
			{
			}

			if (message.is("PageLoaded"))
			{
				// Set saved phone number here.
			}
		}
	}

	void savePhoneNoAndSendSMS(
		const MAUtil::String& phoneNo,
		const MAUtil::String&  message)
	{
		lprintfln("*** SMS to: %s", phoneNo.c_str());
		lprintfln("*** SMS to cleaned: %s", cleanPhoneNo(phoneNo.c_str()).c_str());
		lprintfln("*** SMS data: %s", message.c_str());
	}

	/**
	 * Filter out everything except digits in the phone number.
	 */
	MAUtil::String cleanPhoneNo(const MAUtil::String& phoneNo)
	{
		MAUtil::String cleanPhoneNo = "";

		for (int i = 0; i < phoneNo.length(); ++i)
		{
			char character = phoneNo[i];
			if (isDigit(character))
			{
				cleanPhoneNo += character;
			}
		}

		return cleanPhoneNo;
	}

	bool isDigit(char ch)
	{
		return
			'0' == ch ||
			'1' == ch ||
			'2' == ch ||
			'3' == ch ||
			'4' == ch ||
			'5' == ch ||
			'6' == ch ||
			'7' == ch ||
			'8' == ch ||
			'9' == ch;
	}

//	void handleGeoLocationEvent(MALocation* location)
//	{
//		char script[512];
//		sprintf(
//			script,
//			"javascript:GeoLocationUpdated('%f','%f');",
//			location->lat,
//			location->lon);
//		maWidgetSetProperty(mWebView, "url", script);
//	}

	void widgetShouldBeValid(MAWidgetHandle widget, const char* panicMessage)
	{
		if (widget <= 0)
		{
			maPanic(0, panicMessage);
		}
	}
};
// End of class WebViewLoveSMSApp

/**
 * Main function that is called when the program starts.
 */
extern "C" int MAMain()
{
	WebViewLoveSMSApp app;
	app.runEventLoop();
	return 0;
}
