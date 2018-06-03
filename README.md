# ESP8266智能门铃魔改代码

### 智能门铃的魔改过程请访问：[blog.tffan.me/doorbell01](https://blog.tffan.me/doorbell01)

**本代码需要[Visual Studio Code](https://code.visualstudio.com/)或[Atom](https://atom.io/)使用[Platform IO IDE](https://platformio.org/platformio-ide)插件才能编译和烧录芯片。**

### 代码依赖：

以下代码库可以在Platform IO的Library Manager中搜索安装。

* PubSubClient
* DFRobotDFPlayerMini
* ESPHelper

以下代码库需要手动下载并放入Platform IO的Library文件夹中

* [Metro](https://github.com/thomasfredericks/Metro-Arduino-Wiring/archive/master.zip)

### 注意事项

编译之前请打开src/consts.h文件，填入对应内容：

* const_ssid：wifi名称
* const_ssid_pass：wifi密码;
* const_mqtt_broker：MQTT服务器，**不填则可以当普通门铃使用**
* const_mqtt_user：MQTT服务器的用户名，没有可以不填
* const_mqtt_user_pass：MQTT服务器的用户名密码，没有可以不填
* const_ota_pass：OTA的验证密码，可以不填

有些ESP8266的开发板要手动激活烧录模式，具体如何激活请自行搜索。

### 使用OTA

OTA是不需要连接电脑也可以上传固件给ESP8266芯片的功能

1. 首先要确保ESP8266正在正常运行
2. ESP8266和你的电脑要在同个局域网下
3. 打开paltformio.ini文件
4. 新起一行填入：`upload_port = 你的ESP8266的局域网地址`
5. 如果你有填OTA验证密码,再新起一行填入：`upload_flags = --auth=OTA验证密码`



