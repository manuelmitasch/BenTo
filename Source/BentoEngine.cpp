/*
  ==============================================================================

	BentoEngine.cpp
	Created: 10 Apr 2018 5:14:40pm
	Author:  Ben

  ==============================================================================
*/

#include "BentoEngine.h"

#include "LightBlock/LightBlockIncludes.h"
#include "Prop/PropIncludes.h"
#include "Video/VideoIncludes.h"
#include "Node/NodeIncludes.h"

#include "Audio/AudioManager.h"
#include "Common/Serial/SerialManager.h"
//#include "WebServer/BentoWebServer.h"
#include "BentoSettings.h"


BentoEngine::BentoEngine() :
	Engine("BenTo", ".bento"),
	ioCC("Input - Output")
{
	Engine::mainEngine = this;

	addChildControllableContainer(LightBlockModelLibrary::getInstance());
	addChildControllableContainer(PropManager::getInstance());

	remoteHost = ioCC.addStringParameter("Remote Host", "Global remote host to send OSC to", "127.0.0.1");
	remotePort = ioCC.addIntParameter("Remote port", "Remote port to send OSC to", 43001, 1024, 65535); 
	globalSender.connect("0.0.0.0", 1024);

	ProjectSettings::getInstance()->addChildControllableContainer(&ioCC);
	ProjectSettings::getInstance()->addChildControllableContainer(AudioManager::getInstance());
	
	
	//Communication
	OSCRemoteControl::getInstance()->localPort->defaultValue = 43000;
	OSCRemoteControl::getInstance()->localPort->resetValue();

	OSCRemoteControl::getInstance()->addRemoteControlListener(this);
	SerialManager::getInstance(); // init


	GlobalSettings::getInstance()->addChildControllableContainer(BentoSettings::getInstance());


	//BentoWebServer::getInstance(); //init

}

BentoEngine::~BentoEngine()
{
	PropManager::deleteInstance();
	PropShapeLibrary::deleteInstance();

	SerialManager::deleteInstance();

	NodeFactory::deleteInstance();
	LightBlockModelLibrary::deleteInstance();

	AudioManager::deleteInstance();

	//BentoWebServer::deleteInstance();

	BentoSettings::deleteInstance();

	ZeroconfManager::deleteInstance();
}

void BentoEngine::clearInternal()
{
	PropManager::getInstance()->clear();
	LightBlockModelLibrary::getInstance()->clear();
}


void BentoEngine::processMessage(const OSCMessage & m)
{
	StringArray aList;
	aList.addTokens(m.getAddressPattern().toString(), "/", "\"");

	if (aList.size() < 2) return;

	if (aList[1] == "model")
	{
		String modelName = OSCHelpers::getStringArg(m[0]);
		LightBlockModel * lm = LightBlockModelLibrary::getInstance()->getModelWithName(modelName);

		if (lm != nullptr)
		{
			if (aList[2] == "assign")
			{
				if (m.size() >= 2)
				{
					int id = OSCHelpers::getIntArg(m[1]);

					LightBlockModelPreset * mp = nullptr;
					if (m.size() >= 3)
					{
						String presetName = OSCHelpers::getStringArg(m[2]);
						mp = lm->presetManager.getItemWithName(presetName);
					}

					LightBlockColorProvider * providerToAssign = mp != nullptr ? mp : (LightBlockColorProvider *)lm;
					if (id == -1)
					{
						for(auto & p:PropManager::getInstance()->items)  p->activeProvider->setValueFromTarget(providerToAssign);
					}
					else
					{
						Prop * p = PropManager::getInstance()->getPropWithId(id);
						if (p != nullptr) p->activeProvider->setValueFromTarget(providerToAssign);
					}
					
				}


			}
		}

	} else if (aList[1] == "prop")
	{
		int id = aList[2] == "all" ? -1 : aList[2].getIntValue();
		
		String localAddress = "/" + aList.joinIntoString("/", 3);
		OSCMessage lm(localAddress);
		lm.addString(""); //fake ID
		for (auto& a : m) lm.addArgument(a);
		lm.setAddressPattern(localAddress);

		if (id == -1)
		{
			for (auto & p : PropManager::getInstance()->items)  p->handleOSCMessage(lm);
		}
		else
		{
			if(Prop * p = PropManager::getInstance()->getPropWithId(id)) p->handleOSCMessage(lm);
		}
	}
}


var BentoEngine::getJSONData()
{
	var data = Engine::getJSONData();

	var mData = LightBlockModelLibrary::getInstance()->getJSONData();
	if (!mData.isVoid() && mData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("models", mData);

	
	var propData = PropManager::getInstance()->getJSONData();
	if (!propData.isVoid() && propData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("props", propData);

	return data;
}

void BentoEngine::loadJSONDataInternalEngine(var data, ProgressTask * loadingTask)
{
	//ProgressTask * projectTask = loadingTask->addTask("Project"
	ProgressTask * modelsTask = loadingTask->addTask("Models");
	ProgressTask * propTask = loadingTask->addTask("Props");

	
	modelsTask->start();
	LightBlockModelLibrary::getInstance()->loadJSONData(data.getProperty("models", var()));
	modelsTask->setProgress(1);
	modelsTask->end();

	
	propTask->start();
	PropManager::getInstance()->loadJSONData(data.getProperty("props", var()));
	propTask->setProgress(1);
	propTask->end();	


}
