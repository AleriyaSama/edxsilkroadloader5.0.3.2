# 🎮 Silkroad Online Packet Analyzer - Enhanced Edition

<div align="center">

![Version](https://img.shields.io/badge/version-5.0.3.2-blue)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)

**Gerçek zamanlı paket analizi için geliştirilmiş, kullanıcı dostu Silkroad Online paket yakalama aracı**

[Özellikler](#-özellikler) • [Kurulum](#-kurulum) • [Kullanım](#-kullanım) • [Örnek Çıktı](#-örnek-çıktı) • [Teknik Detaylar](#-teknik-detaylar)

</div>

---

## 📖 Hakkında

Bu proje, Silkroad Online MMORPG oyununun istemci-sunucu arasındaki ağ trafiğini gerçek zamanlı olarak yakalayıp analiz eden geliştirilmiş bir araçtır. Orijinal açık kaynak projeyi alıp **375+ opcode isim tanımı** ekleyerek paket analizini çok daha anlaşılır hale getirdik.

### 🎯 Ne Yapar?

- ✅ Oyun paketlerini gerçek zamanlı yakalar
- ✅ Her paketi **anlamlı isimlerle** gösterir (örn: `CHARACTER_MOVEMENT_REQUEST`)
- ✅ Hex dump formatında veri görüntüler
- ✅ Akıllı filtreleme ile istediğiniz paketleri izleyin
- ✅ Client→Server ve Server→Client yönlerini ayırın

### 🆚 Önceki Versiyondan Farklar

| Önceki Versiyon | Enhanced Edition |
|-----------------|------------------|
| `[Client->Server][7021]` | `[Client -> Server][0x7021 - CHARACTER_MOVEMENT_REQUEST]` |
| Sadece hex opcode | 375+ açıklayıcı isim |
| Boost kütüphanesi gerekli | Modern C++ standart kütüphane |
| MFC bağımlılığı | Windows SDK ile derlenir |
| Karmaşık kod yapısı | Okunabilir, yorumlu kod |

---

## ✨ Özellikler

### 🏷️ 375+ Opcode İsim Tanımları

Tüm paketler artık anlamlı isimlerle gösteriliyor:

```
[Server -> Client][0x0FF1 - GATEWAY_SERVER_READY]
[Server -> Client][0x1002 - GATEWAY_CAPTCHA_CHALLENGE]
[Client -> Server][0x1003 - GATEWAY_CAPTCHA_RESPONSE]
[Server -> Client][0x0FF3 - GATEWAY_CAPTCHA_ACCEPT]
[Client -> Server][0x6102 - GATEWAY_LOGIN_REQUEST]
[Server -> Client][0xA102 - GATEWAY_LOGIN_RESPONSE]
[Client -> Server][0x7021 - CHARACTER_MOVEMENT_REQUEST]
[Server -> Client][0x3015 - ENTITY_SPAWN]
[Client -> Server][0x7025 - CHAT_REQUEST]
[Server -> Client][0xB025 - CHAT_RESPONSE]
```

### 📂 Kategorize Edilmiş Opcode'lar

Tüm opcode'lar mantıksal kategorilere ayrılmış:

<details>
<summary><b>🌐 GLOBAL (13 opcode)</b></summary>

- El sıkışma (Handshake)
- Kimlik doğrulama
- Sertifikasyon
- Yönlendirme

</details>

<details>
<summary><b>🔐 GATEWAY/LOGIN (17 opcode)</b></summary>

- Sunucu hazırlık
- Captcha sistemi
- Patch kontrol
- Sunucu listesi
- Giriş doğrulama

</details>

<details>
<summary><b>👤 CHARACTER (23 opcode)</b></summary>

- Karakter seçimi
- Hareket (movement)
- Teleport
- Stat artırma
- Body/move state

</details>

<details>
<summary><b>🎭 ENTITY (22 opcode)</b></summary>

- Spawn/Despawn
- Pozisyon güncelleme
- Animasyon
- Cooldown yönetimi
- Durum güncellemeleri

</details>

<details>
<summary><b>⚔️ ACTION/COMBAT (10 opcode)</b></summary>

- Hedef seçme
- Saldırı komutları
- Action response'ları

</details>

<details>
<summary><b>🏰 GUILD (59 opcode)</b></summary>

- Lonca yönetimi
- Union sistemi
- Lonca savaşları
- Depo işlemleri
- Seçim sistemi

</details>

<details>
<summary><b>👥 PARTY (19 opcode)</b></summary>

- Grup oluşturma
- Davet sistemi
- Matching sistemi
- Dağıtım ayarları

</details>

<details>
<summary><b>🎒 INVENTORY (6 opcode)</b></summary>

- Eşya taşıma
- Kullanma
- Güncelleme

</details>

<details>
<summary><b>⭐ SKILL (8 opcode)</b></summary>

- Yetenek öğrenme
- Mastery sistemi
- Geri alma

</details>

<details>
<summary><b>💬 CHAT (3 opcode)</b></summary>

- Mesaj gönderme
- Mesaj alma
- Yanıt

</details>

**Ve daha fazlası:** Alchemy, Exchange, Stall, Quest, Community, COS (Pet), Academy, Consignment, Silk, Environment...

#### Ignore Opcodes (Kara Liste)
İstenmeyen paketleri gizleyin. Örneğin sürekli gelen `ENTITY_UPDATE` paketlerini filtreleyerek sadece önemli trafiği görün.

#### Show Only Opcodes (Beyaz Liste)
Sadece belirli opcode'ları gösterin. Örneğin sadece login işlemini analiz etmek için:
- `0x6102` (LOGIN_REQUEST)
- `0xA102` (LOGIN_RESPONSE)

#### Yön Filtresi
- ☑️ **Client→Server**: Oyuncunun gönderdiği paketler
- ☑️ **Server→Client**: Sunucunun gönderdiği paketler

### 📝 Dosya Loglama

Tüm paketler otomatik olarak dosyaya kaydedilir. Daha sonra analiz için kullanabilirsiniz.

### 🎨 Renkli ve Organize Görünüm

Paketler okunabilir hex dump formatında görüntülenir:

```
[Server -> Client][0x3015 - ENTITY_SPAWN]
A8 61                                    .a..............
9C 03                                    ................
E0 FF                                    ................
4B 05                                    K...............
```

---

## 🚀 Kurulum

### Gereksinimler

- Windows 7/8/10/11
- Visual Studio 2019 veya üzeri (derleme için)
- Silkroad Online istemcisi

### Derleme

1. **Projeyi klonlayın:**
```bash
git clone https://github.com/yourusername/edxSilkroadLoader5.git
cd edxSilkroadLoader5
```

2. **Visual Studio ile açın:**
```bash
src/edxSilkroadLoader5.sln
```

3. **Release modunda derleyin:**
   - Configuration: `Release`
   - Platform: `x86` (Silkroad 32-bit)

4. **Çıktı dosyaları:**
   - `edxSilkroadLoader5.exe` - Ana uygulama
   - `edxSilkroadDll5.dll` - Injection DLL

---

## 💻 Kullanım

### Temel Kullanım

1. **Analyzer'ı Başlatın**: Program otomatik olarak Silkroad'a enjekte edilir
2. **Paketleri İzleyin**: Gerçek zamanlı olarak tüm ağ trafiğini görün
3. **Filtreleme Yapın**: Sağ panel üzerinden istediğiniz filtreleri uygulayın

### Örnek Senaryolar

#### 📌 Senaryo 1: Login İşlemini Analiz Etme

**Show Only Opcodes'a ekleyin:**
- `6102` (LOGIN_REQUEST)
- `A102` (LOGIN_RESPONSE)
- `1002` (CAPTCHA_CHALLENGE)
- `1003` (CAPTCHA_RESPONSE)

Artık sadece giriş ile ilgili paketleri görürsünüz!

#### 📌 Senaryo 2: Karakter Hareketini İnceleme

**Show Only Opcodes'a ekleyin:**
- `7021` (CHARACTER_MOVEMENT_REQUEST)
- `B021` (ENTITY_UPDATE_MOVEMENT)
- `7023` (CHARACTER_FORWARD_REQUEST)

Sadece hareket paketlerini görürsünüz.

#### 📌 Senaryo 3: Chat Trafiğini Filtreleme

**Ignore Opcodes'a ekleyin:**
```
3015, 3016, B021, B023, B024, 304E, 3056
```
(Entity spawn/update paketleri)

Artık sadece önemli paketler görünür!

---

## 📸 Örnek Çıktı

### Giriş Akışı
```
[Server -> Client][0x0FF1 - GATEWAY_SERVER_READY]
01 00 00 00                                       ................

[Server -> Client][0xA106 - GATEWAY_SHARD_LIST_PING_RESPONSE]
01 14 1A 3A BE 2F                                 .:./............

[Server -> Client][0x1002 - GATEWAY_CAPTCHA_CHALLENGE]
00 00 00 00                                       ................
00 00 00 00                                       ................

[Client -> Server][0x1003 - GATEWAY_CAPTCHA_RESPONSE]
31 00                                             1...............

[Server -> Client][0x0FF3 - GATEWAY_CAPTCHA_ACCEPT]
01 00 00 00                                       ................

[Client -> Server][0x6102 - GATEWAY_LOGIN_REQUEST]
75 73 65 72 6E 61 6D 65                          username........
70 61 73 73 77 6F 72 64                          password........

[Server -> Client][0xA102 - GATEWAY_LOGIN_RESPONSE]
01 00                                             ................
```

### Oyun İçi Trafik
```
[Client -> Server][0x7021 - CHARACTER_MOVEMENT_REQUEST]
29 5D 01 00                                       )...............
A8 61                                             .a..............

[Server -> Client][0xB021 - ENTITY_UPDATE_MOVEMENT]
29 5D 01 00                                       )...............
A8 61                                             .a..............
9C 03                                             ................

[Server -> Client][0x3015 - ENTITY_SPAWN]
FF FF FF FF                                       ................
12 34 56 78                                       .4Vx............

[Client -> Server][0x7025 - CHAT_REQUEST]
01 48 65 6C 6C 6F                                 .Hello..........

[Server -> Client][0x3026 - CHAT_RECEIVE]
50 6C 61 79 65 72                                 Player..........
48 65 6C 6C 6F                                    Hello...........
```

---

## 🔧 Teknik Detaylar

### Mimari

```
┌─────────────────────────────────────┐
│  edxSilkroadLoader5.exe             │
│  (Ana Uygulama)                     │
│  - Silkroad başlatıcı               │
│  - DLL injection                    │
└──────────────┬──────────────────────┘
               │
               │ Inject
               ▼
┌─────────────────────────────────────┐
│  edxSilkroadDll5.dll                │
│  (Packet Interceptor)               │
│  - API hooking (Detours)            │
│  - Packet yakalama                  │
│  - Analyzer'a gönderme              │
└──────────────┬──────────────────────┘
               │
               │ IPC
               ▼
┌─────────────────────────────────────┐
│  Analyzer.cpp                       │
│  (Packet Viewer)                    │
│  - 375+ opcode tanımı               │
│  - Filtreleme mantığı               │
│  - GUI görüntüleme                  │
└─────────────────────────────────────┘
```

### Opcode Sistemi

Tüm opcode tanımları `Analyzer.cpp` içinde:

```cpp
std::map<WORD, std::string> opcodeNames;

void InitOpcodeNames() {
    // GLOBAL
    opcodeNames[0x2001] = "GLOBAL_IDENTIFICATION";
    opcodeNames[0x5000] = "GLOBAL_HANDSHAKE";
    opcodeNames[0x9000] = "GLOBAL_HANDSHAKE_ACCEPT";
    
    // GATEWAY
    opcodeNames[0x0FF1] = "GATEWAY_SERVER_READY";
    opcodeNames[0x6102] = "GATEWAY_LOGIN_REQUEST";
    opcodeNames[0xA102] = "GATEWAY_LOGIN_RESPONSE";
    
    // CHARACTER
    opcodeNames[0x7021] = "CHARACTER_MOVEMENT_REQUEST";
    opcodeNames[0xB021] = "ENTITY_UPDATE_MOVEMENT";
    
    // ... 370+ more opcodes
}
```

### Filtreleme Algoritması

```cpp
void Analyzer_SetOpcode(WORD opcode, bool s2c) {
    // 1. Yön kontrolü
    bool directionEnabled = s2c ? bShowS2C : bShowC2S;
    
    // 2. Beyaz liste kontrolü
    bool isInAllowList = (showOnlyOpcodes.empty()) || 
                         (showOnlyOpcodes.contains(opcode));
    
    // 3. Kara liste kontrolü
    bool isNotInIgnoreList = !filterOpcodes.contains(opcode);
    
    // 4. Öncelikli opcode
    bool isPriorityOpcode = showOnlyOpcodes.contains(opcode);
    
    // Gösterme kararı
    packet_show = (directionEnabled && isInAllowList && 
                   isNotInIgnoreList && !isPriorityOpcode) 
                   || isPriorityOpcode;
}
```

### Modern C++ Geçişi

**Boost Bağımlılığı Kaldırıldı:**
```cpp
// ❌ Eski
#include <boost/cstdint.hpp>
using boost::uint32_t;

// ✅ Yeni
#include <cstdint>
using std::uint32_t;
```

**MFC Bağımlılığı Kaldırıldı:**
```cpp
// ❌ Eski (Resource dosyalarında)
#include "afxres.h"

// ✅ Yeni
#include "winres.h"
```

---

## 📚 Opcode Referansları

```
├── GLOBAL/              # 13 opcode
├── GATEWAY/             # 17 opcode
├── DOWNLOAD/            # 3 opcode
└── AGENT/
    ├── CHARACTER/       # 23 opcode
    ├── ENTITY/          # 22 opcode
    ├── GUILD/           # 59 opcode
    ├── PARTY/           # 19 opcode
    ├── CHAT/            # 3 opcode
    ├── INVENTORY/       # 6 opcode
    ├── SKILL/           # 8 opcode
    ├── ACTION/          # 10 opcode
    ├── ALCHEMY/         # 5 opcode
    ├── EXCHANGE/        # 14 opcode
    ├── STALL/           # 16 opcode
    ├── QUEST/           # 7 opcode
    ├── COMMUNITY/       # 18 opcode
    ├── FORTRESS/        # 5 opcode
    ├── COS/             # 10 opcode
    ├── PK/              # 3 opcode
    ├── CONSIGNMENT/     # 19 opcode
    ├── ACADEMY/         # 24 opcode
    ├── SILK/            # 3 opcode
    ├── LOGOUT/          # 5 opcode
    ├── TAP/             # 5 opcode
    ├── GUIDE/           # 2 opcode
    ├── OPERATOR/        # 1 opcode
    └── ...
```

---

## 🎓 Kullanım Alanları

### 1. **Protokol Analizi**
Silkroad Online protokolünü anlamak ve dokümante etmek için.

### 2. **Bot Geliştirme**
Paket yapılarını öğrenerek otomasyon araçları geliştirin.

### 3. **Hata Ayıklama**
Sunucu-istemci iletişim sorunlarını tespit edin.

### 4. **Reverse Engineering**
Oyun mekanizmalarını anlayın.

### 5. **Eğitim**
Network protokolleri ve paket analizi öğrenin.

### 6. **Emulator Geliştirme**
Özel sunucu yazılımı geliştirenler için referans.

---

## 📝 Değişiklik Notları

### v5.0.3.2 - Enhanced Edition

#### ✨ Yeni Özellikler
- ✅ **375+ opcode isim tanımı** eklendi
- ✅ Tüm opcode'lar **0x prefix** ile gösteriliyor
- ✅ Kategorize edilmiş opcode yapısı
- ✅ Geliştirilmiş filtreleme mantığı
- ✅ Okunabilir kod yapısı (yorumlar eklendi)

#### 🔧 İyileştirmeler
- ✅ Boost bağımlılığı kaldırıldı (modern C++ stdlib kullanımı)
- ✅ MFC bağımlılığı kaldırıldı (Windows SDK ile derlenen)
- ✅ Değişken isimleri anlaşılır hale getirildi
- ✅ Kod yorumları Türkçe eklendi

#### 🐛 Düzeltmeler
- ✅ `uint32_t` tanımlama hataları düzeltildi
- ✅ `afxres.h` eksik başlık hatası çözüldü
- ✅ Derleme hataları giderildi

---

## ⚠️ Yasal Uyarı

Bu araç **yalnızca eğitim ve araştırma amaçlıdır**. 

**NOT:** Bu aracı kullanarak oyunun kurallarını ihlal etmekten kaynaklanan tüm sorumluluk kullanıcıya aittir.

---

## 🤝 Katkıda Bulunma

Katkılarınızı bekliyoruz! 

1. Fork yapın
2. Feature branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add amazing feature'`)
4. Branch'inizi push edin (`git push origin feature/amazing-feature`)
5. Pull Request açın

### Katkıda Bulunabileceğiniz Alanlar

- 🆕 Yeni opcode tanımları
- 🐛 Bug düzeltmeleri
- 📝 Dokümantasyon iyileştirmeleri
- 🎨 UI/UX geliştirmeleri
- 🌐 Çoklu dil desteği

---

## 🙏 Teşekkürler

- Orijinal **edxSilkroadLoader** projesi geliştiricilerine
- Silkroad Online reverse engineering topluluğuna
- Opcode dokümantasyonu hazırlayan tüm katkıda bulunanlara

---

## 📄 Lisans

Bu proje eğitim amaçlı açık kaynak olarak paylaşılmıştır.

---

<div align="center">

**⭐ Projeyi beğendiyseniz yıldız vermeyi unutmayın! ⭐**

Made with ❤️ for the Silkroad Community

</div>







