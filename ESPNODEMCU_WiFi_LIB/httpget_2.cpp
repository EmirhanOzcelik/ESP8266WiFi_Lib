#include "AG_LIB.h"

/**
 * @brief İnternetten güncel tarih-saat alır ve verilen işaretçilere yazar.
 *
 * @param sn         İşaretçi: saniye (0-59)
 * @param dk         İşaretçi: dakika (0-59)
 * @param saat       İşaretçi: saat (0-23)
 * @param gun        İşaretçi: gün (1-31)
 * @param ay         İşaretçi: ay (1-12)
 * @param yil        İşaretçi: yıl (örneğin 2025)
 * @param haftagunu  İşaretçi: haftanın günü (1 = Pazar, 7 = Cumartesi)
 * @param gmtOffset  Saat dilimi offset'i saniye cinsinden (örnek: +3 saat için 10800)
 * @param timeout_ms Maksimum bekleme süresi (milisaniye cinsinden)
 */
void Ag::web::zaman_al(int *sn, int *dk, int *saat, int *gun, int *ay, int *yil, int *haftagunu, unsigned long timeout_ms, long gmtOffset)
{
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset, 60000);
    timeClient.begin();
    Serial.println(F("[NTP] NTP istemcisi başlatıldı. Zaman alınmaya çalışılıyor..."));

    unsigned long baslangic = millis();
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
        delay(10);
        if (millis() - baslangic > timeout_ms)
        {
            Serial.println(F("[NTP] Zaman aşımı: NTP sunucusundan zaman alınamadı."));
            return;
        }
    }

    unsigned long epochTime = timeClient.getEpochTime();

    time_t rawTime = (time_t)epochTime;
    struct tm *ptm = gmtime(&rawTime);

    *sn = ptm->tm_sec;
    *dk = ptm->tm_min;
    *saat = ptm->tm_hour;
    *gun = ptm->tm_mday;
    *ay = ptm->tm_mon + 1;
    *yil = ptm->tm_year + 1900;
    *haftagunu = ptm->tm_wday + 1;

    Serial.println(F("[NTP] Tarih ve saat bilgisi alındı:"));
    Serial.printf("  Tarih: %02d/%02d/%04d\n", *gun, *ay, *yil);
    Serial.printf("  Saat : %02d:%02d:%02d\n", *saat, *dk, *sn);
    Serial.printf("  Hafta Günü: %d (Pazar=1)\n", *haftagunu);
}
