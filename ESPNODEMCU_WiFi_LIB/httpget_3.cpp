#include "AG_LIB.h"

// Basit URL encode (yalnızca boşlukları %20 yapar)
String urlEncode(const String &str)
{
    String encoded = "";
    for (char c : str)
    {
        if (c == ' ')
            encoded += "%20";
        else
            encoded += c;
    }
    return encoded;
}

/**
 * @brief API'den JSON veri çek
 * @param baseUrl API URL, örn: http://api.openweathermap.org/data/2.5/weather
 * @param apiKey OpenWeatherMap API anahtarı
 * @param il Şehir ismi
 * @param ilce İlçe ismi (opsiyonel, boş bırakılabilir)
 * @param timeout_ms HTTP isteği için zaman aşımı (ms)
 * @return JSON veri String olarak döner, hata olursa boş String
 */
String Ag::web::json_al_detayli(const String &baseUrl, const String &apiKey, const String &il, const String &ilce, uint32_t timeout_ms)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("WiFi bağlı değil!"));
        return "";
    }

    String query = urlEncode(il);
    if (ilce.length() > 0)
    {
        query += "," + urlEncode(ilce);
    }

    String tamUrl = baseUrl + "?q=" + query + "&units=metric&appid=" + apiKey;

    HTTPClient http;
    http.setTimeout(timeout_ms);
    Serial.print(F("[HTTP] Bağlanıyor: "));
    Serial.println(tamUrl);

    if (!http.begin(secureClient,tamUrl))
    {
        Serial.println(F("[HTTP] Bağlantı başlatılamadı!"));
        return "";
    }

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println(F("[HTTP] Veri alındı:"));
        Serial.println(payload);
        http.end();
        return payload;
    }
    else
    {
        Serial.print(F("[HTTP] Hata kodu: "));
        Serial.println(httpCode);
        Serial.print(F("[HTTP] Hata mesajı: "));
        Serial.println(http.errorToString(httpCode));
        http.end();
        return "";
    }
}