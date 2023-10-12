/*
  ==============================================================================

	PropFlasherPanel.cpp
	Created: 1 Jan 2023 7:36:22pm
	Author:  bkupe

  ==============================================================================
*/

#include "Prop/PropIncludes.h"
#include "PropFlasherPanel.h"

PropFlasherPanel::PropFlasherPanel() :
	ShapeShifterContentComponent("Firmware Uploader")
{
	filterKnownDevicesUI.reset(PropFlasher::getInstance()->filterKnownDevices->createToggle());
	firmwareToUploadUI.reset((EnumParameterUI*)PropFlasher::getInstance()->fwType->createDefaultUI());
	firmwareCustomFileUI.reset((StringParameterFileUI*)PropFlasher::getInstance()->fwFileParam->createStringParameterFileUI());
	setWifiAfterFlashUI.reset(PropFlasher::getInstance()->setWifiAfterFlash->createToggle());
	wifiSSIDUI.reset(PropFlasher::getInstance()->wifiSSID->createStringParameterUI());
	wifiPassUI.reset(PropFlasher::getInstance()->wifiPass->createStringParameterUI());
	flashAllUI.reset(PropFlasher::getInstance()->flashTrigger->createButtonUI());
	progressUI.reset(PropFlasher::getInstance()->progression->createSlider());

	firmwareCustomFileUI->useCustomBGColor = true;
	firmwareCustomFileUI->customBGColor = BG_COLOR;
	firmwareCustomFileUI->updateUIParams();

	wifiSSIDUI->useCustomBGColor = true;
	wifiSSIDUI->customBGColor = BG_COLOR;
	wifiSSIDUI->updateUIParams();

	wifiPassUI->useCustomBGColor = true;
	wifiPassUI->customBGColor = BG_COLOR;
	wifiPassUI->updateUIParams();

	addAndMakeVisible(filterKnownDevicesUI.get());
	addAndMakeVisible(firmwareToUploadUI.get());
	addAndMakeVisible(firmwareCustomFileUI.get());
	addAndMakeVisible(setWifiAfterFlashUI.get());
	addAndMakeVisible(wifiSSIDUI.get());
	addAndMakeVisible(wifiPassUI.get());
	addAndMakeVisible(flashAllUI.get());
	addAndMakeVisible(progressUI.get());

	SerialManager::getInstance()->addSerialManagerListener(this);
	PropFlasher::getInstance()->filterKnownDevices->addAsyncCoalescedParameterListener(this);

	updateInfos();
}

PropFlasherPanel::~PropFlasherPanel()
{
	if (SerialManager::getInstanceWithoutCreating()) SerialManager::getInstance()->removeSerialManagerListener(this);
	if (PropFlasher::getInstanceWithoutCreating()) PropFlasher::getInstance()->filterKnownDevices->removeAsyncParameterListener(this);
}

void PropFlasherPanel::paint(Graphics& g)
{
	g.setColour(TEXT_COLOR);
	g.setFont(26);
	g.drawText("Firmware Uploader", getLocalBounds().reduced(10).withHeight(40), Justification::centred);

	g.setColour(BG_COLOR.brighter(.1f));
	g.fillRoundedRectangle(propInfosRect.toFloat(), 4);
	g.setColour(BG_COLOR.brighter(.05f));
	g.fillRoundedRectangle(controlRect.toFloat(), 4);


	g.setColour(BG_COLOR.brighter(.3f));
	g.drawRoundedRectangle(fwRect.toFloat(), 4, 1);
	g.drawRoundedRectangle(wifiRect.toFloat(), 4, 1);
	g.drawRoundedRectangle(flashRect.toFloat(), 4, 1);

	g.setColour(TEXT_COLOR);
	g.setFont(18);
	g.drawText("1. Choose Firmware", fwRect.withHeight(24), Justification::centred);
	g.drawText("2. Set Wifi Informations", wifiRect.withHeight(24), Justification::centred);
	g.drawText("3. Upload Firmware", flashRect.withHeight(24), Justification::centred);

	int numDevices = infos.size();

	Rectangle<int> r = propInfosRect.reduced(10);
	g.setColour(TEXT_COLOR);
	g.setFont(16);
	g.drawText(String(numDevices) + " Connected Devices", r.removeFromTop(30).toFloat(), Justification::centred);

	r.removeFromTop(10);

	g.setFont(12);
	for (auto& info : infos)
	{
		g.drawText(info->uniqueDescription, r.removeFromTop(16).toFloat(), Justification::centredLeft);
	};
}

void PropFlasherPanel::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(10);
	r.removeFromTop(40);

	propInfosRect = r.withWidth(jmin<int>(400, r.getWidth() * .3f));

	filterKnownDevicesUI->setBounds(propInfosRect.removeFromBottom(30).withSizeKeepingCentre(160, 20));

	controlRect = r.withLeft(propInfosRect.getRight() + 10);

	Rectangle<int> cr = controlRect.reduced(10);
	fwRect = cr.removeFromTop(68);
	Rectangle<int> fwr = fwRect.reduced(10);
	fwr.removeFromTop(20);
	firmwareToUploadUI->setBounds(fwr.removeFromLeft(200).reduced(4));
	fwr.removeFromLeft(20);
	firmwareCustomFileUI->setBounds(fwr.reduced(4));

	cr.removeFromTop(8);

	wifiRect = cr.removeFromTop(68);
	Rectangle<int> wr = wifiRect.reduced(10);
	wr.removeFromTop(20);

	setWifiAfterFlashUI->setBounds(wr.removeFromLeft(150).reduced(4));
	wr.removeFromLeft(70);
	wifiSSIDUI->setBounds(wr.removeFromLeft(200).reduced(4));
	wr.removeFromLeft(20);
	wifiPassUI->setBounds(wr.reduced(4));

	cr.removeFromTop(8);

	flashRect = cr.removeFromTop(140);
	Rectangle<int> fr = flashRect.reduced(10);
	fr.removeFromTop(20);
	flashAllUI->setBounds(fr.removeFromTop(70).withSizeKeepingCentre(jmin(fr.getWidth(), 400), 50));
	progressUI->setBounds(fr.removeFromBottom(40));
}


void PropFlasherPanel::updateInfos()
{
	infos = PropFlasher::getInstance()->getDevicesToFlash();
}


void PropFlasherPanel::portAdded(SerialDeviceInfo* info)
{
	updateInfos();
	repaint();
}



void PropFlasherPanel::portRemoved(SerialDeviceInfo* info)
{
	updateInfos();
	repaint();
}

void PropFlasherPanel::newMessage(const Parameter::ParameterEvent& e)
{
	if (e.parameter == PropFlasher::getInstance()->filterKnownDevices)
	{
		updateInfos();
		repaint();
	}
}

