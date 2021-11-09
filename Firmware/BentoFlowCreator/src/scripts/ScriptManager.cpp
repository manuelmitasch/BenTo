#include "ScriptManager.h"
#include "../files/FileManager.h"

ScriptManager *ScriptManager::instance = nullptr;

ScriptManager::ScriptManager() : Component("scripts"),
                                 isRunning(false),
                                 runtime(NULL)
{
    instance = this;
}

void ScriptManager::init()
{
    NDBG("Script Manager init.");
    env = m3_NewEnvironment();
    if (!env)
        NDBG("Script environment error");
}

void ScriptManager::update()
{
    if (isRunning)
    {
        //TSTART()
        m3_CallV(updateFunc);
         //TFINISH("Script ")
    }
}

void ScriptManager::launchScript(String path)
{
    if (isRunning)
    {
        NDBG("Script is running, stop before load");
        stop();
    }

    NDBG("Load script " + path + "...");

    File f = FileManager::openFile("/" + path + ".wasm", false); // false is for reading
    if (!f)
    {
        NDBG("Error reading file " + path);
        return;
    }

    long totalBytes = f.size();
    if (totalBytes > SCRIPT_MAX_SIZE)
    {
        NDBG("Script size is more than max size");
        return;
    }

    scriptSize = totalBytes;

    f.read(scriptData, scriptSize);

    NDBG("Read " + String(scriptSize) + " bytes");

    launchWasm();
}

void ScriptManager::launchWasm()
{
    NDBG("Launching wasm...");
    if (isRunning)
        stop();

#if WASM_ASYNC
    xTaskCreate(&ScriptManager::launchWasmTaskStatic, "wasm3", NATIVE_STACK_SIZE, NULL, 5, NULL);
#else
    launchWasmTask();
#endif

    NDBG("Wasm task launched...");
}

#if WASM_ASYNC
void ScriptManager::launchWasmTaskStatic(void *v)
{
    instance->launchWasmTask();
}
#endif

void ScriptManager::launchWasmTask()
{
    M3Result result = m3Err_none;
    IM3Module module;

    if (runtime != NULL)
    {
        NDBG("New run free Runtime");
        m3_FreeRuntime(runtime);
    }

    runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
    if (!runtime)
    {
        NDBG("Script runtime setup error");
        return;
    }

#ifdef WASM_MEMORY_LIMIT
    runtime->memoryLimit = WASM_MEMORY_LIMIT;
#endif

    result = m3_ParseModule(env, &module, scriptData, scriptSize);
    if (result)
    {
        DBG("ParseModule error " + String(result));
        return;
    }

    result = m3_LoadModule(runtime, module);
    if (result)
    {
        DBG("LoadModule error " + String(result));
        return;
    }

    result = LinkArduino(runtime);
    if (result)
    {
        DBG("LinkArduino error " + String(result));
        return;
    }

    result = m3_FindFunction(&initFunc, runtime, "init");
    if (result)
    {
        DBG("FindFunction init error " + String(result));
        return;
    }

    result = m3_FindFunction(&updateFunc, runtime, "update");
    if (result)
    {
        DBG("FindFunction update error " + String(result));
        return;
    }

    result = m3_FindFunction(&stopFunc, runtime, "_stop");
    if (result)
    {
        DBG("FindFunction _stop error " + String(result));
        return;
    }

    isRunning = true;
    result = m3_CallV(initFunc);

#if WASM_ASYNC
    vTaskDelete(NULL);
#endif
}

M3Result ScriptManager::LinkArduino(IM3Runtime runtime)
{
    IM3Module module = runtime->modules;
    const char *arduino = "arduino";

    m3_LinkRawFunction(module, arduino, "millis", "i()", &m3_arduino_millis);
    m3_LinkRawFunction(module, arduino, "delay", "v(i)", &m3_arduino_delay);
    m3_LinkRawFunction(module, arduino, "print", "v(*i)", &m3_arduino_print);

    m3_LinkRawFunction(module, arduino, "clearLeds", "v()", &m3_clearLeds);
    m3_LinkRawFunction(module, arduino, "fillLeds", "v(i)", &m3_fillLeds);
    m3_LinkRawFunction(module, arduino, "fillLedsRGB", "v(iii)", &m3_fillLedsRGB);
    m3_LinkRawFunction(module, arduino, "fillLedsHSV", "v(iii)", &m3_fillLedsHSV);
    m3_LinkRawFunction(module, arduino, "setLed", "v(ii)", &m3_setLed);
    m3_LinkRawFunction(module, arduino, "setLedRGB", "v(iiii)", &m3_setLedRGB);
    m3_LinkRawFunction(module, arduino, "setLedHSV", "v(iiii)", &m3_setLedHSV);
    m3_LinkRawFunction(module, arduino, "pointRGB", "v(ffiii)", &m3_pointRGB);
    m3_LinkRawFunction(module, arduino, "pointHSV", "v(ffiii)", &m3_pointHSV);
    m3_LinkRawFunction(module, arduino, "getOrientation", "i(i)", &m3_getOrientation);
    m3_LinkRawFunction(module, arduino, "getYaw", "f()", &m3_getYaw);
    m3_LinkRawFunction(module, arduino, "getPitch", "f()", &m3_getPitch);
    m3_LinkRawFunction(module, arduino, "getRoll", "f()", &m3_getRoll);
    m3_LinkRawFunction(module, arduino, "getThrowState", "i()", &m3_getThrowState);
    m3_LinkRawFunction(module, arduino, "updateLeds", "v()", &m3_updateLeds);
    m3_LinkRawFunction(module, arduino, "getNumber", "f()", &m3_getNumber);

    return m3Err_none;
}

void ScriptManager::stop()
{
    NDBG("Stopping script");
    m3_CallV(stopFunc);

    isRunning = false;
    m3_FreeRuntime(runtime);
    runtime = NULL;
    m3_FreeEnvironment(env);
    env = NULL;
}

void ScriptManager::shutdown()
{
    stop();
 
}

bool ScriptManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "load", numData, 1))
    {
        launchScript(data[0].stringValue());
        return true;
    }
    if (checkCommand(command, "stop", numData, 0))
    {
        stop();
        return true;
    }
    
    return false;
}