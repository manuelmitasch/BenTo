/*
  ==============================================================================

    ModelListener.h
    Created: 12 Apr 2018 1:47:03pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class LightBlock;
class LightBlockColorProvider : 
	public BaseItem
{
public:
	LightBlockColorProvider(const String &name = "Provider", bool canBeDisabled = true, bool canHaveScripts = false);
	virtual ~LightBlockColorProvider();
	
	Trigger * assignToAll;

	virtual Array<WeakReference<Parameter>> getModelParameters() = 0;
	virtual void updateColorsForBlock(LightBlock * block, var params = var()) = 0;

	void onContainerTriggerTriggered(Trigger *) override;

	class ProviderListener
	{
	public:
		virtual ~ProviderListener() {}
		virtual void providerParametersChanged(LightBlockColorProvider *) {}
	};

	ListenerList<ProviderListener> providerListeners;
	void addColorProviderListener(ProviderListener* newListener) { providerListeners.add(newListener); }
	void removeColorProviderListener(ProviderListener* listener) { providerListeners.remove(listener); }

private:
	WeakReference<LightBlockColorProvider>::Master masterReference;
	friend class WeakReference<LightBlockColorProvider>;
};