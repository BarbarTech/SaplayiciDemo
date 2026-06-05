# SaplayiciDemo

Windows Filtering Platform (WFP) kullanarak uygulama bazlı internet erişimini çekirdek seviyesinde engelleyen minimal bir KMDF sürücüsü.

---

## Özellikler

- Uygulama bazlı internet engelleme (AppID ile)
- IPv4 ve IPv6 Connect katmanı desteği
- IOCTL üzerinden başlatma / durdurma
- Maksimum 1024 eş zamanlı filtre

---

## Gereksinimler

| Gereksinim | Sürüm |
|---|---|
| Windows | 10 / 11 (x64) |
| Visual Studio | 2022 |
| Windows Driver Kit (WDK) | 10.0.26100.6584 |
| Windows SDK | 10.0.26100 |

---

## Projeyi Sıfırdan Kurma

### 1. Visual Studio'da Yeni KMDF Projesi Oluştur

1. Visual Studio 2022'yi **Yönetici olarak** aç
2. `Dosya → Yeni → Proje`
3. Arama kutusuna **"Kernel Mode Driver Empty (KMDF)"** yaz ve seç
4. `Main.c` dosyasını projeye ekle

> **Şablon listede gözükmüyorsa:** Visual Studio Installer'ı aç, **"C++ ile masaüstü geliştirme"** iş yükünü ve Bağımsız Bileşenler sekmesinden **"MSVC v143 - VS 2022 C++ x64/x86 Spectre risk azaltılmış kitaplıklar (En yeni)"** ile **"Windows Sürücü Kiti"** bileşenlerini yükle.

---

### 2. NuGet Paketlerini Yükle

`Proje → NuGet Paketlerini Yönet → Gözat` aç ve şu paketi indir:

```
Microsoft.Windows.WDK.x64
-Version 10.0.26100.6584
```

---

### 3. Proje Ayarlarını Yapılandır

Proje üzerine sağ tıkla → **Özellikler** → yapılandırmayı `Release | x64` ya da `Tüm Yapılandırmalar` olarak ayarla.

#### Bağlayıcı → Giriş → Ek Bağımlılıklar

Aşağıdakilerin hepsini tek satır olarak boşluksuz ekle:

```
%(AdditionalDependencies);$(KernelBufferOverflowLib);$(DDK_LIB_PATH)ntoskrnl.lib;$(DDK_LIB_PATH)hal.lib;$(DDK_LIB_PATH)wmilib.lib;$(KMDF_LIB_PATH)$(KMDF_VER_PATH)\WdfLdr.lib;$(KMDF_LIB_PATH)$(KMDF_VER_PATH)\WdfDriverEntry.lib;fwpkclnt.lib;ntstrsafe.lib
```

#### C/C++ → Genel → Uyarıları Hata Olarak Ele Al

`Hayır` olarak ayarla.

#### Inf2Cat → Yerel Saati Kullan

`Evet` olarak ayarla. (isteğe bağlı)

---

### 4. Derle

Yapılandırmayı `Release | x64` ya da `Debug | x64` olarak seç ve `Çözümü Derle` ile derle.

Çıktı: `x64/Release/` ya da `x64/Debug/` klasöründe `.sys`, `.inf` ve `.cat` dosyaları oluşur.

---

## Sürücüyü Yükleme

### Test İmzalama Modunu Aç

**Yönetici** olarak CMD'yi aç:

```cmd
bcdedit /set testsigning on
```

Bilgisayarı yeniden başlat. Sağ alt köşede "Test Modu" yazısı görünüyorsa hazırsın.

#### Güvenli Önyükleme (Secure Boot) Hatası

Test İmzalama Modu bazı sistemlerde çalışmaz çünkü Secure Boot aktif olabilir. Bu durumda BIOS/UEFI üzerinden Secure Boot kapatılmalıdır.

#### Tek Seferlik Test (Geçici Yöntem)

Sistemi kalıcı olarak değiştirmek istemeyenler için:

1. Shift tuşuna basılı tutarak **Yeniden Başlat** seçeneğini seç
2. Sorun Gider → Gelişmiş Seçenekler → Başlangıç Ayarları
3. **"Sürücü imza zorlamasını devre dışı bırak"** seçeneğini seç

> Bu yöntem yalnızca o oturum için geçerlidir.

---

### Sürücüyü Yükle

Derleme sonrası elde edilen `.sys` dosyasının tam yolunu kopyala ve CMD'yi **Yönetici** olarak çalıştır:

```cmd
sc create SaplayiciDemo binPath= "C:\tam\yol\SaplayiciDemo.sys" type= kernel start= demand
```

Mevcut durum için:

```cmd
sc query SaplayiciDemo
```

Başlatmak için:

```cmd
sc start SaplayiciDemo
```

Durdurmak için:

```cmd
sc stop SaplayiciDemo
```

Kaldırmak için:

```cmd
sc delete SaplayiciDemo
```

---

## Exe ile Kullanım

Sürücü yüklü ve çalışır durumdayken `BulucuV3.exe` uygulamasını **Yönetici olarak** çalıştır.

Uygulama, `\\Device\\SaplayiciDevice` sembolik bağlantısı üzerinden sürücüyle haberleşir:

- Listeden bir uygulama seçip **başlat** dediğinde, seçilen uygulamanın AppID bilgisi sürücüye gönderilir ve o uygulama için IPv4/IPv6 filtreleri aktif edilir. Seçilen uygulama internete erişemez hale gelir.
- **Durdur** dediğinde sürücüye durdurma sinyali gönderilir, tüm filtreler kaldırılır ve engelleme sona erer.

---

## Notlar

- Test İmzalama Modu yalnızca geliştirme amaçlıdır. Üretim ortamı için imzalı sürücü önerilir.
- Sürücü kaldırılmadan sistem kapatılırsa filtreler otomatik temizlenir (`UnloadDriver` ile).
- Maksimum 1024 uygulama filtresi desteklenir (`MAX_FILTERS`).

---

## Lisans

Bu proje MIT lisansı ile lisanslanmıştır.
