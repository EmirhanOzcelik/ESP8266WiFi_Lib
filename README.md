# esp8266 iot kütüphanesi

bu proje ek bağımlılıklar gerektirir. Lütfen ortamınızda library.json dosyanızın kullanıldığından emin olun.

# içindekiler

1. [hakkında](#açıklama)
1. [kod örnekleri](#örnekler)
   * [ağa bağlanmak ve tarih bilgisi almak](#ağa-bağlanıp-ag-bilgilerini-almak-ve-tarih-çekmek)
   * [iot projelere giriş](#mqtt-server-ile-iot-proje)

## açıklama

- **bu kütüphane ile neler yapabilirsiniz?**
  - _kolayca wifi bağlantısı kurabilir yönetebilir ve durumu izleyebilirsiniz._
  - _ntp ile saat verisi çekebilir herhangi başka sitelerden json verileri çekip veri alabilirsiniz._
  - _mqtt ile kolayca proje kurabilir kanal yönetimi ve mesaj okuma-yazma yapabilirsiniz._

# örnekler

## ağa bağlanıp ag bilgilerini almak ve tarih çekmek

- en temel özelliktir.

- seri ekranda durumu anlık izleyebilirsiniz.

```cpp
#include "AG_LIB.h"

Ag esp; // nesne tanımı

const char *ssid = "wifi agınız ismi";
const char *sifre = "wifi agınız şifresi";

uint8_t saat = 0, dakika = 0, saniye = 0, ay = 0, ay_gun = 0, hafta_gun = 0;
uint16_t yil = 1970;

void setup()
{
    Serial.begin(115200);
    if(esp.baglanti.kur(ssid,sifre))
        // internetle alakalı işlemleriniz
    else
        esp.baglanti.tekrar_dene(ssid,sifre,5); // 5 kez tekrar bağlanmaya çalışsın

    if(esp.baglanti.sorgu()) // kart internete bağlı mı?
    {
        // internetle alakalı işlemleriniz
        // bu örnekte internet bilgilerini yazdıralım ve ntp ile tarih alalım

        esp.ogren.ag_adi();         //  ağ ismini yazdırır
        esp.ogren.ip_adresi();      //  kartın ip adresini yazdırır
        esp.ogren.mac_adresi();     //  kartın mac adresini yazdırır
        esp.ogren.sinyal_gucu();    // kartın sinyal gücünü yazdırır

        // bu fonksiyon değişkenlerinizi güncel tarih bilgileri ile günceller.
        ag.web.zaman_al(&saniye, &dakika, &saat, &ay_gun, &ay, &yil, &hafta_gun);
    }
    else
        Serial.println(F("wifi baglantisi yok"));

}

void loop()
{

}
```

## mqtt server ile iot proje

- mqtt internetten güvenli şekilde veri alıp gönderebileceğiniz çok basit hızlı ve modüler bir sistemdir.

- bu kütüphane sensör zamanlama vs. gibi bilgileri rahatça aktarabilmenizi amaçlar.

```cpp
#include "AG_LIB.h"

Ag esp; // nesne tanımı

const char *ssid = "wifi agınız ismi";
const char *sifre = "wifi agınız şifresi";

void setup()
{
    Serial.begin(115200);
    if(esp.baglanti.kur(ssid,sifre))
        // internetle alakalı işlemleriniz
    else
        esp.baglanti.tekrar_dene(ssid,sifre,5); // 5 kez tekrar bağlanmaya çalışsın

    if(esp.baglanti.sorgu()) // kart internete bağlı mı?
    {
        // proje urlnizi gireceksiniz
        esp.server.ayarla_url("*****************...*****.hivemq.cloud");
        // proje portu (varsayılan 8883 çoğu zaman kullanmanıza gerek yok)
        esp.server.ayarla_port(8883);
        // projede oluşturduğunuz kullanıcılardan herhangi birinin kullanıcı ismi
        esp.server.ayarla_KullaniciAdi("user1");
        // projede oluşturduğunuz kullanıcılardan herhangi birinin şifresi
        esp.server.ayarla_KullaniciSifre("sifre");
        // tüm bilgiler hazır olduktan sonra bağlan komutu ile baglanıyoruz
        esp.server.baglan();
    }
    else
        Serial.println(F("wifi baglantisi yok"));

    // bunu setup kısmına yazmalısınız
    esp.server.veri_al();
}

void loop()
{

    // bunu loop kısmına yazmalısınız
    esp.server.aktif_tut();

    // seri haberleşme ile kontrol ünitesi
    if(Serial.available()>0)
    {
        char veri = Serial.read();
        switch (veri)
        {
            case ('F'):
                // test adında bir kanal başlatıyoruz ve abone oluyoruz
                esp.server.abone("test", 1);
            break;
            case ('S'):
                // test kanalı açıksa kapıyoruz ve abonelikten çıkıyoruz
                esp.server.abone("test", 0);
            break;
            case ('M'):
                // test kanalında mesaj yolluyoruz (abone olmamız gerek)
                esp.server.veriGonder("test", "merhaba dunya");
            break;
        }
    }
}
```
