/*
接线说明:无

程序说明:开发板发布MQTT信息,在电脑端通过MQTT.fx软件来对开发板发布的主题进行订阅,
         在电脑端就可以实现查看开发板发布的信息
         这次发布消息使用了保留消息的方式,当有客户端订阅了该主题后就会即刻收到该保留消息
         如果需要删除保留消息的发送,只需要发送一条内容为零字节的消息即可实现删除保留消息

注意事项:

函数示例:无

作者:灵首

时间:2023_4_17

*/
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiMulti.h>


WiFiMulti wifi_multi; // 建立WiFiMulti 的对象,对象名称是 wifi_multi
WiFiClient wifiClient;  //建立WiFiClient
PubSubClient mqttClient(wifiClient);  //根据WiFiClient来建立PubSubClient对象


const char* mqttServer = "test.ranye-iot.net";  //这是需要连接的MQTT服务器的网址,可更改



/**
* @brief 连接WiFi的函数
*
* @param 无
* @return 无
*/
void wifi_multi_con(void)
{
  int i = 0;
  while (wifi_multi.run() != WL_CONNECTED)
  {
    delay(1000);
    i++;
    Serial.print(i);
  }
}



/**
* @brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
*
* @param 无
* @return 无
*/
void wifi_multi_init(void)
{
  wifi_multi.addAP("haoze2938", "12345678");
  wifi_multi.addAP("LINGSOU1029", "12345678");
  wifi_multi.addAP("haoze1029", "12345678"); // 通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}



/**
* @brief 生成客户端名称并连接服务器同时串口输出信息
*
* @param 无
* @return 无
*/
void connectMQTTServer(){
  //生成客户端的名称(同一个服务器下不能存在两个相同的客户端名称)
  String clientId = "esp32s3---" + WiFi.macAddress();

  //尝试连接服务器,并通过串口输出有关信息
  if(mqttClient.connect(clientId.c_str())){
    Serial.println("MQTT Server Connect successfully!!!.\n");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.print("\n");
    Serial.println("ClientId:");
    Serial.println(clientId);
    Serial.print("\n");
  }
  else{
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    Serial.print("\n");
    delay(3000);
  }

}



/**
* @brief 发布MQTT信息,包含建立主题以及发布消息
*
* @param 无
* @return 无
*/
void pubMQTTRemainedMsg(){
  // 建立发布主题。主题名称以lingsou-为前缀，后面添加设备的MAC地址。
  // 这么做是为确保不同用户进行MQTT信息发布时，ESP32s3客户端名称各不相同，
  //同时建立主题后的两句程序是将 string 的数据转换为 char[] 字符数组类型
  //因为在之后的发布操作中只支持字符数组作为参数
  String topicString = "lingsou-" + WiFi.macAddress();  
  char publishTopic[topicString.length() + 1];  
  strcpy(publishTopic, topicString.c_str());  //将字符串数据 topicString 转换为字符数组类型的数据 publishTopic
 
  // 建立发布的保留信息。信息内容是 Hello World ,and this is retained msg!!! 
  String messageString = "Hello World ,and this is retained msg!!! " ; 
  char publishRemainedMsg[messageString.length() + 1];   
  strcpy(publishRemainedMsg, messageString.c_str());

  // // 删除保留消息
  // String messageString =""; 
  // char publishRemainedMsg[messageString.length() + 1];   
  // strcpy(publishRemainedMsg, messageString.c_str());
  
  // 实现ESP32s3向主题发布保留信息
  if(mqttClient.publish(publishTopic, publishRemainedMsg,true)){
    Serial.print("Publish Topic:");
    Serial.print(publishTopic);
    Serial.print("\n");
    Serial.print("Publish Retained message:");
    Serial.print(publishRemainedMsg);
    Serial.print("\n");    
  } else {
    Serial.print("Message Publish Failed.\n"); 
  }
}



void setup() {
  // 连接串口
  Serial.begin(9600);
  Serial.print("serial is OK\n");

  // wifi 连接设置
  wifi_multi_init();
  wifi_multi_con();
  Serial.print("wifi connected!!!\n");

  // 输出连接信息(连接的WIFI名称及开发板的IP地址)
  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\n");
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());
  Serial.print("\n");

  //设置连接的MQTT服务器
  mqttClient.setServer(mqttServer,1883);

  //连接MQTT服务器
  connectMQTTServer();

  //发布保留消息
  if(mqttClient.connected()){
    pubMQTTRemainedMsg();
  }

}



void loop() {
  //这是在保持客户端心跳
  mqttClient.loop();  
}