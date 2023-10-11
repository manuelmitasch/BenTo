#pragma once
DeclareComponentSingleton(Communication, "comm", )

#ifdef USE_SERIAL
    SerialComponent serial;
#endif

#ifdef USE_OSC
OSCComponent osc;
#endif

bool initInternal(JsonObject o) override;

void onChildComponentEvent(const ComponentEvent &e) override;

void sendParamFeedback(Component *c, void *param, const String &pName, Component::ParamType pType);
void sendMessage(Component *c, const String &mName, const String &val);
void sendEventFeedback(const ComponentEvent &e);

DeclareComponentEventTypes(MessageReceived);
DeclareComponentEventNames("MessageReceived");

EndDeclareComponent