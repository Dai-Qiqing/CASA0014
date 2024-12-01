
Introduction
Studies have shown that environmental factors significantly impact learning efficiency and concentration. Variables such as temperature, humidity, and lighting not only influence human comfort but are also directly linked to cognitive performance and learning outcomes. Research indicates that maintaining optimal indoor temperature and humidity can effectively enhance students' learning efficiency, while dynamically adjusting light intensity helps improve concentration in learning environments. This is especially critical during extended study sessions, where a comfortable environment supports sustained attention.

Based on this background, I designed an environmental monitoring and lighting control system. The system uses an ESP8266 WiFi module integrated with temperature and humidity sensors, as well as a light sensor, to monitor real-time data on temperature, humidity, and light intensity in the learning environment.

Current Features
Environmental Monitoring and Feedback:
The system employs a DHT11 sensor to monitor temperature and humidity, and a TEMT6000 light sensor to measure light intensity. Based on the real-time data, the system adjusts the color and brightness of an LED light. When the environment is optimal, the light displays green; when temperature, humidity, or light intensity deviates from the comfortable range, the light turns red to alert the user.

Manual Control Mode:
The system allows users to set desired environmental conditions manually. Through a web-based control interface, users can adjust the comfortable thresholds for temperature, humidity, and light intensity. The interface also provides real-time feedback on environmental data, enabling users to make necessary adjustments.

Intelligent Operation Interface:
Using the ESP8266 and MQTT protocol, users can view real-time environmental data, manually adjust the LED light’s color and brightness, and customize comfort thresholds. The web interface is user-friendly, providing an intuitive platform for interaction.

Future Outlook
Due to the malfunction of the BME680 sensor, the system currently cannot collect more comprehensive environmental data. In the future, I plan to reintegrate the BME680 sensor to enhance the diversity of data collection, covering additional parameters such as air quality and atmospheric pressure.

Future improvements will focus on the following areas:

Multi-Area Control:
Currently, the system controls a single lamp via MQTT. In the future, I aim to expand control to all lamps, dividing them into four independent zones based on data types (temperature, humidity, air pressure, air quality). Each zone will be independently adjustable, enabling more precise environmental management.

User-Customized Light Colors:
The web-based control interface will be enhanced with more customization options, allowing users to set light colors according to their preferences. In addition to adjusting comfort thresholds, users will be able to select light colors tailored to specific needs or personal preferences. This functionality will be implemented through the web interface, providing an intuitive way for users to modify settings and further enhancing system personalization and flexibility.

Further Integration and Analysis of Sensor Data:
With the restoration of the BME680 sensor, the system will collect more comprehensive data, including air quality and air pressure. This will provide users with more detailed environmental feedback. Additionally, I plan to incorporate data analysis and trend prediction features, enabling the system to anticipate future environmental changes and preemptively adjust settings to ensure optimal conditions for learning.

References:
https://www.aivc.org/resource/effects-outdoor-air-supply-rate-office-perceived-air-quality-sick-building-syndrome-sbs
https://www.researchgate.net/publication/354565637_THE_IMPACT_OF_THE_INDOOR_ENVIRONMENTAL_QUALITY_ON_STUDENTS'_PERFORMANCE

