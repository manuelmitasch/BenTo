#include "LedManager.h"

LedManager::LedManager() : Component("leds"),
                           mode(Mode::Direct),
                           connectionIsAlive(false),
#ifdef LED_COUNT
                           currentMode(nullptr),
                           sysLedMode(rgbManager.leds, LED_COUNT),
                           streamMode(rgbManager.leds, LED_COUNT),
                           playerMode(rgbManager.leds, LED_COUNT),
#endif
                           connectedTimer(2000)
{

#ifdef LED_COUNT
    playerMode.addListener(std::bind(&LedManager::playerEvent, this, std::placeholders::_1));
    connectedTimer.addListener(std::bind(&LedManager::timerEvent, this, std::placeholders::_1));
#endif
}

void LedManager::init()
{
#ifdef LED_COUNT
    rgbManager.init();
    rgbManager.addListener(std::bind(&LedManager::rgbLedsEvent, this, std::placeholders::_1));
    sysLedMode.init();
    irManager.init();
#endif

    setMode(System);
}

void LedManager::update()
{
#ifdef LED_COUNT
    //if (mode == System) rgbManager.clear();

    bool shouldUpdateLeds = false;

    if (connectionIsAlive || playerMode.isPlaying)
    {
        if (currentMode != nullptr)
            shouldUpdateLeds = currentMode->update();
    }
    else
    {
        rgbManager.point(CHSV(0, 255, cos(millis() / 200.0f) * 127 + 127), .5f, .05f);
        shouldUpdateLeds = true;
    }

    if(shouldUpdateLeds) rgbManager.update();
    irManager.update();

    connectedTimer.update();
#endif
}

void LedManager::setMode(Mode m)
{
#ifdef LED_COUNT
    if (m == mode)
        return;

    if (currentMode != nullptr)
    {
        currentMode->stop();
    }

    mode = m;

    switch (mode)
    {
    case Direct:
        currentMode = nullptr;
        break;

    case System:
        currentMode = &sysLedMode;
        break;
    case Stream:
        currentMode = &streamMode;
        break;

    case Player:
        currentMode = &playerMode;
        break;

    default:
        break;
    }

    rgbManager.clear();

    if (currentMode != nullptr)
    {
        currentMode->start();
    }
#endif
}

void LedManager::shutdown(CRGB color)
{
#ifndef NO_ANIMATIONS

#ifdef LED_COUNT
    CRGB initLeds[LED_COUNT];
    memcpy(initLeds, rgbManager.leds, LED_COUNT * sizeof(CRGB));

    for (int i = 0; i < 255; i++)
    {
        for (int led = 0; led < LED_COUNT; led++)
            rgbManager.setLed(led, blend(initLeds[led], color, i));
        FastLED.delay(2);
    }

    FastLED.delay(50);

    for (float i = 1; i >= 0; i -= .01f)
    {
        rgbManager.fillRange(color, 0, i, true);
        rgbManager.update();
        FastLED.delay(3);
    }

    rgbManager.clear();
    rgbManager.update();
#endif //LED_COUNT

#endif

delay(100);
}

void LedManager::setConnectionState(ConnectionState state)
{
#ifdef LED_COUNT
    if (state == PingDead || state == PingAlive || state == Connected || state == Connecting)
    {
        connectionIsAlive = state == PingAlive || state == Connected || state == Connecting;
        rgbManager.clear();
        if (state == PingAlive || state == PingDead)
            return;
    }

    setMode(System);
    sysLedMode.setConnectionState(state);

    if (state == Connected || state == Hotspot)
        connectedTimer.start();
    else
        connectedTimer.stop();
#endif
}

#ifdef LED_COUNT
void LedManager::rgbLedsEvent(const RGBLedsEvent &e)
{
    if (e.type == RGBLedsEvent::ASK_FOCUS)
        setMode(Direct);
}

void LedManager::playerEvent(const PlayerEvent &e)
{
    if (e.type == PlayerEvent::Load || e.type == PlayerEvent::Play)
    {
        setMode(Player);
    }
}
#endif

void LedManager::timerEvent(const TimerEvent &e)
{
    if (e.timer == &connectedTimer)
        setMode(Stream);
}

bool LedManager::handleCommand(String command, var *data, int numData)
{
#ifdef LED_COUNT
    if (checkCommand(command, "mode", numData, 1))
    {
        if (data[0].type == 's')
        {
            if (data[0].stringValue() == "direct")
                setMode(Direct);
            else if (data[0].stringValue() == "system")
                setMode(System);
            else if (data[0].stringValue() == "stream")
                setMode(Stream);
            else if (data[0].stringValue() == "player")
                setMode(Player);
        }
        else
        {
            setMode((Mode)data[0].intValue());
        }
    }
#endif
}