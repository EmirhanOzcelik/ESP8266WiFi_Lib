#include "AG_LIB.h"

/**
 * @brief JSON metni içinden iç içe noktalı anahtarla string değer döner.
 *
 * @param json JSON verisi (örnek: "{\"user\":{\"name\":\"ali\"}}")
 * @param path Nokta ile ayrılmış yol (örnek: "user.name")
 * @return String Değer string ise döner, yoksa boş string ("")
 */
String Ag::web::json_veri_string(const String &json, const String &path)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        Serial.print("[JSON] Hata: ");
        Serial.println(error.c_str());
        return "";
    }

    JsonVariant current = doc.as<JsonVariant>();
    size_t start = 0;

    while (start < path.length())
    {
        int dotIndex = path.indexOf('.', start);
        String key = (dotIndex == -1) ? path.substring(start) : path.substring(start, dotIndex);

        if (!current.is<JsonObject>())
            return "";
        current = current[key];
        if (current.isNull())
            return "";

        if (dotIndex == -1)
            break;
        start = dotIndex + 1;
    }

    return current.is<String>() ? current.as<String>() : "";
}

/**
 * @brief JSON metni içinden iç içe noktalı anahtarla int değer döner.
 *
 * @param json JSON verisi (örnek: "{\"user\":{\"age\":30}}")
 * @param path Nokta ile ayrılmış yol (örnek: "user.age")
 * @return int Değer int ise döner, yoksa 0
 */
int Ag::web::json_veri_int(const String &json, const String &path)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        Serial.print("[JSON] Hata: ");
        Serial.println(error.c_str());
        return 0;
    }

    JsonVariant current = doc.as<JsonVariant>();
    size_t start = 0;

    while (start < path.length())
    {
        int dotIndex = path.indexOf('.', start);
        String key = (dotIndex == -1) ? path.substring(start) : path.substring(start, dotIndex);

        if (!current.is<JsonObject>())
            return 0;
        current = current[key];
        if (current.isNull())
            return 0;

        if (dotIndex == -1)
            break;
        start = dotIndex + 1;
    }

    return current.is<int>() ? current.as<int>() : 0;
}

/**
 * @brief JSON metni içinden iç içe noktalı anahtarla float değer döner.
 *
 * @param json JSON verisi (örnek: "{\"user\":{\"height\":175.5}}")
 * @param path Nokta ile ayrılmış yol (örnek: "user.height")
 * @return float Değer float veya double ise döner, yoksa 0.0f
 */
float Ag::web::json_veri_float(const String &json, const String &path)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        Serial.print("[JSON] Hata: ");
        Serial.println(error.c_str());
        return 0.0f;
    }

    JsonVariant current = doc.as<JsonVariant>();
    size_t start = 0;

    while (start < path.length())
    {
        int dotIndex = path.indexOf('.', start);
        String key = (dotIndex == -1) ? path.substring(start) : path.substring(start, dotIndex);

        if (!current.is<JsonObject>())
            return 0.0f;
        current = current[key];
        if (current.isNull())
            return 0.0f;

        if (dotIndex == -1)
            break;
        start = dotIndex + 1;
    }

    return current.is<float>() || current.is<double>() ? current.as<float>() : 0.0f;
}

/**
 * @brief Belirtilen URL'den HTTP veya HTTPS ile veri çeker.
 *
 * Wi-Fi bağlantısı kontrol edilir, belirtilen süre içinde (timeout_ms) işlem tamamlanmazsa zaman aşımı olur.
 * HTTPS için sertifika doğrulaması yapılmaz (client.setInsecure()).
 * Gelen veri String olarak döner. Başarısız veya zaman aşımında boş string döner.
 *
 * @param url Veri çekilecek tam URL (http:// veya https:// ile başlamalı)
 * @param timeout_ms Zaman aşımı süresi (milisaniye)
 * @return String İstek başarılı ise gelen veri, aksi halde boş string
 */
String Ag::web::json_al(const String &url, uint32_t timeout_ms)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Wi-Fi bağlı değil."));
        return "";
    }

    uint32_t baslangic = millis();
    bool is_https = url.startsWith("https://");

    auto zaman_asimi_kontrol = [&]() -> bool {
        return (millis() - baslangic > timeout_ms);
    };

    HTTPClient http;
    String payload = "";

    auto istek_yap = [&](auto &client) -> bool {
        if (!http.begin(client, url))
        {
            Serial.println(is_https ? F("[WEB] HTTPS başlatılamadı.") : F("[WEB] HTTP başlatılamadı."));
            return false;
        }

        if (zaman_asimi_kontrol())
        {
            Serial.println(F("[WEB] Zaman aşımı - bağlantı."));
            http.end();
            return false;
        }

        int httpCode = http.GET();

        if (zaman_asimi_kontrol())
        {
            Serial.println(F("[WEB] Zaman aşımı - veri alımı."));
            http.end();
            return false;
        }

        if (httpCode == HTTP_CODE_OK)
        {
            payload = http.getString();
            Serial.printf(is_https ? "[WEB] HTTPS OK\n" : "[WEB] HTTP OK\n");
            Serial.println(payload);
        }
        else
        {
            Serial.printf("[WEB] HTTP HATA %d: %s\n", httpCode, http.errorToString(httpCode).c_str());
        }

        http.end();
        return httpCode == HTTP_CODE_OK;
    };

    if (is_https)
    {
        WiFiClientSecure client;
        client.setInsecure();
        Serial.println(F("[WEB] HTTPS deneniyor..."));
        if (!istek_yap(client)) return "";
    }
    else
    {
        WiFiClient client;
        Serial.println(F("[WEB] HTTP deneniyor..."));
        if (!istek_yap(client)) return "";
    }

    return payload;
}


