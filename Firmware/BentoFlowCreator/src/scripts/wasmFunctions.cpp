#include "wasmFunctions.h"
#include "../common/Common.h"
#include "../MainManager.h"

m3ApiRawFunction(m3_arduino_millis)
{
    m3ApiReturnType(uint32_t);

    m3ApiReturn(millis());
}

m3ApiRawFunction(m3_arduino_delay)
{
    m3ApiGetArg(uint32_t, ms);

    delay(ms);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_print)
{
    m3ApiGetArgMem(const uint8_t *, buf);
    m3ApiGetArg(uint32_t, len);

    // printf("api: print %p %d\n", buf, len);
    Serial.write(buf, len);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_clearLeds)
{
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.clear();

    m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLeds)
{
    m3ApiGetArg(uint32_t, color);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CRGB(color));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLedsRGB)
{
    m3ApiGetArg(uint32_t, r);
    m3ApiGetArg(uint32_t, g);
    m3ApiGetArg(uint32_t, b);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CRGB((uint8_t)r, (uint8_t)g, (uint8_t)b));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_fillLedsHSV)
{
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.fillAll(CHSV((uint8_t)h, (uint8_t)s, (uint8_t)v));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLed)
{
    m3ApiGetArg(uint32_t, index);
    m3ApiGetArg(uint32_t, color);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CRGB(color));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLedRGB)
{
    m3ApiGetArg(uint32_t, index);
    m3ApiGetArg(uint32_t, r);
    m3ApiGetArg(uint32_t, g);
    m3ApiGetArg(uint32_t, b);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CRGB((uint8_t)r, (uint8_t)g, (uint8_t)b));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_setLedHSV)
{
    m3ApiGetArg(uint32_t, index);
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);
    // MainManager::instance->leds.setMode(LedManager::Mode::Stream);
    MainManager::instance->leds.rgbManager.setLed(index, CHSV((uint8_t)h, (uint8_t)s, (uint8_t)v));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_pointRGB)
{
    m3ApiGetArg(float, pos);
    m3ApiGetArg(float, radius);
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);

    DBG("Call pointRGB " + String(pos));
    MainManager::instance->leds.rgbManager.point(CRGB((uint8_t)h, (uint8_t)s, (uint8_t)v), pos, radius, false);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_pointHSV)
{
    m3ApiGetArg(float, pos);
    m3ApiGetArg(float, radius);
    m3ApiGetArg(uint32_t, h);
    m3ApiGetArg(uint32_t, s);
    m3ApiGetArg(uint32_t, v);
    MainManager::instance->leds.rgbManager.point(CHSV((uint8_t)h, (uint8_t)s, (uint8_t)v), pos, radius, false);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_getOrientation)
{
    m3ApiGetArg(uint32_t, oi);
    m3ApiReturnType(uint32_t);
    DBG("Get orientation "+String(oi));
    float v = oi < 3 ? MainManager::instance->imu.orientation[oi] : -1;
    m3ApiReturn((uint32_t)v);
}

m3ApiRawFunction(m3_getYaw)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.orientation[0]);
}

m3ApiRawFunction(m3_getPitch)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.orientation[1]);
}
m3ApiRawFunction(m3_getRoll)
{
    m3ApiReturnType(float);
    m3ApiReturn(MainManager::instance->imu.orientation[2]);
}


m3ApiRawFunction(m3_getThrowState)
{
    m3ApiReturnType(uint32_t);
    m3ApiReturn((uint32_t)MainManager::instance->imu.throwState);
}

m3ApiRawFunction(m3_setIMUEnabled)
{
    m3ApiGetArg(uint32_t, en);
    DBG("Set IMU enabled from script : "+String((int)en));
    MainManager::instance->imu.setEnabled((bool)en);
    m3ApiSuccess();
}


m3ApiRawFunction(m3_updateLeds)
{
    MainManager::instance->leds.rgbManager.update();
    m3ApiSuccess();
}

m3ApiRawFunction(m3_getButtonState)
{
    m3ApiGetArg(uint32_t, oi);
    m3ApiReturnType(uint32_t);
    
    #ifdef BUTTON_COUNT
    int v = oi < BUTTON_COUNT ? MainManager::instance->buttons.isPressed[oi] : 0;
    DBG("Get button state "+String(oi)+" : "+String(v));
    #else
    int v = 0;
    #endif

    m3ApiReturn((uint32_t)v);
}
