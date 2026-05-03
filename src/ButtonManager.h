#pragma once

#include <AceButton.h>
#include <map>
#include <functional>

using namespace ace_button;

typedef std::function<void()> ButtonCallback;

class ButtonManager {
public:
    ButtonManager();
    void setup();
    void loop();

    // Map Key: {PinID, EventType} -> Callback
    // EventTypes: AceButton::kEventPressed, kEventDoubleClicked, kEventLongPressed, etc.
    std::map<std::pair<uint8_t, uint8_t>, ButtonCallback> actions;

private:
    static void handleEvent(AceButton* button, uint8_t eventType, uint8_t eventState);
    static ButtonManager* s_instance; 
    
    AceButton** buttons = nullptr;
    uint8_t buttonCount = 0;
};