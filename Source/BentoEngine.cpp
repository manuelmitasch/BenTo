/*
  ==============================================================================

	BentoEngine.cpp
	Created: 10 Apr 2018 5:14:40pm
	Author:  Ben

  ==============================================================================
*/

#include "BentoEngine.h"

#include "LightBlock/model/LightBlockModelLibrary.h"
#include "Prop/PropManager.h"
#include "Video/Spatializer.h"
#include "Audio/AudioManager.h"
#include "Timeline/layers/Block/LightBlockLayer.h"
#include "Common/Serial/SerialManager.h"
//#include "WebServer/BentoWebServer.h"
#include "Prop/Cluster/PropClusterGroupManager.h"


BentoEngine::BentoEngine() :
	Engine("BenTo", ".bento"),
	ioCC("Input - Output")
{
	Engine::mainEngine = this;

	addChildControllableContainer(LightBlockModelLibrary::getInstance());
	addChildControllableContainer(PropManager::getInstance());
	addChildControllableContainer(PropClusterGroupManager::getInstance());

	remoteHost = ioCC.addStringParameter("Remote Host", "Global remote host to send OSC to", "127.0.0.1");
	remotePort = ioCC.addIntParameter("Remote port", "Remote port to send OSC to", 43001, 1024, 65535); 
	globalSender.connect("0.0.0.0", 1024);

	ProjectSettings::getInstance()->addChildControllableContainer(&ioCC);
	ProjectSettings::getInstance()->addChildControllableContainer(AudioManager::getInstance());
	
	//Timeline
	SequenceLayerFactory::getInstance()->layerDefs.add(SequenceLayerDefinition::createDef("Blocks", &LightBlockLayer::create));
	SequenceLayerFactory::getInstance()->layerDefs.add(SequenceLayerDefinition::createDef("Audio", &AudioLayer::create));
	
	//Communication
	OSCRemoteControl::getInstance()->addRemoteControlListener(this);
	SerialManager::getInstance(); // init
	//BentoWebServer::getInstance(); //init

}

BentoEngine::~BentoEngine()
{
	PropManager::deleteInstance();
	PropClusterGroupManager::deleteInstance();

	LightBlockModelLibrary::deleteInstance();
	AudioManager::deleteInstance();
	SequenceLayerFactory::deleteInstance();

	SerialManager::deleteInstance();

	//BentoWebServer::deleteInstance();
}

void BentoEngine::clearInternal()
{
	PropManager::getInstance()->clear();
	PropClusterGroupManager::getInstance()->clear();
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
					Prop * p = PropManager::getInstance()->getPropWithId(id);
					if(p != nullptr) p->activeProvider->setValueFromTarget(providerToAssign);
				}


			}
		}

	} else if (aList[1] == "prop")
	{
		int id = OSCHelpers::getIntArg(m[0]);
		Prop * p = PropManager::getInstance()->getPropWithId(id);

		if (aList[2] == "enable")
		{
			bool active = OSCHelpers::getIntArg(m[1]) > 0;
			if(p != nullptr) p->enabled->setValue(active);
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
	
	var clusterData = PropClusterGroupManager::getInstance()->getJSONData();
	if (!clusterData.isVoid() && clusterData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("clusters", clusterData);

	return data;
}

void BentoEngine::loadJSONDataInternalEngine(var data, ProgressTask * loadingTask)
{
	//ProgressTask * projectTask = loadingTask->addTask("Project"
	ProgressTask * modelsTask = loadingTask->addTask("Models");
	ProgressTask * propTask = loadingTask->addTask("Props");
	ProgressTask * clusterTask = loadingTask->addTask("Clusters");

	
	clusterTask->start();
	PropClusterGroupManager::getInstance()->loadJSONData(data.getProperty("clusters", var()));
	clusterTask->setProgress(1);
	clusterTask->end(); 

	modelsTask->start();
	LightBlockModelLibrary::getInstance()->loadJSONData(data.getProperty("models", var()));
	modelsTask->setProgress(1);
	modelsTask->end();

	
	propTask->start();
	PropManager::getInstance()->loadJSONData(data.getProperty("props", var()));
	propTask->setProgress(1);
	propTask->end();	
}
