#include "AG_LIB.h"
#include <vector>

/**
 * @brief Wi-Fi ağına bağlanmaya çalışır
 *
 * @param ssid Ağa bağlanılacak SSID
 * @param sifre Ağa ait şifre
 * @return true Bağlantı başarılı
 * @return false Bağlantı başarısız
 */
bool Ag::baglanti::kur(const char *ssid, const char *sifre)
{
    WiFi.mode(WIFI_STA);
    Serial.print(F("Wi-Fi baglantisi deneniyor: "));
    Serial.println(ssid);

    WiFi.begin(ssid, sifre);

    uint8_t sayac = 0;
    while (WiFi.status() != WL_CONNECTED && sayac < 20)
    {
        delay(500);
        Serial.print(".");
        sayac++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(F("\nBaglanti basarili."));
        bilgi();
        return true;
    }
    else
    {
        Serial.println(F("\nBaglanti basarisiz."));
        bilgi();
        return false;
    }
}

/**
 * @brief Mevcut Wi-Fi bağlantısını sonlandırır
 */
void Ag::baglanti::kes()
{
    WiFi.disconnect(true);
    Serial.println(F("Wi-Fi baglantisi kesildi."));
}

/**
 * @brief Wi-Fi bağlantısının durumunu kontrol eder
 * @return true Wi-Fi bağlı
 * @return false Wi-Fi bağlı değil
 */
bool Ag::baglanti::sorgu() { return WiFi.status() == WL_CONNECTED; }

/**
 * @brief Mevcut bağlantı hakkında temel bilgileri yazdırır
 */
void Ag::baglanti::bilgi()
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
 * @brief Belirtilen ağ bilgileriyle tekrar bağlanmayı dener(!!! delay kullanır)
 *
 * @param ssid SSID
 * @param sifre Şifre
 * @param deneme_sayisi Deneme sayısı (varsayılan 5)
 */
void Ag::baglanti::tekrar_dene(const char *ssid, const char *sifre, uint8_t deneme_sayisi)
{
    for (uint8_t i = 0; i < deneme_sayisi; i++)
    {
        Serial.print(F("Tekrar baglaniyor (deneme "));
        Serial.print(i + 1);
        Serial.println(F(")..."));

        if (kur(ssid, sifre))
        {
            Serial.println(F("Baglanti tekrar saglandi."));
            return;
        }
        delay(500);
    }

    Serial.println(F("Tum denemeler basarisiz oldu."));
}

/**
 * @brief Önceden bağlanılmış bilinen ağlara otomatik bağlanmayı dener.
 *
 * WiFi.begin() çağrılmadan, esp kendiliğinden bilinen ağa bağlanabilir.
 * @return true - Bağlantı başarılı
 * @return false - Bağlantı başarısız
 */
bool Ag::baglanti::eski_aga_otomatik()
{
    Serial.println(F("Bilinen ağa otomatik bağlanılıyor..."));
    WiFi.begin();
    for (uint8_t i = 0; i < 20; i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println(F("Otomatik bağlantı başarılı."));
            bilgi();
            return true;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println(F("\nOtomatik bağlantı başarısız."));
    return false;
}

/**
 * @brief Statik IP ile bağlantı kurar
 *
 * @param ssid SSID
 * @param sifre Şifre
 * @param ip IP Adresi
 * @param gateway Ağ geçidi
 * @param subnet Alt ağ maskesi
 * @return true - bağlantı başarılı
 * @return false - bağlantı başarısız
 */
bool Ag::baglanti::statik_ip_kur(const char *ssid, const char *sifre, IPAddress ip, IPAddress gateway, IPAddress subnet)
{
    WiFi.config(ip, gateway, subnet);
    return kur(ssid, sifre);
}

/**
 * @brief Satır satır ayrılmış stringi satırlara böler
 *
 * @param input Satır satır veri (örn: "ag1\nag2\nag3\n")
 * @return std::vector<String> Satırlar listesi
 */
std::vector<String> satirlara_bol(const String &input)
{
    std::vector<String> satirlar;
    int start = 0;
    while (true)
    {
        int end = input.indexOf('\n', start);
        if (end == -1)
        {
            String lastLine = input.substring(start);
            if (lastLine.length() > 0)
                satirlar.push_back(lastLine);
            break;
        }
        String line = input.substring(start, end);
        satirlar.push_back(line);
        start = end + 1;
    }
    return satirlar;
}
/**
 * @brief Çevredeki Wi-Fi ağlarını tarar, kullanıcıdan alınan satır-satır SSID ve şifre listesiyle karşılaştırır.
 *        Eşleşen bir SSID bulunursa, aynı satırdaki şifreyle ağa bağlanmayı dener.
 *
 * @details
 * - SSID ve şifreler satır-satır aynı sırada olacak şekilde `\n` ile ayrılmış metinlerde verilmelidir.
 * - SSID eşleşmesi sağlandığında, o satırın şifresi ile bağlantı denenir.
 * - Eşleşme ve bağlantı başarılıysa `true` döner, aksi takdirde `false`.
 *
 * @param ssid_list Satır satır SSID verisi (örn: "ev_agim\nofis_wifi\ndukkan\n")
 * @param sifre_list Satır satır şifre verisi (örn: "12345678\nsifrem2024\ndukkan2023\n")
 *
 * @return true Başarılı bir bağlantı kuruldu
 * @return false Eşleşen bir SSID bulunamadı ya da bağlantı başarısız oldu
 */
bool Ag::baglanti::kayitli_aglardan(const String &ssid_list, const String &sifre_list)
{
    Serial.println(F("Çevredeki ağlar taranıyor..."));
    int bulunan_ag_sayisi = WiFi.scanNetworks();
    if (bulunan_ag_sayisi <= 0)
    {
        Serial.println(F("Hiçbir ağ bulunamadı."));
        return false;
    }

    std::vector<String> ssidler = satirlara_bol(ssid_list);
    std::vector<String> sifreler = satirlara_bol(sifre_list);

    // Taranan her ağı kontrol et
    for (int i = 0; i < bulunan_ag_sayisi; ++i)
    {
        String mevcut_ssid = WiFi.SSID(i);

        // Kayıtlı SSID'ler arasında eşleşme arıyoruz
        for (size_t j = 0; j < ssidler.size(); ++j)
        {
            if (mevcut_ssid == ssidler[j])
            {
                String kullanilacak_sifre = (j < sifreler.size()) ? sifreler[j] : "";

                Serial.print(F("Eşleşen ağ bulundu: "));
                Serial.println(mevcut_ssid);
                Serial.println(F("Bağlantı deneniyor..."));

                if (kur(mevcut_ssid.c_str(), kullanilacak_sifre.c_str()))
                {
                    Serial.println(F("Bağlantı başarılı."));
                    return true;
                }
                else
                {
                    Serial.println(F("Bağlantı başarısız."));
                }
            }
        }
    }

    Serial.println(F("Kayıtlı ağlardan uygun bir eşleşme bulunamadı."));
    return false;
}
