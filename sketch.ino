#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <DNSServer.h>

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <Arduino.h>

#include "esp_wifi.h"

const wifi_promiscuous_filter_t filt = { // Idk what this does
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};

typedef struct
{
  String recievedMac;
  String sendMac;
} DevicePacket;

#define maxCh 14 // max Channel -> US = 11, EU = 13, Japan = 14

DevicePacket packets[1024];
int packetCount = 0;

int curChannel = 1;

typedef struct
{
  unsigned frame_ctrl : 16;
  unsigned duration_id : 16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl : 16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct
{
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

// WiFi devices sniffer
void sniffer(void *buff, wifi_promiscuous_pkt_type_t type)
{
  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  if (packetCount > 256)
  {
    return;
  }

  String addr1;
  for (int i = 0; i < 6; i++)
  {
    addr1 += String(hdr->addr1[i], HEX);
  }

  String addr2;
  for (int i = 0; i < 6; i++)
  {
    addr2 += String(hdr->addr2[i], HEX);
  }

  if (addr2.length() < 12 || addr1.length() < 12)
  {
    // mac is not complete! 12 chars needed
    return;
  }

  // add : to mac
  addr1 = addr1.substring(0, 2) + ":" + addr1.substring(2, 4) + ":" + addr1.substring(4, 6) + ":" + addr1.substring(6, 8) + ":" + addr1.substring(8, 10) + ":" + addr1.substring(10, 12);
  addr2 = addr2.substring(0, 2) + ":" + addr2.substring(2, 4) + ":" + addr2.substring(4, 6) + ":" + addr2.substring(6, 8) + ":" + addr2.substring(8, 10) + ":" + addr2.substring(10, 12);

  addr1.toUpperCase();
  addr2.toUpperCase();

  DevicePacket message;

  message.recievedMac = addr1;
  message.sendMac = addr2;

  packets[packetCount] = message;
  packetCount++;
}

void logWifiAP()
{
  int n = WiFi.scanNetworks();

  if (n != 0)
  {
    for (int i = 0; i < n; ++i)
    {
      Serial.print("AR");
      Serial.print(",");
      Serial.print(WiFi.SSID(i));
      Serial.print(",");
      Serial.print(WiFi.BSSIDstr(i));
      Serial.print(",");
      Serial.print(WiFi.RSSI(i));
      Serial.print(",");
      Serial.print(WiFi.channel(i));

      Serial.print("\n");
      delay(30);
    }
  }

  delay(n * 100);
}

void logWifiDevice()
{
  for (int i = 0; i < packetCount; i++)
  {

    DevicePacket pkt = packets[i];

    Serial.print("PK");
    Serial.print(",");
    Serial.print(pkt.recievedMac);
    Serial.print(",");
    Serial.print(pkt.sendMac);

    Serial.print("\n");

    packets[i] = {};

    delay(30);
  }

  packetCount = 0;
}

// sniff

void setup()
{
  Serial.begin(115200);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
}

void loop()
{
  int waitTime = packetCount * 100;

  if (curChannel > maxCh)
  {
    curChannel = 1;
  }
  curChannel++;
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
  logWifiAP();
  logWifiDevice();
  delay(waitTime);
}