#define _DETAY_ 0
/// @brief bu fonksiyon kullanım dışı üstüne çalışmak için httpget_1.cpp dosyasını düzenleyın
/// @param domain sorgulama için domain adresi
void Ag::web::siteGuvenlikBilgileriniAl(const char *domain)
{
#if _DETAY_ == 1

    const char *sslLabsApiHost = "api.ssllabs.com";
    const char *sslLabsApiPath = "/api/v3/analyze?host=";

    const char *whoisApiHost = "www.whoisxmlapi.com";
    const char *whoisApiPath = "/whoisserver/WhoisService?apiKey=YOUR_API_KEY&domainName=";
    Serial.println("\n--- Site Güvenlik Bilgileri ---");

    // --- Sertifika ve HTTP Header bilgileri ---
    {
        WiFiClientSecure client;
        client.setInsecure();

        Serial.printf("Sunucuya bağlanılıyor: %s:443\n", domain);
        if (!client.connect(domain, 443))
        {
            Serial.println("Bağlanılamadı!");
            return;
        }

        X509List *cert = client.getPeerCertificate();
        if (cert)
        {
            Serial.println("=== Sertifika Bilgileri ===");
            Serial.printf("Yayıncı (Issuer): %s\n", cert->issuer().CN().c_str());
            Serial.printf("Sahibi (Subject): %s\n", cert->subject().CN().c_str());
            Serial.printf("Geçerlilik Başlangıcı: %s\n", cert->validFrom().c_str());
            Serial.printf("Geçerlilik Bitişi: %s\n", cert->validTo().c_str());
            Serial.printf("Parmak İzi (SHA1): %s\n", cert->sha1().c_str());
        }
        else
        {
            Serial.println("Sertifika alınamadı!");
        }

        Serial.println("=== HTTP Başlıkları ===");
        HTTPClient http;
        http.begin(client, String("https://") + domain);
        int httpCode = http.GET();
        if (httpCode > 0)
        {
            Serial.printf("HTTP Kod: %d\n", httpCode);
            for (int i = 0; i < http.headers(); i++)
            {
                Serial.printf("%s: %s\n", http.headerName(i).c_str(), http.header(i).c_str());
            }
        }
        else
        {
            Serial.printf("HTTP isteği başarısız: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();

        client.stop();
    }

    // --- HTTP GET ve JSON parse fonksiyonu (iç fonksiyon olarak) ---
    auto httpGet = [](const String &url) -> String
    {
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure();

        http.begin(client, url);
        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            http.end();
            return payload;
        }
        else
        {
            Serial.printf("HTTP GET başarısız: %s\n", http.errorToString(httpCode).c_str());
            http.end();
            return "";
        }
    };

    // --- SSL Labs API çağrısı ---
    {
        String url = String("https://") + sslLabsApiHost + sslLabsApiPath + domain;

        Serial.println("\nSSL Labs API çağrılıyor...");
        String json = httpGet(url);
        if (json.length() == 0)
        {
            Serial.println("SSL Labs API cevabı alınamadı.");
        }
        else
        {
            StaticJsonDocument<4096> doc;
            DeserializationError err = deserializeJson(doc, json);
            if (err)
            {
                Serial.print("JSON parse hatası: ");
                Serial.println(err.c_str());
            }
            else
            {
                Serial.println("=== SSL Labs Güvenlik Bilgileri ===");
                if (doc.containsKey("endpoints") && doc["endpoints"].size() > 0)
                {
                    const char *grade = doc["endpoints"][0]["grade"];
                    Serial.printf("Güvenlik Notu (Grade): %s\n", grade ? grade : "Yok");
                }
                else
                {
                    Serial.println("Grade bilgisi bulunamadı.");
                }
            }
        }
    }

    // --- Whois API çağrısı ---
    {
        String url = String("https://") + whoisApiHost + whoisApiPath + domain;

        Serial.println("\nWhois API çağrılıyor...");
        String json = httpGet(url);
        if (json.length() == 0)
        {
            Serial.println("Whois API cevabı alınamadı.");
        }
        else
        {
            StaticJsonDocument<2048> doc;
            DeserializationError err = deserializeJson(doc, json);
            if (err)
            {
                Serial.print("JSON parse hatası: ");
                Serial.println(err.c_str());
            }
            else
            {
                Serial.println("=== Whois Bilgileri ===");
                if (doc.containsKey("WhoisRecord"))
                {
                    auto record = doc["WhoisRecord"];
                    if (record.containsKey("registrant"))
                    {
                        auto registrant = record["registrant"];
                        Serial.printf("Kayıt Sahibi: %s\n", registrant["name"] | "Bilinmiyor");
                        Serial.printf("Organizasyon: %s\n", registrant["organization"] | "Bilinmiyor");
                        Serial.printf("Ülke: %s\n", registrant["country"] | "Bilinmiyor");
                    }
                    else
                    {
                        Serial.println("Kayıt sahibi bilgisi yok.");
                    }
                }
                else
                {
                    Serial.println("WhoisRecord bulunamadı.");
                }
            }
        }
    }

    Serial.println("\n--- Bilgi Toplama Sonlandı ---");
#else
    Serial.print("[SERTIFIKA KONTROL] bu fonksiyon kullanılmıyor");
#endif
}