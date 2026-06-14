#include "server.h"
#include "../includes/html.h"
#include "../../sd/includes/sd.h"
#include "../../input/includes/input.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

static WebServer controlServer(80);
static AsyncWebServer userServer(80);
static DNSServer dnsServer;

String getMime(const String& path) {
    if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
    if (path.endsWith(".png"))  return "image/png";
    if (path.endsWith(".bmp"))  return "image/bmp";
    if (path.endsWith(".mp4"))  return "video/mp4";
    if (path.endsWith(".mkv"))  return "video/x-matroska";
    if (path.endsWith(".avi"))  return "video/x-msvideo";
    if (path.endsWith(".mov"))  return "video/quicktime";
    if (path.endsWith(".webm")) return "video/webm";
    if (path.endsWith(".txt"))  return "text/plain";
    return "application/octet-stream";
}

// Проверка безопасности пути — только /video/имя без traversal
static bool isSafePath(const String& path) {
    if (!path.startsWith("/video/")) return false;
    if (path.indexOf("..") >= 0)     return false;
    if (path.indexOf("//") >= 0)     return false;
    String name = path.substring(7);
    if (name.indexOf('/') >= 0)      return false;  // нет вложенных папок
    if (name.length() == 0 || name.length() > 128) return false;
    return true;
}

// ===== AP =====
void initAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("CYBERDEVICE", "12345678");
    dnsServer.start(53, "*", WiFi.softAPIP());
}

// ===== ADMIN SERVER (port 80) =====
void handleFiles() {
    if (!stateSD) { controlServer.send(503, "text/plain", "SD NOT MOUNTED"); return; }
    String path = controlServer.hasArg("p") ? controlServer.arg("p") : "/";
    String html = "<html><head><meta charset='UTF-8'><style>:root{--bg:#000;--on:#00ff00;--dim:#00aa44;--line:#004466;--sel:#002200}";
    html += "*{box-sizing:border-box;margin:0;padding:0}body{background:var(--bg);color:var(--on);font-family:monospace;padding:16px;font-size:13px}";
    html += "header{border-bottom:1px solid var(--line);padding-bottom:8px;margin-bottom:10px;display:flex;justify-content:space-between}";
    html += ".path{color:var(--dim);font-size:11px;margin-bottom:10px}.row{display:flex;justify-content:space-between;align-items:center;padding:8px 4px;border-bottom:1px solid var(--line);color:var(--on);text-decoration:none}";
    html += ".row:hover{background:var(--sel)}.name{flex:1}.size{color:var(--dim);font-size:11px;margin:0 10px}.type{color:var(--line);font-size:10px;width:44px;text-align:right}a{text-decoration:none}</style></head><body>";
    html += "<header><div>// FILE BROWSER</div><a href='/' style='color:var(--dim)'>HOME</a></header>";
    html += "<div class='path'>PATH: " + path + "</div>";

    if (path != "/") {
        int last = path.lastIndexOf('/');
        String parent = (last == 0) ? "/" : path.substring(0, last);
        html += "<a class='row' href='/files?p=" + parent + "'><span class='name' style='color:var(--dim)'>.. BACK</span></a>";
    }

    // Директории
    File dir = SD.open(path);
    if (dir && dir.isDirectory()) {
        File f = dir.openNextFile();
        while (f) {
            String fullName = String(f.name());
            int slash = fullName.lastIndexOf('/');
            String name = (slash >= 0) ? fullName.substring(slash + 1) : fullName;
            if (name != "System Volume Information" && name != "LOST.DIR" && f.isDirectory()) {
                String itemPath = (path == "/") ? "/" + name : path + "/" + name;
                html += "<a class='row' href='/files?p=" + itemPath + "'><span class='name'>" + name + "</span><span class='type'>[DIR]</span></a>";
            }
            f.close();
            f = dir.openNextFile();
        }
        dir.close();
    }

    // Файлы
    dir = SD.open(path);
    if (dir && dir.isDirectory()) {
        File f = dir.openNextFile();
        while (f) {
            String fullName = String(f.name());
            int slash = fullName.lastIndexOf('/');
            String name = (slash >= 0) ? fullName.substring(slash + 1) : fullName;
            if (name != "System Volume Information" && name != "LOST.DIR" && !f.isDirectory()) {
                String itemPath = (path == "/") ? "/" + name : path + "/" + name;
                String sz = String(f.size() / 1024) + "kb";
                int dot = name.lastIndexOf('.');
                String ext = (dot >= 0) ? name.substring(dot + 1) : "???";
                ext.toUpperCase();
                html += "<a class='row' href='/download?f=" + itemPath + "'><span class='name'>" + name + "</span><span class='size'>" + sz + "</span><span class='type'>[" + ext + "]</span></a>";
            }
            f.close();
            f = dir.openNextFile();
        }
        dir.close();
    }

    html += "</body></html>";
    controlServer.send(200, "text/html", html);
}

