#pragma once
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "ESPAsyncTCP.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include "PubSubClient.h"
#define RAM_GOSTER Serial.println(F("[RAM] Şu anki boş alan(byte) :") + String(ESP.getFreeHeap()));

extern WiFiClientSecure secureClient;

class Ag
{
private:
    friend struct ogren;
    friend struct baglanti;
    friend struct web;

public:
    struct web
    {
        void zaman_al(int *sn, int *dk, int *saat, int *gun, int *ay, int *yil, int *haftagunu, unsigned long timeout_ms = 8000, long gmtOffset = 10800);
        String json_veri_string(const String &json, const String &key);
        int json_veri_int(const String &json, const String &path);
        float json_veri_float(const String &json, const String &path);
        String json_al(const String &url, uint32_t timeout_ms = 5000);
        void siteGuvenlikBilgileriniAl(const char *domain);
        String json_al_detayli(const String &baseUrl, const String &apiKey, const String &il, const String &ilce = "", uint32_t timeout_ms = 5000);
    } web;

    struct ogren
    {
        uint8_t sinyal_gucu();
        void durum();
        String ip_adresi();
        String ag_adi();
        String mac_adresi();
        String cevredeki_aglar(uint8_t saniye = 255, uint8_t dakika = 255, uint8_t saat = 255, uint8_t gun = 255, uint8_t ay = 255, uint16_t yil = 255);
    } ogren;
    struct baglanti
    {
        bool kur(const char *ssid, const char *sifre);
        void kes();
        bool sorgu();
        void bilgi();
        void tekrar_dene(const char *ssid, const char *sifre, uint8_t deneme_sayisi);
        bool eski_aga_otomatik();
        bool statik_ip_kur(const char *ssid, const char *sifre, IPAddress ip, IPAddress gateway, IPAddress subnet);
        bool kayitli_aglardan(const String &ssid_list, const String &sifre_list);
    } baglanti;

    class server
    {
    private:
        friend class Ag;
        // HiveMQ Cloud TLS bağlantı bilgileri
        String mqttServer;        // url
        uint16_t mqttPort = 8883; // liman
        String mqttUser;          // HiveMQ Cloud kullanıcı adın
        String mqttPassword;      // HiveMQ Cloud şifren
    public:
        bool baglan();
        void baglantiyi_sonlandir();
        void abone(const char *topic, bool islem);
        void veriGonder(const char *topic, const char *mesaj);
        void veri_al();
        String veri_sade();
        void veri_temizle();
        /// @brief bağlantı yapacağınız sunucu url
        void ayarla_url(const String &k) { mqttServer = k; }
        /// @brief bağlantı yapacağınız port : opsiyonel(8883)
        void ayarla_port(uint16_t k) { mqttPort = k; }
        /// @brief baglantı yapacagınız sunucuda oluşturduğunuz profilin kullanıcı adı
        void ayarla_KullaniciAdi(const String &k) { mqttUser = k; }
        /// @brief baglantı yapacagınız sunucuda oluşturduğunuz profilin şifresi
        void ayarla_KullaniciSifre(const String &k) { mqttPassword = k; }
        static void aktif_tut();
    } server;
};

extern String mesaj;
extern void arkaplanda_veriBekle(char *topic, byte *payload, unsigned int length);
