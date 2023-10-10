bool IOComponent::availablePWMChannels[16] = {true};

bool IOComponent::initInternal(JsonObject o)
{
    pwmChannel = -1;

    AddAndSetParameter(pin);
    AddAndSetParameter(mode);

    mode.options = modeOptions;
    mode.numOptions = PINMODE_MAX;

    AddAndSetParameter(inverted);

    AddParameter(value);
    int m = mode.intValue();
    value.readOnly = m == D_INPUT || m == D_INPUT_PULLUP || m == A_INPUT;

    prevValue = value.floatValue();

    setupPin();
    updatePin();

    return true;
}

void IOComponent::updateInternal()
{
    updatePin();
}

void IOComponent::clearInternal()
{
}

void IOComponent::setupPin()
{
    if (curPin != -1 && pwmChannel != -1) // prevPin was a PWM pin
    {
        ledcDetachPin(curPin);
        availablePWMChannels[pwmChannel] = true;
        pwmChannel = -1;
    }

    curPin = pin.intValue();

    if (curPin > 0)
    {
        int m = mode.intValue();

        int pinm = -1;
        switch (m)
        {
        case D_INPUT:
        case A_INPUT:

            pinm = INPUT;
            break;

        case D_INPUT_PULLUP:
            pinm = INPUT_PULLUP;
            break;

        case D_OUTPUT:
            pinm = OUTPUT;
            break;

        default:
            break;
        }

        if (pinm != -1)
        {

            pinMode(curPin, pinm);
        }
        else if (m == A_OUTPUT)
        {
            if (m == A_OUTPUT)
            {
                int channel = getFirstAvailablePWMChannel();
                if (channel >= 0)
                {
                    pwmChannel = channel;
                    ledcSetup(pwmChannel, 5000, 10); // 0-1024 at a 5khz resolution
                    ledcAttachPin(curPin, pwmChannel);
                    availablePWMChannels[pwmChannel] = false;
                    // NDBG("Attach pin " + pin.stringValue() + " to " + String(pwmChannel));
                }
                else
                {
                    NDBG("Max channels reached, not able to create another A_OUTPUT");
                }
            }
        }
    }
}

void IOComponent::updatePin()
{
    if (pin.intValue() == -1)
        return;

    int m = mode.intValue();
    switch (m)
    {
    case D_INPUT:
    case D_INPUT_PULLUP:
    {
        bool val = digitalRead(pin.intValue());
        if (inverted.boolValue())
            val = !val;

        value.set(val);
    }
    break;

    case D_OUTPUT:
    case A_OUTPUT:
    {
        if (prevValue != value.floatValue())
        {
            if (m == D_OUTPUT)
            {
                digitalWrite(pin.intValue(), inverted.boolValue() ? !value.boolValue() : value.boolValue());
            }
            else
            {
                if (pwmChannel != -1)
                {
                    uint32_t v = value.floatValue() * 1024;
                    // NDBG("Set PWM with value " + String(v));
                    ledcWrite(pwmChannel, v);
                }
            }

            prevValue = value.floatValue();
        }
    }
    break;

    case A_INPUT:
    {
        float v = analogRead(pin.intValue()) / 4095.0f;
        if (inverted.boolValue())
            v = 1 - v;
        value.set(v);
    }
    break;
    }
}

void IOComponent::onParameterEventInternal(const ParameterEvent &e)
{
    if (e.parameter == &mode)
    {
        int m = mode.intValue();
        value.readOnly = m == D_INPUT || m == D_INPUT_PULLUP || m == A_INPUT;
        setupPin();
    }
    else if (e.parameter == &pin)
    {
        setupPin();
    }
}

int IOComponent::getFirstAvailablePWMChannel() const
{
    for (int i = 0; i < 16; i++)
    {
        if (availablePWMChannels[i])
            return i;
    }
    return -1;
}


//Manager

ImplementSingleton(IOManagerComponent);

bool IOManagerComponent::initInternal(JsonObject o)
{
    AddAndSetParameter(numIOs);

    for (int i = 0; i < numIOs.intValue(); i++)
    {
        DBG("Add io " + String(i + 1) + " of " + String(numIOs.intValue()));
        ios[i].name = "io" + String(i + 1);
        AddOwnedComponent(&ios[i]);
    }

     // initialize static io here
    memset(IOComponent::availablePWMChannels, true, sizeof(IOComponent::availablePWMChannels));

    return true;
}