void handleDownload() {
    if (!stateSD) { controlServer.send(503, "text/plain", "SD NOT MOUNTED"); return; }
    String path = controlServer.arg("f");
    if (!path.startsWith("/")) path = "/" + path;
    File f = SD.open(path);
    if (!f) { controlServer.send(404, "text/plain", "NOT FOUND"); return; }
    controlServer.sendHeader("Content-Length", String(f.size()));
    controlServer.streamFile(f, getMime(path));
    f.close();
}

void initServer() {
    controlServer.on("/", HTTP_GET, []() { controlServer.send_P(200, "text/html", INDEX_HTML); });
    controlServer.on("/files",    HTTP_GET, handleFiles);
    controlServer.on("/download", HTTP_GET, handleDownload);

    controlServer.on("/generate_204",        HTTP_GET, []() { controlServer.sendHeader("Location", "/"); controlServer.send(302); });
    controlServer.on("/hotspot-detect.html", HTTP_GET, []() { controlServer.sendHeader("Location", "/"); controlServer.send(302); });
    controlServer.on("/connecttest.txt",     HTTP_GET, []() { controlServer.sendHeader("Location", "/"); controlServer.send(302); });

    controlServer.on("/cmd/reboot", HTTP_GET, []() {
        controlServer.send(200, "text/plain", "REBOOTING...");
        delay(500); ESP.restart();
    });
    controlServer.on("/cmd/sd", HTTP_GET, []() {
        controlServer.send(200, "application/json", "{\"mounted\":" + String(stateSD ? "true" : "false") + "}");
    });
    controlServer.on("/cmd/status", HTTP_GET, []() {
        String j = "{\"heap\":" + String(ESP.getFreeHeap()) + ",\"wifi\":\"" + WiFi.softAPIP().toString() + "\",\"sd\":" + String(stateSD ? "true" : "false") + "}";
        controlServer.send(200, "application/json", j);
    });

    controlServer.begin();
}

