/*
  ==============================================================================

    ModelListener.h
    Created: 12 Apr 2018 1:47:03pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class LightBlock;
class Prop;

//Baking
struct PlaybackData
{
	PlaybackData(StringRef name = "default", float startTime = 0, float endTime = 10, int fps = 100, var metaData = new DynamicObject()) :
		name(String(name)), startTime(startTime), endTime(endTime), numFrames(0), fps(fps), metaData(metaData)
	{

	}
	String name;
	float startTime;
	float endTime;
	int numFrames;
	int fps;
	MemoryBlock data;
	var metaData;
};


class BaseColorProvider
{
public:
	virtual ~BaseColorProvider() {}
	virtual Array<Colour> getColors(Prop* p, double time, var params) = 0;
	virtual PlaybackData getPlaybackDataForProp(Prop*) = 0;
};

class LightBlockColorProvider : 
	public BaseItem,
	public BaseColorProvider
{
public:
	enum ProviderType { SOURCE, FILTER };
	
	LightBlockColorProvider(const String &name = "Provider", bool canBeDisabled = true, bool canHaveScripts = false, ProviderType type = SOURCE);
	virtual ~LightBlockColorProvider();
	
	ProviderType providerType;

	Trigger * assignToAll;

	//Bake info
	enum PlaybackGenControl { PLAY, PAUSE, STOP, SEEK, SHOW_ID };
	
	virtual Array<WeakReference<Controllable>> getModelParameters() = 0;

	virtual Array<Colour> getColors(Prop* p, double time, var params) override;
	virtual PlaybackData getPlaybackDataForProp(Prop*) override;

	void onContainerTriggerTriggered(Trigger *) override;

	virtual void handleEnterExit(bool enter, Array<Prop *> props) {}

	class ProviderListener
	{
	public:
		virtual ~ProviderListener() {}
		virtual void providerParametersChanged(LightBlockColorProvider *) {}
		virtual void providerParameterValueUpdated(LightBlockColorProvider *, Parameter *) {}
		virtual void providerPlaybackGenControlUpdate(PlaybackGenControl control, var data = var()) {}
	};

	ListenerList<ProviderListener> providerListeners;
	void addColorProviderListener(ProviderListener* newListener) { providerListeners.add(newListener); }
	void removeColorProviderListener(ProviderListener* listener) { providerListeners.remove(listener); }


	InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables) override;

private:
	WeakReference<LightBlockColorProvider>::Master masterReference;
	friend class WeakReference<LightBlockColorProvider>;

	
};
