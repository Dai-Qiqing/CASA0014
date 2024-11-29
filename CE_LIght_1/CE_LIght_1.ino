#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>

// WiFi 和 MQTT 配置
const char* ssid          = "CE-Hub-Student";
const char* password      = "casa-ce-gagarin-public-service";
const char* mqtt_username = "student";
const char* mqtt_password = "ce2021-mqtt-forget-whale";
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);
char mqtt_topic_demo[] = "student/CASA0014/light/2/pixel/";

// DHT11 传感器
#define DHTPIN D4      // DHT11 数据引脚连接到 D4（GPIO2）
#define DHTTYPE DHT11  // 使用 DHT11 传感器
DHT dht(DHTPIN, DHTTYPE);

// 传感器数据
float temperature = 0.0;
float humidity = 0.0;
float lightLevel = 0.0;  // 光照强度

// LED 颜色
int R = 0, G = 0, B = 0;

// 控制参数，默认为温度
String controlParameter = "temperature";

// 舒适范围和警报线定义，可由用户设置
struct ComfortRange {
  float minComfort;
  float maxComfort;
  float alarmThreshold;
};

ComfortRange tempRange = {20.0, 25.0, 30.0};      // 温度
ComfortRange humRange = {40.0, 60.0, 70.0};       // 湿度
ComfortRange lightRange = {200.0, 800.0, 900.0};  // 光照强度

// 红灯闪烁控制
unsigned long alarmStartTime = 0;
bool isAlarm = false;

// 自动/手动模式
bool isAutoMode = true;

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.hostname("ESP8266_Light_25");
  startWifi();
  client.setServer(mqtt_server, mqtt_port);

  // 初始化 DHT11
  dht.begin();

  // 设置服务器路由
  server.on("/", handleRoot);
  server.on("/getSensorData", handleGetSensorData);
  server.on("/sendmqtt", handleSendMQTT);
  server.on("/setControlParameter", handleSetControlParameter);
  server.on("/setMode", handleSetMode);
  server.on("/setComfortRanges", handleSetComfortRanges);
  server.on("/setCustomRGB", handleSetCustomRGB);

  server.begin();
  Serial.println("5 秒后开始输出...");
  delay(5000);
  Serial.println("系统初始化完成");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();
  server.handleClient();

  // 读取 DHT11 传感器数据
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // 读取 TEMT6000 光照传感器数据
  lightLevel = analogRead(A0);  // 假设 TEMT6000 连接到 A0 引脚

  // 调整 LED 颜色
  if (isAutoMode) {
    adjustLED();
  }
}

void startWifi() {
  Serial.print("正在连接到 WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 连接成功");
  Serial.print("IP 地址: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("尝试连接 MQTT 服务器...");
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("MQTT 连接成功");
    } else {
      Serial.print("连接失败, 错误代码: ");
      Serial.print(client.state());
      Serial.println(" 5 秒后重试...");
      delay(5000);
    }
  }
}

void adjustLED() {
  float value = 0.0;
  ComfortRange range;

  if (controlParameter == "temperature") {
    value = temperature;
    range = tempRange;
  } else if (controlParameter == "humidity") {
    value = humidity;
    range = humRange;
  } else if (controlParameter == "lightLevel") {
    value = lightLevel;
    range = lightRange;
  }

  if (value >= range.minComfort && value <= range.maxComfort) {
    // 舒适范围，绿灯
    R = 0;
    G = 255;
    B = 0;
    isAlarm = false;
    alarmStartTime = 0;
  } else if (value > range.alarmThreshold) {
    // 超过警报值，红灯
    R = 255;
    G = 0;
    B = 0;
    isAlarm = false;
    alarmStartTime = 0;
  } else {
    // 超出舒适范围但未达警报线，根据数值映射到RGB
    mapValueToRGB(value);
  }

  // 发送 MQTT 消息
  for (int k = 0; k <= 11; k++) {
    sendmqtt(k, R, G, B, 0);
  }
}

