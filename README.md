# ESP8266 WiFi Kütüphanesi

Bu kütüphane, ESP8266 NodeMCU kartlarınız için kablosuz ağ yönetimi, internetten veri çekme ve MQTT sunucu iletişimi gibi ağ işlerini kolaylaştıran kullanışlı bir pakettir

## Kurulum 

1. Kütüphaneyi projenize ekleyin: Arduino IDE kullanıyorsanız, `AG_LIB.h`, `ag_bilgiler.cpp`, `baglantilar.cpp`, `httpget_1.cpp`, `httpget_2.cpp`, `httpget_3.cpp`, `Sserver__1.cpp` dosyalarını projenizin klasörüne taşıyın.

2. Gerekli harici kütüphaneleri (ESP8266WiFi, DNSServer, ESP8266HTTPClient, WiFiClientSecure, ESPAsyncTCP, WiFiUdp, NTPClient, ArduinoJson, PubSubClient) Arduino IDE Kütüphane Yöneticisi'nden yükleyin.

## Nasıl Kullanılır?

`Ag` sınıfı, `web`, `ogren`, `baglanti` ve `server` adlı dört ana bölümle kütüphanenin tüm özelliklerini sunar. Her bölümün kendi görevleri var, hadi inceleyelim!

### `Ag::web` Bölümü: İnternet İşleri

`Ag::web` kısmı, internetten veri çekme ve bu verileri işleme konularında size yardımcı oluyor.

* **`void zaman_al(int *sn, int *dk, int *saat, int *gun, int *ay, int *yil, int *haftagunu, unsigned long timeout_ms = 8000, long gmtOffset = 10800)`**

  * **Açıklama:** İnternetten güncel tarih ve saat bilgilerini alıp belirlediğiniz yerlere kaydeder. NTP sunucusuyla zamanı senkronize eder.

  * **Parametreler:** `sn`, `dk`, `saat`, `gun`, `ay`, `yil`, `haftagunu` (işaretçiler), `timeout_ms` (varsayılan 8000), `gmtOffset` (varsayılan 10800).

  * **Dönüş Değeri:** Yok.

* **`String json_veri_string(const String &json, const String &path)`**

  * **Açıklama:** JSON metninden, iç içe anahtar yolları (örn. `"user.name"`) ile string değer çeker.

  * **Parametreler:** `json` (String), `path` (String).

  * **Dönüş Değeri:** String veya boş string (`""`).

* **`int json_veri_int(const String &json, const String &path)`**

  * **Açıklama:** JSON metninden, iç içe anahtar yolları (örn. `"user.age"`) ile tam sayı (`int`) değer çeker.

  * **Parametreler:** `json` (String), `path` (String).

  * **Dönüş Değeri:** `int` veya `0`.

* **`float json_veri_float(const String &json, const String &path)`**

  * **Açıklama:** JSON metninden, iç içe anahtar yolları (örn. `"user.height"`) ile ondalık sayı (`float`) değer çeker.

  * **Parametreler:** `json` (String), `path` (String).

  * **Dönüş Değeri:** `float` veya `0.0f`.

* **`String json_al(const String &url, uint32_t timeout_ms = 5000)`**

  * **Açıklama:** Belirtilen URL'den (HTTP/HTTPS) veri çeker. WiFi bağlantısını kontrol eder ve zaman aşımı uygular.

  * **Parametreler:** `url` (String), `timeout_ms` (varsayılan 5000).

  * **Dönüş Değeri:** Gelen veri (String) veya boş string.

* **`void siteGuvenlikBilgileriniAl(const char *domain)`**

  * **Açıklama:** (Kullanım dışı/Geliştirme aşamasında) Belirtilen alan adı için sertifika, HTTP başlıkları, SSL Labs ve Whois bilgilerini almaya çalışır.

  * **Parametreler:** `domain` (karakter dizisi).

  * **Dönüş Değeri:** Yok.

* **`String json_al_detayli(const String &baseUrl, const String &apiKey, const String &il, const String &ilce = "", uint32_t timeout_ms = 5000)`**

  * **Açıklama:** Belirlenen API'den (örn. OpenWeatherMap), şehir ve isteğe bağlı ilçe bilgileriyle JSON veri çeker.

  * **Parametreler:** `baseUrl` (String), `apiKey` (String), `il` (String), `ilce` (String, varsayılan boş), `timeout_ms` (varsayılan 5000).

  * **Dönüş Değeri:** JSON verisi (String) veya boş string.

### `Ag::ogren` Bölümü: Ağ Bilgileri

`Ag::ogren` kısmı, ESP8266 cihazınızın bağlı olduğu veya etrafındaki WiFi ağları hakkında bilgi toplamanıza yardımcı oluyor.

* **`uint8_t sinyal_gucu()`**

  * **Açıklama:** WiFi sinyal gücünü 0-3 arası seviyede döndürür.

    * `3`: Çok Güçlü (`>= -50 dBm`)

    * `2`: Orta (`>= -65 dBm`)

    * `1`: Zayıf (`>= -75 dBm`)

    * `0`: Çok zayıf veya bağlantı yok.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Sinyal seviyesi (`uint8_t`).

* **`void durum()`**

  * **Açıklama:** WiFi bağlantı durumunu (örn. `WL_CONNECTED`) seri monitöre yazdırır.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.

* **`String ip_adresi()`**

  * **Açıklama:** Bağlı ağın IP adresini döndürür.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** IP adresi (String) veya boş string.

