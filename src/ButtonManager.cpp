#include "ButtonManager.h"

ButtonManager* ButtonManager::s_instance = nullptr;

ButtonManager::ButtonManager() {
    s_instance = this;
}

void ButtonManager::setup() {
    std::map<uint8_t, bool> uniquePins;
    for (auto const& [key, callback] : actions) {
        uniquePins[key.first] = true;
    }

    buttonCount = uniquePins.size();
    buttons = new AceButton*[buttonCount];
    uint64_t wakeup_bitmask = 0;

    ButtonConfig* config = ButtonConfig::getSystemButtonConfig();
    config->setEventHandler(handleEvent);
    
    config->setLongPressDelay(800);
    config->setFeature(ButtonConfig::kFeatureClick);
    config->setFeature(ButtonConfig::kFeatureLongPress);
    config->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
    config->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
    config->setFeature(ButtonConfig::kFeatureRepeatPress);
    config->setRepeatPressDelay(500);
    config->setRepeatPressInterval(100);

    int i = 0;
    for (auto const& [pin, unused] : uniquePins) {
        pinMode(pin, INPUT_PULLUP);
        buttons[i] = new AceButton(pin, 1, pin);
        wakeup_bitmask |= (1ULL << pin);
        i++;
    }

    // esp_sleep_enable_ext1_wakeup(wakeup_bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
}

void ButtonManager::handleEvent(AceButton* button, uint8_t eventType, uint8_t eventState) {
    uint8_t pin = button->getPin();
    unsigned long now = millis();
    
    Serial.printf("Time: %lu | Pin: %d | Event: %d | State: %d\n", now, pin, eventType, eventState);

    std::pair<uint8_t, uint8_t> key = {pin, eventType};
    if (s_instance && s_instance->actions.count(key)) {
        s_instance->actions[key]();
    }
}

void ButtonManager::loop() {
    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->check();
    }
}
