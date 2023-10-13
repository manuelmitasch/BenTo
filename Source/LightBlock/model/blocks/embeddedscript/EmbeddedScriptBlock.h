/*
  ==============================================================================

    EmbeddedScriptBlock.h
    Created: 9 Nov 2021 6:06:13pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once




class EmbeddedScriptBlock :
	public LightBlockModel,
    public Thread
{
public:
	EmbeddedScriptBlock(var params = var());
	~EmbeddedScriptBlock();

    enum CompileType { COMPILE_DEBUG, COMPILE_OPTIMIZED, COMPILE_TINY };
    FileParameter* scriptFile;
    EnumParameter* compileType;
    BoolParameter* lowMemory;
    Trigger* compileTrigger;
    Trigger* uploadToPropsTrigger;
    Trigger* loadOnPropsTrigger;

    BoolParameter* autoCompile;
    BoolParameter* autoUpload;
    BoolParameter* autoLaunch;
    Trigger* stopOnPropsTrigger;
    Time lastModTime;

    void checkAutoCompile();
    void compile();

    void loadScriptOnProp(Prop* p);
    void stopScriptOnProp(Prop* p);

    File getWasmFile();

    void run() override;

    void onContainerParameterChangedInternal(Parameter* p) override;
    void onContainerTriggerTriggered(Trigger* t) override;

    void getColorsInternal(Array<Colour>* result, Prop* p, double time, int id, int resolution, var params) override;
    void handleEnterExit(bool enter, Array<Prop*> props) override;

    String getTypeString() const override { return "Embedded"; }
};


class EmbeddedScriptBlockManager :
    public UserLightBlockModelManager,
    public Timer
{
public:
    EmbeddedScriptBlockManager() : UserLightBlockModelManager("Embedded", EMBEDDED_SCRIPT) { startTimerHz(1); }
    ~EmbeddedScriptBlockManager() {}

    void timerCallback() override { for (auto& i : items) ((EmbeddedScriptBlock*)i)->checkAutoCompile(); }
};