* **`String ag_adi()`**

  * **Açıklama:** Bağlı WiFi ağının adını (SSID) döndürür.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** SSID (String) veya boş string.

* **`String mac_adresi()`**

  * **Açıklama:** Cihazın MAC adresini döndürür.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** MAC adresi (String).

* **`String cevredeki_aglar(uint8_t saniye = 255, uint8_t dakika = 255, uint8_t saat = 255, uint8_t gun = 255, uint8_t ay = 255, uint16_t yil = 255)`**

  * **Açıklama:** Çevredeki WiFi ağlarını tarar, sinyal gücüne göre sıralar ve tablo halinde sunar. Başlıkta toplam ağ sayısı, en güçlü sinyal ve tarih/saat bilgisi bulunur.

  * **Parametreler:** `saniye`, `dakika`, `saat`, `gun`, `ay`, `yil` (varsayılan 255).

  * **Dönüş Değeri:** Düzenlenmiş WiFi ağı listesi (String).

### `Ag::baglanti` Bölümü: Bağlantı Yönetimi

`Ag::baglanti` kısmı, WiFi bağlantınızı kurmak, kesmek ve yönetmek için harika fonksiyonlar sunuyor.

* **`bool kur(const char *ssid, const char *sifre)`**

  * **Açıklama:** Belirtilen SSID ve şifre ile WiFi ağına bağlanır.

  * **Parametreler:** `ssid` (karakter dizisi), `sifre` (karakter dizisi).

  * **Dönüş Değeri:** `true` (başarılı) veya `false` (başarısız).

* **`void kes()`**

  * **Açıklama:** Mevcut WiFi bağlantısını sonlandırır.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.

* **`bool sorgu()`**

  * **Açıklama:** WiFi bağlantı durumunu kontrol eder.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** `true` (bağlı) veya `false` (bağlı değil).

* **`void bilgi()`**

  * **Açıklama:** Mevcut bağlantı bilgilerini seri monitöre yazdırır.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.

* **`void tekrar_dene(const char *ssid, const char *sifre, uint8_t deneme_sayisi)`**

  * **Açıklama:** Belirtilen ağ bilgileriyle tekrar bağlanmayı dener (`delay` kullanır).

  * **Parametreler:** `ssid` (karakter dizisi), `sifre` (karakter dizisi), `deneme_sayisi`.

  * **Dönüş Değeri:** Yok.

* **`bool eski_aga_otomatik()`**

  * **Açıklama:** Bilinen ağlara otomatik bağlanmayı dener.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** `true` (başarılı) veya `false` (başarısız).

* **`bool statik_ip_kur(const char *ssid, const char *sifre, IPAddress ip, IPAddress gateway, IPAddress subnet)`**

  * **Açıklama:** Statik IP adresi kullanarak belirlenen ağa bağlanır.

  * **Parametreler:** `ssid` (karakter dizisi), `sifre` (karakter dizisi), `ip` (`IPAddress`), `gateway` (`IPAddress`), `subnet` (`IPAddress`).

  * **Dönüş Değeri:** `true` (başarılı) veya `false` (başarısız).

* **`bool kayitli_aglardan(const String &ssid_list, const String &sifre_list)`**

  * **Açıklama:** Çevredeki WiFi ağlarını tarar, sağlanan SSID ve şifre listeleriyle eşleştirip bağlanmayı dener. Listelerdeki sıra önemlidir!

  * **Parametreler:** `ssid_list` (String), `sifre_list` (String).

  * **Dönüş Değeri:** `true` (başarılı) veya `false` (başarısız).

### `Ag::server` Sınıfı: MQTT Sunucu İşleri

`Ag::server` sınıfı, MQTT bulut sunucularına (mesela HiveMQ Cloud) güvenli bir şekilde bağlanmanızı, mesajlara abone olmanızı, veri göndermenizi ve almanızı sağlıyor.

* **`bool baglan()`**

  * **Açıklama:** Ayarlar sonrası MQTT sunucusuna bağlanır. Sertifika kontrolü test için kapalıdır.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** `true` (başarılı) veya `false` (başarısız).

* **`void baglantiyi_sonlandir()`**

  * **Açıklama:** MQTT sunucu bağlantısını keser.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.

* **`void abone(const char *topic, bool islem)`**

  * **Açıklama:** Belirtilen MQTT başlığına abone olur veya aboneliği iptal eder.

  * **Parametreler:** `topic` (karakter dizisi), `islem` (`bool`).

  * **Dönüş Değeri:** Yok.

* **`void veriGonder(const char *topic, const char *mesaj)`**

  * **Açıklama:** Belirtilen MQTT başlığına veri gönderir.

  * **Parametreler:** `topic` (karakter dizisi), `mesaj` (karakter dizisi).

  * **Dönüş Değeri:** Yok.

* **`void veri_al()`**

  * **Açıklama:** `setup`'ta çağrılması gereken bu fonksiyon, gelen abonelik verilerini işleyen bir callback ayarlar.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.

* **`String veri_sade()`**

  * **Açıklama:** Alınan MQTT verilerini `topic;Veri\n` formatında döndürür.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Alınan veriler (String).

* **`void veri_temizle()`**

  * **Açıklama:** Alınan veri paketini temizler. Bellek taşmasını önlemek için düzenli temizlik önemlidir!

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.

* **`static void aktif_tut()`**

  * **Açıklama:** `loop()` içinde kullanılması gereken bu fonksiyon, MQTT istemcisini aktif tutar ve mesaj alışverişini yönetir.

  * **Parametreler:** Yok.

  * **Dönüş Değeri:** Yok.
