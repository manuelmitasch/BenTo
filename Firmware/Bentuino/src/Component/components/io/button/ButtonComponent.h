#pragma once

#define LONGPRESS_TIME 500      // more than 500ms is long press
#define VERYLONGPRESS_TIME 1500 // more than 1500ms is very long press
#define SHORTPRESS_TIME 500     // less than 500ms is short press
#define MULTIPRESS_TIME 300     // each new press shorter than 500ms after the previous one will increase the multiclick
#define BUTTONPRESS_DEBOUNCE 5  // denoising, needs five reads to validate a change

class ButtonComponent : public IOComponent
{
public:
    ButtonComponent(const String &name = "button", bool _enabled = false) : IOComponent(name, _enabled) {}

    int debounceCount;
    long timeAtPress;

    DeclareIntParam(multiPressCount, 0);
    DeclareBoolParam(longPress, false);
    DeclareBoolParam(veryLongPress, false);

    bool initInternal(JsonObject o) override;
    void updateInternal() override;
    // void onParameterEventInternal(const ParameterEvent &e) override;

    DeclareComponentEventTypes(ShortPress, LongPress, VeryLongPress, MultiPress);
    DeclareComponentEventNames("ShortPress", "LongPress", "VeryLongPress", "MultiPress");

    LinkScriptFunctionsStart
        LinkScriptFunction(ButtonComponent, getState, i, );
    LinkScriptFunction(ButtonComponent, getMultipress, i, );
    LinkScriptFunctionsEnd

    DeclareScriptFunctionReturn0(ButtonComponent, getState, uint32_t)
    {
        return veryLongPress ? 3 : longPress ? 2
                                             : value;
    }

    DeclareScriptFunctionReturn0(ButtonComponent, getMultipress, uint32_t) { return multiPressCount; }

    HandleSetParamInternalStart
        HandleSetParamMotherClass(IOComponent)
            CheckAndSetParam(multiPressCount);
    CheckAndSetParam(longPress);
    CheckAndSetParam(veryLongPress);
    HandleSetParamInternalEnd;

    FillSettingsInternalStart
        FillSettingsInternalMotherClass(IOComponent)
            FillSettingsParam(multiPressCount);
    FillSettingsParam(longPress);
    FillSettingsParam(veryLongPress);
    FillSettingsInternalEnd

        FillOSCQueryInternalStart
            FillOSCQueryInternalMotherClass(IOComponent)
                FillOSCQueryIntParam(multiPressCount);
    FillOSCQueryBoolParam(longPress);
    FillOSCQueryBoolParam(veryLongPress);
    FillOSCQueryInternalEnd
};

DeclareComponentManager(Button, BUTTON, buttons, button)
    EndDeclareComponent
