#include "AG_LIB.h"
#define _MAX_ABONELIK_ 8

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

bool _debug = true;
String gecici_msj = "";
String mesaj;
String abonelikler[_MAX_ABONELIK_];
uint8_t _abonelik_sayac = 0;
void ayarla_debug_izin(bool k);
void ayarla_debug_izin(bool k) { _debug = k; }

bool abonelik_sorgula(String veerri);
bool abonelige_ekle(String veerri);
bool aboneligi_kaldir(String veerri);

/// @brief gerekli ayarlamalardan sonra server a baglanmak için gerekli fonksiyon.
/// @return baglanabilirse 1 baglanamazsa 0
bool Ag::server::baglan()
{
    secureClient.setInsecure(); // Sertifika kontrolü devre dışı (test için)
    mqttClient.setServer(mqttServer.c_str(), mqttPort);
    if (_debug)
        Serial.println(F("[SERVER] Cloud sunucusuna bağlanılıyor..."));

    String k = mqttUser + String(random(13, 17) * random(97, 100) + 1 - millis() / 10);
    if (mqttClient.connect(k.c_str(), mqttUser.c_str(), mqttPassword.c_str()))
    {
        for (int v = 0; v < _abonelik_sayac; v++)
        {
            if (abonelikler[v] != "")
            {
                abone(abonelikler[v].c_str(), 1);
            }
        }
        if (_debug)
        {
            Serial.println(F("[SERVER] MQTT bağlantısı başarılı."));
            Serial.print(F("[SERVER] port : "));
            Serial.println(mqttPort);
            Serial.print(F("[SERVER] isim : "));
            Serial.println(mqttUser);
            Serial.print(F("[SERVER] benzersiz ID : "));
            Serial.println(k);
        }
        return true;
    }
    else
    {
        if (_debug)
        {
            Serial.print(F("[SERVER] MQTT bağlantısı başarısız, durum: "));
            Serial.println(mqttClient.state());
        }
        return false;
    }
}
/// @brief server baglantısını keser
void Ag::server::baglantiyi_sonlandir()
{
    if (mqttClient.connected())
    {
        mqttClient.disconnect();
        mesaj = "";
        if (_debug)
            Serial.println(F("[SERVER] MQTT bağlantısı sonlandırıldı."));
    }
    else if (_debug)
        Serial.println(F("[SERVER] Zaten bağlantı yok."));
}

/// @brief server içinde verilen başlığa abonelik başlatır/sonlandırır (dinlemeye alır)
/// @param topic başlık
/// @param islem 0 ise abonelik varsa sonlanır, 1 ise abonelik yoksa başlatılır
void Ag::server::abone(const char *topic, bool islem)
{
    if (islem)
    {
        if (mqttClient.subscribe(topic))
        {
            bool c = abonelige_ekle(topic);
            if (_debug)
            {
                if (c)
                    Serial.println(F("[SERVER] Abone olundu : ") + String(topic));
                else
                    Serial.println(F("[SERVER] Zaten abonesiniz : ") + String(topic));
            }
        }
        else if (_debug)
            Serial.println(F("[SERVER] Abonelik başarısız : ") + String(topic));
    }
    else
    {
        if (mqttClient.unsubscribe(topic))
        {
            bool c = aboneligi_kaldir(topic);
            if (_debug)
            {
                if (c)
                    Serial.println(F("[SERVER] Abonelik iptal edildi : ") + String(topic));
                else
                    Serial.println(F("[SERVER] Zaten abone degilsiniz : ") + String(topic));
            }
        }
        else if (_debug)
            Serial.println(F("[SERVER] Abonelik iptali başarısız: ") + String(topic));
    }
}

/// @brief server içi veri gönderimi yapar
/// @param topic verinin gönderileceği başlık
/// @param mesaj veri
void Ag::server::veriGonder(const char *topic, const char *mesaj)
{
    if (!mqttClient.connected())
    {
        if (_debug)
            Serial.println(F("[SERVER] MQTT bağlantısı yok. Mesaj gönderilemedi."));
        return;
    }
    if (mqttClient.publish(topic, mesaj))
    {
        if (_debug)
            Serial.printf_P(PSTR("[SERVER] Mesaj gönderildi: [%s] -> %s\n"), topic, mesaj);
    }
    else if (_debug)
    {
        Serial.printf_P(PSTR("[SERVER] Mesaj gönderilemedi: [%s]\n"), topic);
    }
}

/// @brief setup da çağırınız callback fonksiyonu ile aboneliklere gelen veriyi alır
void Ag::server::veri_al() { mqttClient.setCallback(arkaplanda_veriBekle); }
/// @brief alınan veriyi şu formatta döndürür : topic1;Veri\n,topic2;Veri\n,...
/// @return String türünde çıktı verir
String Ag::server::veri_sade() { return mesaj; }
/// @brief alınan veri paketini temizler. Bellek taşmaması için sıfırlamanız gerekli!!!
void Ag::server::veri_temizle() { mesaj = ""; };
/// @brief loop kısmında kullanmanız gereken fonksiyon. Server aktifğliği saglar
void Ag::server::aktif_tut() { mqttClient.loop(); }

void arkaplanda_veriBekle(char *topic, byte *payload, unsigned int length)
{
    if (!abonelik_sorgula(topic))
        return;
    mesaj += topic;
    mesaj += ";";
    for (unsigned int i = 0; i < length; i++)
        gecici_msj += (char)payload[i];
    mesaj += gecici_msj + "\n";
    if (_debug)
    {
        Serial.println(F("\n [SERVER] \n--- [MQTT Mesaj Alındı] ---"));
        Serial.println(F("Konu: ") + String(topic));
        Serial.println(F("İçerik: ") + gecici_msj);
    }
    gecici_msj = "";
}
bool abonelige_ekle(String veerri)
{
    if (_abonelik_sayac >= _MAX_ABONELIK_)
        return false;
    if (abonelik_sorgula(veerri))
        return false;
    abonelikler[_abonelik_sayac++] = veerri;
    return true;
}
bool abonelik_sorgula(String veerri)
{
    for (int i = 0; i < _abonelik_sayac; i++)
    {
        if (abonelikler[i] == veerri)
            return true;
    }
    return false;
}
bool aboneligi_kaldir(String veerri)
{
    for (int i = 0; i < _abonelik_sayac; i++)
    {
        if (abonelikler[i] == veerri)
        {
            for (int j = i; j < _abonelik_sayac - 1; j++)
                abonelikler[j] = abonelikler[j + 1];
            abonelikler[_abonelik_sayac - 1] = "";
            _abonelik_sayac--;
            return true;
        }
    }
    return false;
}