void mapValueToRGB(float value) {
  // 自定义数值映射到 RGB 值的算法
  // 这里提供一个简单的线性映射示例，您可以根据需要修改

  float minVal = 0;
  float maxVal = 100; // 根据您的传感器范围调整

  int red = map(value, minVal, maxVal, 0, 255);
  int green = 255 - red;
  int blue = 0;

  // 确保 RGB 值在 0-255 之间
  R = constrain(red, 0, 255);
  G = constrain(green, 0, 255);
  B = constrain(blue, 0, 255);
}

void sendmqtt(int pixelid, int R, int G, int B, int W) {
  char mqtt_message[100];
  sprintf(mqtt_message, "{\"pixelid\":%d,\"R\":%d,\"G\":%d,\"B\":%d,\"W\":%d}", pixelid, R, G, B, W);
  Serial.println(mqtt_message);

  if (client.publish(mqtt_topic_demo, mqtt_message)) {
    Serial.println("MQTT 消息已发布");
  } else {
    Serial.println("MQTT 消息发布失败");
  }
}

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <title>Environment Monitoring and Light Control</title>
      <style>
        body {
          margin: 0;
          padding: 0;
          background: linear-gradient(120deg, #f6d365, #fda085);
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
          color: #333;
          overflow-x: hidden;
        }
        h1 {
          text-align: center;
          padding: 20px 0;
          margin: 0;
          font-size: 2.5em;
          color: #fff;
          text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .container {
          width: 90%;
          margin: 0 auto;
        }
        .sensor-data {
          display: flex;
          flex-wrap: wrap;
          justify-content: center;
          margin-bottom: 20px;
        }
        .sensor-box {
          background: rgba(255, 255, 255, 0.8);
          border-radius: 15px;
          padding: 20px;
          margin: 10px;
          width: 150px;
          text-align: center;
          box-shadow: 0 4px 6px rgba(0,0,0,0.1);
          cursor: pointer;
          transition: transform 0.2s;
        }
        .sensor-box:hover {
          transform: translateY(-5px);
        }
        .sensor-box h2 {
          margin: 0;
          font-size: 1.2em;
          color: #555;
        }
        .sensor-box p {
          margin: 10px 0 0;
          font-size: 1.5em;
          color: #333;
        }
        .main-content {
          display: flex;
          flex-wrap: wrap;
          justify-content: center;
          align-items: flex-start;
        }
        .lamp {
          width: 120px;
          height: 220px;
          margin: 20px;
          background-color: #FFFFFF;
          border-radius: 20px;
          box-shadow: 0 0 20px #FFFFFF;
          transition: background-color 0.5s, box-shadow 0.5s;
        }
        .control-panel, .environment-description {
          background: rgba(255, 255, 255, 0.9);
          border-radius: 15px;
          padding: 20px;
          margin: 20px;
          width: 350px;
          box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .control-panel h2, .environment-description h2 {
          margin-top: 0;
          color: #333;
        }
        .color-controls {
          display: flex;
          flex-direction: column;
          align-items: center;
        }
        .color-controls input[type="color"] {
          margin: 10px 0;
          width: 80px;
          height: 80px;
          border: none;
          border-radius: 50%;
          overflow: hidden;
          padding: 0;
          -webkit-appearance: none;
        }
        .color-controls label {
          font-size: 1.1em;
          color: #555;
        }
        .color-controls input[type="range"] {
          margin: 10px 0;
          width: 100%;
        }
        .color-controls button {
          margin-top: 15px;
          width: 140px;
          height: 45px;
          font-size: 1em;
          border: none;
          border-radius: 5px;
          cursor: pointer;
          color: #fff;
          background-color: #28a745;
          transition: background-color 0.3s;
        }
        .color-controls button:hover {
          background-color: #218838;
        }
        .environment-description p {
          font-size: 1.1em;
          color: #555;
        }
        .mode-toggle {
          text-align: center;
          margin: 30px 0;
        }
        .mode-toggle button {
          width: 140px;
          height: 45px;
          font-size: 1em;
          margin: 10px;
          border: none;
          border-radius: 5px;
          cursor: pointer;
          color: #fff;
          transition: background-color 0.3s;
        }
        #autoModeBtn.active, #manualModeBtn.active {
          background-color: #007bff;
        }
        #autoModeBtn.inactive, #manualModeBtn.inactive {
          background-color: #6c757d;
        }
        #autoModeBtn.active:hover, #manualModeBtn.active:hover {
          background-color: #0056b3;
        }
        #autoModeBtn.inactive:hover, #manualModeBtn.inactive:hover {
          background-color: #5a6268;
        }
        @media (max-width: 768px) {
          .main-content {
            flex-direction: column;
            align-items: center;
          }
          .control-panel, .environment-description {
            width: 90%;
          }
        }
      </style>
    </head>
    <body>
      <h1>Environment Monitoring and Light Control</h1>
      <div class="container">
        <div class="sensor-data">
          <div class="sensor-box" onclick="setControlParameter('temperature')">
            <h2>Temperature</h2>
            <p><span id="temp">--</span> °C</p>
          </div>
          <div class="sensor-box" onclick="setControlParameter('humidity')">
            <h2>Humidity</h2>
            <p><span id="hum">--</span> %</p>
          </div>
          <div class="sensor-box" onclick="setControlParameter('lightLevel')">
            <h2>Light Level</h2>
            <p><span id="lightLevel">--</span></p>
          </div>
        </div>

        <div class="main-content">
          <div class="lamp" id="lamp"></div>

          <div class="control-panel">
            <h2>Set Comfort and Alarm Thresholds</h2>
            <div>
              <label for="minComfort">Min Comfort:</label>
              <input type="number" id="minComfort" value="20">
            </div>
            <div>
              <label for="maxComfort">Max Comfort:</label>
              <input type="number" id="maxComfort" value="25">
            </div>
            <div>
              <label for="alarmThreshold">Alarm Threshold:</label>
              <input type="number" id="alarmThreshold" value="30">
            </div>
            <button onclick="setComfortRanges()">Set Thresholds</button>
          </div>

          <div class="control-panel">
            <div>
              <label for="customValue">Value:</label>
              <input type="number" id="customValue" value="0">
            </div>
            <button onclick="setCustomRGB()">Set Custom RGB</button>
          </div>


        </div>

        <div class="mode-toggle">
          <button id="autoModeBtn" class="active" onclick="setMode('auto')">Auto Mode</button>
          <button id="manualModeBtn" class="inactive" onclick="setMode('manual')">Manual Mode</button>
        </div>
      </div>

      <script>
        let isAutoMode = true;
        let controlParameter = 'temperature';

        function updateSensorData() {
          fetch('/getSensorData')
            .then(response => response.json())
            .then(data => {
              document.getElementById('temp').textContent = data.temperature;
              document.getElementById('hum').textContent = data.humidity;
              document.getElementById('lightLevel').textContent = data.lightLevel;

              document.getElementById('lamp').style.backgroundColor = data.currentColor;
              document.getElementById('lamp').style.boxShadow = '0 0 20px ' + data.currentColor;

              
              let description = '';

          
              if (data.temperature < 18) {
                description += 'Temperature is low, please keep warm.';
              } else if (data.temperature > 30) {
                description += 'Temperature is high, stay cool.';
              } else {
                description += 'Temperature is comfortable.';
              }

            
              if (data.humidity > 70) {
                description += ' Humidity is high, might feel stuffy.';
              } else if (data.humidity < 30) {
                description += ' Humidity is low, air is dry.';
              } else {
                description += ' Humidity is comfortable.';
              }

        
              if (data.lightLevel > 900) {
                description += ' Light level is high.';
              } else if (data.lightLevel < 200) {
                description += ' Light level is low.';
              } else {
                description += ' Light level is moderate.';
              }

              document.getElementById('envDescription').textContent = description;

              if (isAutoMode) {
            
              }
            });
        }

        function setControlParameter(parameter) {
          controlParameter = parameter;
          fetch('/setControlParameter?parameter=' + parameter);
        }

        function setComfortRanges() {
          const minComfort = document.getElementById('minComfort').value;
          const maxComfort = document.getElementById('maxComfort').value;
          const alarmThreshold = document.getElementById('alarmThreshold').value;

          const payload = {
            minComfort: parseFloat(minComfort),
            maxComfort: parseFloat(maxComfort),
            alarmThreshold: parseFloat(alarmThreshold),
            parameter: controlParameter
          };

          fetch("/setComfortRanges", {
            method: "POST",
            headers: {
              "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
          }).then(response => {
            if (response.ok) {
              alert("Thresholds updated!");
            } else {
              alert("Failed to update thresholds, please try again!");
            }
          });
        }

        function setCustomRGB() {
          const value = document.getElementById('customValue').value;

          const payload = {
            value: parseFloat(value),
            parameter: controlParameter
          };

          fetch("/setCustomRGB", {
            method: "POST",
            headers: {
              "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
          }).then(response => {
            if (response.ok) {
              alert("Custom RGB mapping set!");
            } else {
              alert("Failed to set custom RGB mapping, please try again!");
            }
          });
        }

        function setMode(mode) {
          fetch('/setMode?mode=' + mode)
            .then(() => {
              if (mode === 'auto') {
                isAutoMode = true;
                document.getElementById('autoModeBtn').classList.add('active');
                document.getElementById('autoModeBtn').classList.remove('inactive');
                document.getElementById('manualModeBtn').classList.add('inactive');
                document.getElementById('manualModeBtn').classList.remove('active');
              } else {
                isAutoMode = false;
                document.getElementById('autoModeBtn').classList.add('inactive');
                document.getElementById('autoModeBtn').classList.remove('active');
                document.getElementById('manualModeBtn').classList.add('active');
                document.getElementById('manualModeBtn').classList.remove('inactive');
              }
            });
        }

        setInterval(updateSensorData, 2000); 
      </script>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleGetSensorData() {
  DynamicJsonDocument doc(512);
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["lightLevel"] = lightLevel;
  doc["currentColor"] = String("rgb(") + String(R) + "," + String(G) + "," + String(B) + ")";
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSendMQTT() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(200);
    deserializeJson(doc, body);
    int pixelid = doc["pixelid"];
    int R = doc["R"];
    int G = doc["G"];
    int B = doc["B"];
    int W = doc["W"];
    for (int k = 0; k <= 11; k++) {
      sendmqtt(k, R, G, B, W);
    }
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"failure\"}");
  }
}

