#include "AG_LIB.h"

/**
 * @brief Sinyal gücünü seviyeli olarak döndürür (0–3 arası).
 *
 * Bağlantı durumunu kontrol eder, RSSI değerine göre
 * sinyal seviyesini belirler:
 * - 3: Çok Güçlü (>= -50 dBm)
 * - 2: Orta       (>= -65 dBm)
 * - 1: Zayıf      (>= -75 dBm)
 * - 0: Çok zayıf veya bağlantı yok
 *
 * @return uint8_t Sinyal seviyesi (0–3)
 *
 * @code
 * ogren net;
 * uint8_t seviye = net.sinyal_gucu();
 * @endcode
 */
uint8_t Ag::ogren::sinyal_gucu()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Wi-Fi baglantisi yok. Sinyal gücü ölçülemiyor."));
        return 0;
    }

    int rssi = WiFi.RSSI();
    Serial.print(F("RSSI Degeri: "));
    Serial.print(rssi);
    Serial.println(F(" dBm"));

    if (rssi >= -50)
    {
        Serial.println(F("Sinyal Seviyesi: Çok Güçlü (3/3)"));
        return 3;
    }
    else if (rssi >= -65)
    {
        Serial.println(F("Sinyal Seviyesi: Orta (2/3)"));
        return 2;
    }
    else if (rssi >= -75)
    {
        Serial.println(F("Sinyal Seviyesi: Zayıf (1/3)"));
        return 1;
    }
    else
    {
        Serial.println(F("Sinyal Seviyesi: Çok Zayıf (0/3)"));
        return 0;
    }
}

/**
 * @brief Wi-Fi bağlantı durumunu açıklamalı olarak seri monitöre yazdırır.
 *
 * Bağlantı durumu aşağıdaki gibi olabilir:
 * - WL_CONNECTED
 * - WL_NO_SSID_AVAIL
 * - WL_CONNECT_FAILED
 * - WL_IDLE_STATUS
 * - WL_DISCONNECTED
 * - WL_SCAN_COMPLETED
 *
 * @code
 * ogren net;
 * net.durum();
 * @endcode
 */
void Ag::ogren::durum()
{
    wl_status_t durum = WiFi.status();
    switch (durum)
    {
    case WL_CONNECTED:
        Serial.println(F("Wi-Fi baglantisi basarili."));
        break;
    case WL_NO_SSID_AVAIL:
        Serial.println(F("HATA: Belirtilen SSID bulunamadi."));
        break;
    case WL_CONNECT_FAILED:
        Serial.println(F("HATA: SSID veya sifre hatali."));
        break;
    case WL_IDLE_STATUS:
        Serial.println(F("Bilgi: Baglanti beklemede (IDLE)."));
        break;
    case WL_DISCONNECTED:
        Serial.println(F("Bilgi: Wi-Fi baglantisi kesildi."));
        break;
    case WL_SCAN_COMPLETED:
        Serial.println(F("Bilgi: Tarama tamamlandi."));
        break;
    default:
        Serial.println(F("Bilinmeyen Wi-Fi durumu."));
        break;
    }
}

/**
 * @brief Bağlı olduğu ağın IP adresini döndürür.
 *
 * @return String IP adresi (bağlantı yoksa boş string)
 *
 * @code
 * ogren net;
 * String ip = net.ip_adresi();
 * @endcode
 */
String Ag::ogren::ip_adresi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        IPAddress ip = WiFi.localIP();
        String ipStr = ip.toString();
        Serial.print(F("IP Adresi: "));
        Serial.println(ipStr);
        return ipStr;
    }
    else
    {
        Serial.println(F("IP adresi alinamadi. Wi-Fi baglantisi yok."));
        return "";
    }
}

/**
 * @brief Bağlı olunan Wi-Fi ağının adını döndürür (SSID).
 *
 * @return String SSID (bağlantı yoksa boş string)
 *
 * @code
 * ogren net;
 * String ssid = net.ag_adi();
 * @endcode
 */
String Ag::ogren::ag_adi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        String ssid = WiFi.SSID();
        Serial.print(F("Bagli olunan ag: "));
        Serial.println(ssid);
        return ssid;
    }
    else
    {
        Serial.println(F("Wi-Fi baglantisi yok. SSID alinamadi."));
        return "";
    }
}

