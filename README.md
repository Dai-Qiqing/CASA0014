# Intelligent Environment Monitoring System for Learning Optimization

## Project Overview
This project aims to optimize learning environments by utilizing IoT technology to monitor and adjust key environmental factors such as temperature, humidity, and light intensity in real-time. By addressing these factors, the system enhances focus, comfort, and overall productivity for students.

![System Architecture](images/8aa724528b2bc763224ae47261168fa.jpg)


---

## Features
### **1. Environmental Monitoring and Feedback**
- Real-time collection and adjustment of:
  - **Temperature and Humidity** using DHT11 sensors.
  - **Light Intensity** using TEMT6000 sensors.
- Dynamic light control:
  - **Green Light**: Indicates optimal environmental conditions.
  - **Red Light**: Alerts deviations from ideal settings.
  - **Flashing Red Light**: Signals critical conditions, prompting immediate action (e.g., closing curtains).

### **2. Personalized User Settings**
- **Real-Time Data Display**:
  - Users can monitor environmental data (temperature, humidity, and light intensity) on a simple and user-friendly web interface.
- **Customizable Thresholds**:
  - Allows users to set personalized preferences, such as cooler temperatures or brighter lighting for specific needs.
- **Streamlined Interface Design**:
  - Focus on simplicity to ensure accessibility for all users, including those with limited technical knowledge.

### **3. IoT Data Transmission**
- **Reliable Communication**:
  - Data from sensors is transmitted using the **ESP8266 WiFi module** via the MQTT protocol for real-time monitoring.
- **Dynamic Feedback**:
  - Adjustments to lighting color and brightness are automatically executed based on real-time data and user-defined preferences.

---

## Technologies Used
### **Hardware**
- **ESP8266 WiFi Module**: For wireless data transmission.
- **DHT11 Sensor**: For temperature and humidity monitoring.
- **TEMT6000 Sensor**: For light intensity measurement.

![ESP8266 Code Flowchart](https://raw.githubusercontent.com/Dai-Qiqing/CASA0014/main/images/ESP8266_code_flowchart%20(1)(1).png)

### **Software**
- **Communication Protocol**: MQTT for reliable real-time data transfer.
- **Web Interface**: Designed using HTML/CSS and JavaScript to ensure functionality and user-friendliness.

---

## How to Use
### **1. Set Up Sensors**
- Install the DHT11 sensor for temperature and humidity monitoring.
- Set up the TEMT6000 sensor to measure light intensity.

### **2. Connect to the System**
- Use the ESP8266 module to connect the sensors to the cloud-based MQTT server.

### **3. Access the Web Interface**
- Monitor real-time environmental data via the web interface.
- Customize thresholds to match your preferred settings for optimal comfort.

---

## Challenges and Solutions
### **Challenge 1: Sensor Data Integration**
- **Problem**: Inconsistent data readings from multiple sensors.
- **Solution**: 
  - Developed reliable code for consistent data synchronization.
  - Applied data smoothing techniques to ensure accurate and real-time feedback.

### **Challenge 2: User-Friendly Interface**
- **Problem**: Making the interface accessible to non-technical users.
- **Solution**: 
  - Adopted a modular design approach.
  - Iteratively tested and refined the interface for simplicity and functionality.

### **Challenge 3: Data Reliability**
- **Problem**: Sensor readings were affected by environmental interference.
- **Solution**:
  - Optimized sensor placement (e.g., shielding the light sensor from direct sunlight).
  - Incorporated data calibration to improve reliability.

---

## Future Improvements
- Expand system functionality to integrate with additional smart devices such as air purifiers and curtains.
- Develop advanced analytics for tracking user behavior and optimizing learning environments over time.
- Create mobile app compatibility for on-the-go adjustments and monitoring.

---

## Contributions
This project highlights the potential of IoT in creating smarter, more adaptive educational spaces. By addressing key environmental factors, it enhances learning efficiency and provides a model for future applications in smart classrooms.
 
---

## References
1.Krawczyk, N. and Dębska, L. (2022). Indoor Environment, Lighting Conditions and
Productivity in the Educational Buildings. Civil and Environmental Engineering, 18(2), pp.581–588. doi:https://doi.org/10.2478/cee-2022-0055. Huang, K., Liu, R., Cheng, Y., Hui, M. and Xu, X. (2023). IoT Based Smart Classroom

2.Environmental Monitoring System Design and Implementation. Lecture notes in
electrical engineering, pp.827–834. doi:https://doi.org/10.1007/978-981-99-3951-0_91. 

3.Jiang, H., Iandoli, M., Dessel, S., Liu, S. and Whitehill, J. (n.d.). Measuring students’ thermal comfort and its impact on learning. [online] Available at:
https://files.eric.ed.gov/fulltext/ED599249.pdf?utm_source=chatgpt.com [Accessed
29 Nov. 2024].

---