void initUserServer() {
    static bool started = false;
    if (started) return;
    started = true;

    // Captive portal
    userServer.on("/generate_204",        HTTP_GET, [](AsyncWebServerRequest* req) { req->redirect("/"); });
    userServer.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest* req) { req->redirect("/"); });
    userServer.on("/connecttest.txt",     HTTP_GET, [](AsyncWebServerRequest* req) { req->redirect("/"); });
    userServer.on("/ncsi.txt",            HTTP_GET, [](AsyncWebServerRequest* req) { req->redirect("/"); });
    userServer.onNotFound(               [](AsyncWebServerRequest* req) { req->redirect("/"); });

    userServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/html", VIDEO_HTML);
    });

    userServer.on("/api/videos", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!stateSD) { req->send(503, "text/plain", "SD NOT MOUNTED"); return; }
        String json = "[";
        bool first = true;
        File dir = SD.open("/video");
        if (dir && dir.isDirectory()) {
            File f = dir.openNextFile();
            while (f) {
                String fullName = String(f.name());
                int slash = fullName.lastIndexOf('/');
                String name = (slash >= 0) ? fullName.substring(slash + 1) : fullName;
                if (!f.isDirectory() && !name.startsWith(".")) {
                    if (!first) json += ",";
                    float kb = f.size() / 1024.0f;
                    String size = kb > 1024 ? String(kb/1024.0f,1)+" MB" : String(kb,0)+" KB";
                    // экранируем кавычки в имени
                    String safeName = name; safeName.replace("\"", "\\\"");
                    json += "{\"name\":\""+safeName+"\",\"path\":\"/video/"+safeName+"\",\"size\":\""+size+"\"}";
                    first = false;
                }
                f.close();
                f = dir.openNextFile();
            }
            dir.close();
        }
        json += "]";
        req->send(200, "application/json", json);
    });

    // Стриминг с поддержкой Range (нужно для перемотки)
    userServer.on("/stream", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!stateSD) { req->send(503, "text/plain", "SD NOT MOUNTED"); return; }
        if (!req->hasParam("f")) { req->send(400, "text/plain", "NO FILE"); return; }

        String path = req->getParam("f")->value();

        // Защита от path traversal
        if (!isSafePath(path)) {
            req->send(403, "text/plain", "FORBIDDEN");
            return;
        }
        if (!SD.exists(path)) { req->send(404, "text/plain", "NOT FOUND"); return; }

        // Получаем размер файла
        File tmp = SD.open(path, FILE_READ);
        if (!tmp) { req->send(500, "text/plain", "OPEN FAILED"); return; }
        size_t fileSize = tmp.size();
        tmp.close();

        // Парсим Range: bytes=start-end
        size_t rangeStart = 0, rangeEnd = fileSize - 1;
        bool isRange = false;
        if (req->hasHeader("Range")) {
            String rng = req->getHeader("Range")->value();
            if (rng.startsWith("bytes=")) {
                rng = rng.substring(6);
                int dash = rng.indexOf('-');
                if (dash >= 0) {
                    rangeStart = (size_t)rng.substring(0, dash).toInt();
                    String endStr = rng.substring(dash + 1);
                    rangeEnd = endStr.length() ? (size_t)endStr.toInt() : fileSize - 1;
                    isRange = true;
                }
            }
        }
        if (rangeStart >= fileSize) rangeStart = 0;
        if (rangeEnd   >= fileSize) rangeEnd   = fileSize - 1;
        if (rangeStart > rangeEnd)  rangeStart = 0;
        size_t sendLen = rangeEnd - rangeStart + 1;

        File* sf = new File(SD.open(path, FILE_READ));
        if (!*sf) { delete sf; req->send(500, "text/plain", "OPEN FAILED"); return; }
        sf->seek(rangeStart);

        AsyncWebServerResponse* resp = req->beginResponse(
            getMime(path), sendLen,
            [sf, sendLen](uint8_t* buf, size_t maxLen, size_t index) mutable -> size_t {
                if (!*sf || index >= sendLen) { sf->close(); delete sf; return 0; }
                return sf->read(buf, min(maxLen, sendLen - index));
            }
        );
        resp->setCode(isRange ? 206 : 200);
        resp->addHeader("Accept-Ranges", "bytes");
        resp->addHeader("Cache-Control", "no-cache");
        if (isRange) {
            resp->addHeader("Content-Range",
                "bytes " + String(rangeStart) + "-" + String(rangeEnd) + "/" + String(fileSize));
        }
        req->send(resp);
    });

    userServer.on("/download", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!stateSD) { req->send(503, "text/plain", "SD NOT MOUNTED"); return; }
        if (!req->hasParam("f")) { req->send(400, "text/plain", "NO FILE"); return; }
        String path = req->getParam("f")->value();
        if (!isSafePath(path)) { req->send(403, "text/plain", "FORBIDDEN"); return; }
        if (!SD.exists(path)) { req->send(404, "text/plain", "NOT FOUND"); return; }
        req->send(SD, path, "application/octet-stream", true);
    });

    // Плеер — отдаём WATCH_HTML, путь берёт сам через JS
    userServer.on("/watch", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/html", WATCH_HTML);
    });

    userServer.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json",
            "{\"name\":\"CYBERMEDIA\",\"display\":\"fullscreen\","
            "\"background_color\":\"#000000\",\"theme_color\":\"#000000\"}");
    });

    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("VIDEO_HTML size: %d\n", strlen(VIDEO_HTML));
    userServer.begin();
    Serial.println("User server: http://" + WiFi.softAPIP().toString() + ":8080");
}

void serverAdmins() {
    dnsServer.processNextRequest();
    controlServer.handleClient();
}

void serverUsers() {}

void closeServer() {
    controlServer.stop();
    userServer.end();
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
}

void loopGetButtonServer() {
    while (true) {
        dnsServer.processNextRequest();
        if (getClickJoyButton()) break;
    }
}