/**
 * @brief Cihaza ait MAC adresini döndürür.
 *
 * @return String MAC adresi
 *
 * @code
 * ogren net;
 * String mac = net.mac_adresi();
 * @endcode
 */
String Ag::ogren::mac_adresi()
{
    String mac = WiFi.macAddress();
    Serial.print(F("MAC Adresi: "));
    Serial.println(mac);
    return mac;
}

/**
 * @brief Etraftaki Wi-Fi ağlarını tarar ve sinyal gücüne göre güçlüden zayıfa sıralı olarak listeler.
 *
 * Bu fonksiyon etraftaki Wi-Fi ağlarını tarar, sinyal gücüne göre sıralar ve ağları biçimlendirilmiş
 * bir tablo şeklinde string olarak döndürür. Liste başlığında toplam ağ sayısı, en yüksek RSSI değeri
 * ve verilen tarih/zaman bilgisi yer alır.
 *
 * @param saniye  Saniye bilgisi (0–59)
 * @param dakika  Dakika bilgisi (0–59)
 * @param saat    Saat bilgisi (0–23)
 * @param gun     Gün bilgisi (1–31)
 * @param ay      Ay bilgisi (1–12)
 * @param yil     Yıl bilgisi (örn. 2025)
 *
 * @return String Biçimlendirilmiş Wi-Fi ağı listesi
 *
 * @code
 * {3,-42 dBm,15:24:10-27/05/2025}
 * No  | RSSI     | SSID                 | Guvenlik
 * ----|----------|----------------------|----------
 * #1  | -42 dBm  | MyHomeNetwork        | sifreli
 * #2  | -60 dBm  | OpenGuest            | sifresiz
 * #3  | -75 dBm  | EvNet                | sifreli
 * @endcode
 */
String Ag::ogren::cevredeki_aglar(uint8_t saniye, uint8_t dakika, uint8_t saat,
                              uint8_t gun, uint8_t ay, uint16_t yil)
{
    Serial.println(F("Ağlar taranıyor..."));
    int n = WiFi.scanNetworks();
    if (n <= 0)
    {
        Serial.println(F("Hiçbir ağ bulunamadı."));
        return F("Hiçbir ağ bulunamadı.\n");
    }

    // İndeks sıralama dizisi
    int indices[n];
    for (int i = 0; i < n; ++i)
        indices[i] = i;

    // Sinyal gücüne göre sıralama
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
            {
                int temp = indices[i];
                indices[i] = indices[j];
                indices[j] = temp;
            }
        }
    }

    int en_yuksek_rssi = WiFi.RSSI(indices[0]);

    String k = "";
    if (saniye > 60 || dakika > 60 || saat > 60)
        k = "?:?:?-";
    else
        k = String(saniye) + ":" + String(dakika) + ":" + String(saat) + "-";
    String l = "";
    if (yil < 2000 || ay > 12 || gun > 33)
        l = "?/?/?";
    else
        l = String(gun) + "/" + String(ay) + "/" + String(yil);

    String baslik = "{";
    baslik += String(n) + ",";                  // Toplam ağ sayısı
    baslik += String(en_yuksek_rssi) + " dBm,"; // En yüksek sinyal gücü
    baslik += k;
    baslik += l;
    baslik += "}\n";

    // İçerik tablosu başlığı
    String sonuc = baslik;
    sonuc += F("No \tRSSI   \tSSID                \tGuvenlik\n");
    sonuc += F("---\t-------\t--------------------\t----------\n");

    // Her bir ağı yaz
    for (int i = 0; i < n; ++i)
    {
        int idx = indices[i];
        int rssi = WiFi.RSSI(idx);
        String ssid = WiFi.SSID(idx);
        String guvenlik;

#if defined(ESP8266)
        guvenlik = (WiFi.encryptionType(idx) == ENC_TYPE_NONE) ? F("sifresiz") : F("sifreli");
#else
        guvenlik = (WiFi.encryptionType(idx) == WIFI_AUTH_OPEN) ? F("sifresiz") : F("sifreli");
#endif

        // SSID hizalama
        while (ssid.length() < 20)
            ssid += ' ';

        sonuc += "#" + String(i + 1) + "\t";
        sonuc += String(rssi) + F(" dBm\t");
        sonuc += ssid + "\t";
        sonuc += guvenlik + F("\n");
    }

    Serial.println(sonuc);
    return sonuc;
}