void handleSetControlParameter() {
  if (server.hasArg("parameter")) {
    controlParameter = server.arg("parameter");
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"failure\"}");
  }
}

void handleSetMode() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    isAutoMode = (mode == "auto");
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"failure\"}");
  }
}

void handleSetComfortRanges() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(200);
    deserializeJson(doc, body);
    float minComfort = doc["minComfort"];
    float maxComfort = doc["maxComfort"];
    float alarmThreshold = doc["alarmThreshold"];
    String parameter = doc["parameter"];

    if (parameter == "temperature") {
      tempRange.minComfort = minComfort;
      tempRange.maxComfort = maxComfort;
      tempRange.alarmThreshold = alarmThreshold;
    } else if (parameter == "humidity") {
      humRange.minComfort = minComfort;
      humRange.maxComfort = maxComfort;
      humRange.alarmThreshold = alarmThreshold;
    } else if (parameter == "lightLevel") {
      lightRange.minComfort = minComfort;
      lightRange.maxComfort = maxComfort;
      lightRange.alarmThreshold = alarmThreshold;
    }

    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"failure\"}");
  }
}

void handleSetCustomRGB() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(200);
    deserializeJson(doc, body);
    float value = doc["value"];
    String parameter = doc["parameter"];

    mapValueToRGB(value);


    for (int k = 0; k <= 11; k++) {
      sendmqtt(k, R, G, B, 0);
    }

    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"failure\"}");
  }
}