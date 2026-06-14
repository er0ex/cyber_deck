#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* apiKey = "API";

// ── Глобальные переменные для async задачи ──
static String           _aiPrompt = "";
static String           _aiReply  = "";
static volatile bool    _aiDone   = false;

void _aiTask(void *param) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;

    if (https.begin(client, "https://openrouter.ai/api/v1/chat/completions")) {
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Authorization", String("Bearer ") + apiKey);
        https.addHeader("HTTP-Referer", "http://localhost");
        https.addHeader("X-Title", "ESP32-Jarvis");

        DynamicJsonDocument doc(2048);
        JsonArray messages = doc.createNestedArray("messages");

        JsonObject sys = messages.createNestedObject();
        sys["role"] = "system";
        sys["content"] =
            "Tu es un assistant IA nomme JARVIS. "
            "Reponds toujours en francais, mais sans aucun caractere special ou accent "
            "(pas de e accent, pas de c cedille, pas de a accent, etc). "
            "Ecris uniquement avec les 26 lettres de l alphabet latin standard (a-z, A-Z). "
            "N utilise jamais d emojis ni de symboles speciaux. "
            "Reponds de facon concise et directe. "
            "Si la question est simple une seule phrase suffit. "
            "Si necessaire deux ou trois phrases courtes maximum. "
            "Pas d explications inutiles, pas de remplissage.";

        JsonObject user = messages.createNestedObject();
        user["role"]    = "user";
        user["content"] = _aiPrompt;

        doc["model"]       = "deepseek/deepseek-chat";
        doc["temperature"] = 0.7;

        String body;
        serializeJson(doc, body);

        int httpCode = https.POST(body);

        if (httpCode == HTTP_CODE_OK) {
            String response = https.getString();
            https.end();

            DynamicJsonDocument result(4096);
            DeserializationError err = deserializeJson(result, response);

            if (!err)
                _aiReply = result["choices"][0]["message"]["content"].as<String>();
            else
                Serial.println("JSON parse error");
        } else {
            Serial.printf("HTTP Error: %d\n", httpCode);
            https.end();
        }
    }

    _aiDone = true;
    vTaskDelete(NULL);
}

String askDeepSeek(String prompt) {
    _aiPrompt = prompt;
    _aiReply  = "";
    _aiDone   = false;

    xTaskCreatePinnedToCore(
        _aiTask,
        "aiTask",
        16384,  // 16KB стек для TLS
        NULL,
        1,
        NULL,
        0       // core 0, loop на core 1 — не мешают друг другу
    );

    while (!_aiDone) delay(100);

    return _aiReply.length() ? _aiReply : "";
}