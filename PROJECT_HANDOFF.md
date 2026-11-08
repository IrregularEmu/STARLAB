# STARLAB — teknik devir reçetesi

Bu belge projeye yeni devam edecek geliştirici için tek başlangıç noktasıdır. Güncel kaynak dizini:

`/home/ayberk/Projects/B4`

Eski hesapların yetkili referansı:

`/home/ayberk/Projects/B3/roketsimdeneme1`

## 1. Ürünün amacı ve değişmez kararlar

STARLAB, OpenRocket benzeri fakat kendi hesap motorumuzu kullanan masaüstü odaklı bir roket tasarım ve uçuş simülasyonu uygulamasıdır. OpenRocket hesap motoru ürüne gömülmeyecek; yalnız davranış/UX karşılaştırma referansı olabilir. Fizik denklemlerinin mevcut kaynağı B3 içindeki eski RoketSiM projesidir. B3 kodunun mimarisi kopyalanmaz; yalnız hesap matematiği ve varsayımları korunarak yeni node mimarisine ayrılır.

Ana ilke: kullanıcı bir girdiyi değiştirdiğinde yalnız o girdiye bağlı nodelar ve onların torunları yeniden hesaplanır. Arayüz hesap yapmaz. Arayüz yalnız proje verisi toplar, SI birimine dönüştürür, hesap isteği üretir ve sonucu gösterir.

Şimdilik B3'te bulunmayan fizik uydurulmaz. Eksik model açıkça “desteklenmiyor” diye engellenir. Sessiz varsayım, sessiz parça atlama ve sahte sonuç yasaktır.

## 2. Genel veri akışı

```text
React UI / .star proje verisi
        ↓
src/engine/*Inputs.ts adaptörleri (mm→m, g→kg, seçim/konfigürasyon)
        ↓
src/engine/protocol.ts EngineRequest
        ↓ HTTP NDJSON
server/engine.ts (UI'dan ayrı, düşük öncelikli Python child process)
        ↓ JSONL stdin/stdout
engine/worker.py
        ↓
engine/models/*.py + engine/scenarios/*.py
        ↓
engine/graph.py bağımlılık DAG'si ve incremental cache
        ↓ ctypes
native/**/*.cpp derlenmiş paylaşımlı kütüphane
        ↓
EngineResponse → hook/workspace → sadece sunum
```

`server/engine.ts` hesaplamayı ayrı Python sürecinde çalıştırır. Bu yüzden ağır hesap UI thread'ini dondurmaz. Her session'ın ayrı worker/cache'i vardır. Yeni istek aynı session'daki eski işi iptal edebilir; timeout ve process çökmesi arayüzü çökertmez. Tarayıcı `src/engine/client.ts` üzerinden `/api/calculations/structure` endpoint'ine bağlanır.

Python burada ağır matematik yazmak için değil, graph orkestrasyonu ve C++ FFI katmanı için kullanılır. Asıl sayısal fonksiyonlar `native/` altındaki saf C++ dosyalarıdır. C++ değişince `npm run build:engine` ile paylaşımlı kütüphane yeniden derlenir. Uygulama güncellemesinde bu build artifact paketlenir; son kullanıcı kaynak derlemez.

## 3. Hesap node sistemi

Temel sözleşmeler `engine/contracts.py` içindedir:

- `Port`: değer, birim, frame ve validasyon sözleşmesi.
- `Source`: dışarıdan gelen girdi.
- `Binding`: node girdisinin hangi source/node çıktısına bağlandığı.
- `Node`: kimlik, sürüm, schema sürümü, girdiler, çıktı, implementation ve provenance.
- `Model`: belirli bir hesap ailesinin source/node listesi.
- `Scenario`: birden fazla modeli tek dinamik DAG içinde bağlayan kompozisyon.

`engine/graph.py` topolojik sıralama yapar, cycle ve contract uyuşmazlığını reddeder. Her node sonucunda `ready`, `missing`, `blocked` veya `error` state'i vardır. Bir source değişince dirty zinciri yalnız bağımlı nodelara yayılır. Değişmeyen node `cached` kalır. Cache yalnız tüm evaluation tamamlanınca atomik değiştirilir.

Node fingerprint'i `engine/worker.py` tarafından şu bilgilerden türetilir: node sürümü, schema sürümü, binding'ler, native C++ modül hash'i, Python node dosyası, model ve ortak runtime dosyaları. Dolayısıyla tek bir C++ hesabı değişince ilgili node ve torunları invalid olur; bağımsız hesaplar cache'de kalır.

Her küçük hesap ayrı dosyadır. Örnek:

- C++: `native/nose/mass.cpp`
- Python node: `engine/nose/mass.py`
- Model kaydı: `engine/models/nose.py`
- Proje adaptörü: `src/engine/structuralInputs.ts`
- Test: `engine/tests/test_design.py`

Dinamik tüm-roket bağları:

- `engine/assembly.py`: parça modellerini ID prefix'leriyle birleştirir; toplam kütle, ilk moment, CG ve paralel eksen ataleti.
- `engine/aero_assembly.py`: parça CNa/CP sonuçlarını `Σ(CNa·CP)/ΣCNa` ile birleştirir.
- `engine/coupling.py`: model çıkışlarını başka model source'larına tip güvenli bağlar.
- `engine/scenarios/design.py`: structure + static aero ağını yalnız son CG/CP/marj nodelarında birleştirir.
- `engine/scenarios/flight.py`: yapı toplamını tek kademeli axial RK4 uçuş modeline bağlar.

## 4. B3 eski proje nasıl kullanılıyor

B3 kaynak kodu doğrudan runtime dependency değildir. Oradaki üretim hesabı bulunur, davranışı değişmeden küçük saf C++ fonksiyonlarına ayrılır. Her node'un `reference` alanı eski kaynağı belirtir. `native/source-provenance.json` kaynak parmak izlerini tutar.

Karşılaştırma scriptleri:

- `scripts/check-legacy-design.py`
- `scripts/check-legacy-structure.mjs`
- `scripts/check-legacy-assembly.py`
- `scripts/check-legacy-motor.py`
- `scripts/check-legacy-installed-motor.py`
- `scripts/check-legacy-aero.py`
- `scripts/check-legacy-environment.py`
- `scripts/check-legacy-parity.mjs`

Bu scriptler eski fonksiyonları bağımsız derleyip yeni native çıktılarla yüzlerce rastgele vakada karşılaştırır. “Test geçti” yalnız yeni kodun kendi kendisiyle tutarlı olması değildir; mümkün olan her model eski kaynakla sayısal parity görmelidir. Kaynakta garip ama bilinçli davranış varsa düzeltmeden korunur ve test/not ile açıklanır.

## 5. Yeni hesap ekleme reçetesi

1. Önce B3 içinde gerçek üretim yolunu bul. Aynı isimli eski/sınıf tabanlı fakat kullanılmayan fonksiyona yanlışlıkla bağlanma. CLI/runtime yolunu izle.
2. Girdi, çıktı, birim, koordinat frame'i, sınır ve varsayımları yaz. Hesap B3'te yoksa uygulama; kullanıcıya bildir.
3. Tek sorumluluklu saf fonksiyonu `native/<alan>/<hesap>.cpp` içine koy. Export biçimi mevcut komşu dosyalardan kopyalanır; UI/JSON bilgisi C++'a girmez.
4. `engine/<alan>/<hesap>.py` içinde `Node` oluştur. Stabil bir node ID, açık `version`, `implementation` yolu ve B3 `reference` bilgisi ver. Formül değişirse node version artır.
5. Portları `engine/<alan>/__init__.py` içinde tanımla; birimleri ve frame'leri gevşetme.
6. Node'u `engine/models/<model>.py` modeline ekle. Birden çok model bağlanacaksa mevcut scenario/coupling yapısını genişlet; worker içine özel-case yazma.
7. Arayüz proje verisini değiştirmeden `src/engine/*Inputs.ts` adaptöründe SI request'e projekte et. Domain verisi ile engine request'i ayrı kalsın.
8. Sonucu bir hook/workspace üzerinden göster. React render içinde fizik formülü tekrar etme.
9. B3 parity testi, node dependency/cache testi, invalid input/recovery testi ve TS adaptör testi ekle.
10. `npm run build:engine`, ilgili Python/Vitest testlerini çalıştır.
11. `python3 scripts/update-calcs.py` ve ardından `python3 scripts/update-calcs.py --check` çalıştır. Yerel `CALCS` her node'un formül, dosya, girdi/çıktı ve bağlantı indeksidir.
12. Küçük, kısa, lowercase English commit at. Örnek: `add tube fin stability`.

Bir hesap değişikliğinde tüm modeli yeniden yazma. Yeni yetenek normalde yeni node + gerekli binding + adaptör alanı olmalıdır.

## 6. Şu anda hazır hesaplar

Yapı:

- Gövde/iç tüp/annular elemanlar: hacim, kütle, CG, eksenel ve enine atalet.
- Burun: altı profil, kabuk/dolu yapı, omuz ve kapak.
- Trapez kanat: alan, hacim, kütle, CG, boyutlar ve atalet.
- Transition: kesik koni integrasyonu ile kütle/CG/atalet.
- Ray button: çoklu örnek, ayrım kaynaklı CG/atalet.
- Paraşüt/kordon paketli kütle modeli; eski kaynağın sıfır atalet kabulü korunur.
- Tüm roket assembly: toplam kütle, CG ve atalet; component mass/CG override.

Motor:

- `.eng` itki eğrisi, enterpolasyon, impuls, toplam impuls, kalan yakıt, kuru/ıslak kütle.
- Motor yuvasına konumlandırma, ignition delay, motor CG ve eski runtime atalet katkıları.
- Aktif uçuş konfigürasyonu `.star` içinde tutulur.

Aerodinamik/çevre:

- Girilen tüm-roket CNa, CP, axial Cd üzerinden kuvvet/moment ağı.
- Burun, trapez kanat ve transition için geometriden statik CNa/CP.
- Toplam CP, static margin ve tasarım canvasında canlı CG/CP işaretleri.
- Surface ve US1976 atmosferi; sıcaklık, basınç, yoğunluk, ses hızı, viskozite, Mach, Reynolds, q ve değişken g.
- Sabit/power-law/log rüzgâr, meteorolojik yön dönüşümü ve ray düzlemine izdüşüm.

Uçuş:

- Tek kademeli azaltılmış 2B axial noktasal kütle RK4.
- İtki, zamana bağlı motor kütlesi, atmosfer, rüzgâr, sürükleme, pad desteği ve ray kısıtı bağlıdır.
- Çıktı: t, AGL irtifa, hız, ivme, downrange, vx, vz ve rail-cleared.
- Uçuş sayfasında grafik, metrikler ve gerçek konumdan oynatılan 2D yörünge vardır.

## 7. Bilinen sınırlar — yanlış iddia etme

- Uçuş çözücüsü henüz gerçek 3DoF/6DoF değildir. Gövde ekseni rayın ilk doğrultusunda sabittir.
- Weathercocking, pitch attitude, gerçek yönelim animasyonu, türbülans, staging ve recovery deployment uçuş dinamiği yoktur.
- Tasarım çoklu bağlı kademeleri t=0 statik analizde birleştirir; uçuş hâlâ tek kademeyle sınırlıdır.
- Serbest biçimli kanat ve tube-fin için tam yapı/statik aero modelleri bekliyor.
- Alt-ağaç seviyesinde toplu mass/CG override yoktur; component override vardır.
- Harici eski dört kolonlu CSV desteklenir fakat downrange yoksa 2D yörünge uydurulmaz.
- 2D yörünge roket işaretçisi hız vektörünü gösterir; fiziksel gövde attitude'u değildir.

En doğru sonraki büyük adım: B3'teki gerçek 3DoF pitch/yaw state ve türevlerini küçük nodelar/çekirdekler halinde port etmek; önce bağımsız parity baseline kurmak, sonra staging ve recovery event state-machine eklemek. Mevcut axial solver'ı sessizce “3DoF” diye yeniden adlandırma.

## 8. Arayüz ve ürün tasarım dili

Uygulama Vite + React + TypeScript. 3D görünüm React Three Fiber/Three.js. Ortak UI paketi `packages/ui-kit` (`@stardrop/ui`) ve paylaşım kopyası `/home/ayberk/Projects/stardrop-ui`.

Yeni kontrol eklemeden önce şu kaynakları incele:

- `/home/ayberk/Projects/remotecode/workbench/src`
- `/home/ayberk/Projects/remotecode/ui-kit/src`
- `packages/ui-kit/src`

Tasarım kuralları:

- Her yerde bundled Michroma.
- Zemin siyah, yazı/çizgi beyaz; gri yüzeyler yok.
- Seçim/hover işareti ince düz beyaz çizgidir; yuvarlak renkli pill kalabalığı kullanılmaz.
- Desktop viewport aşağı kaymaz; yalnız panel içeriği kendi içinde scroll olur.
- Paneller mouse/touchpad ile resize edilir ve ölçü local storage'da korunur.
- Az yazı, büyükçe okunur etiket, ikon + tooltip; mikro açıklama satırları yok.
- Aynı bilgi iki yerde gösterilmez. Proje/marka/header hiyerarşisi korunur.
- Bileşen ağacı parent-child, drag/drop ve flow görünümü destekler. Shift çoklu seçim, ortak özellik düzenleme, F odaklanma, breadcrumb geri dönüş vardır.
- Canvas touchpad pan/zoom, snap, hizalama ve geometry handle'larıyla çalışır. Sayısal inspector her zaman kesin kaynaktır.
- 3D seçim ve odak tasarımla aynı domain state'ini kullanır. 15 sn idle showroom ayrı presentation katmanıdır.
- Hesap beklerken UI bloke edilmez; stale cevap yeni state'i ezemez. Eksik girdide eski sonuç gizlenir.

UI'da tekrar kullanılabilir bir şey gerekiyorsa önce `packages/ui-kit` içine genel bileşen olarak ekle, katalog/testini güncelle, sonra STARLAB'da tüket. Roket fiziğine özel bileşen UI kit'e taşınmaz.

## 9. Proje/domain ve `.star`

Ana domain `src/domain/` altındadır. `.star` bir “roket dosyası” değil tam proje dosyasıdır: parça ağacı, malzemeler, aero girdileri, motor kataloğu/eğrileri, motor atamaları, uçuş konfigürasyonları, ortam, simülasyon vakaları ve sonuçları tutulabilir. Import sırasında validate/migration, silme sırasında referans temizliği uygulanır. Undo/redo domain command/history katmanındadır.

Hesap request'lerini `.star` şemasına gömme. Domain şeması kullanıcı projesidir; engine protocol çalışma anı DTO'sudur. Aradaki tek yönlü dönüşüm `src/engine/*Inputs.ts` dosyalarında kalmalıdır.

## 10. Önemli dizinler

- `src/domain/`: proje, bileşen, seçim, history, `.star`, motor/uçuş şemaları.
- `src/components/`: tasarım canvası, hierarchy, inspector, startup/showroom.
- `src/components/engineering/`: motor, simülasyon, analiz, ortam ve grafik workspace'leri.
- `src/engine/`: UI→engine adaptörleri, protokol ve client.
- `server/`: hesap worker process sınırı ve Vite plugin.
- `engine/`: Python DAG, model/scenario kayıtları, FFI worker.
- `native/`: saf C++ hesap çekirdekleri.
- `engine/tests/`: node, dependency, incremental cache ve scenario testleri.
- `scripts/`: native build, B3 parity ve CALCS üretimi.
- `packages/ui-kit/`: Stardrop UI kaynak paketi.
- `public/brand/starlab.png`: tek resmi logo.
- `ideas.md`: yerel ilerleme/karar notu; GitHub'a gönderilmez.
- `CALCS`: yerel otomatik hesap indeksi; GitHub'a gönderilmez.

## 11. Çalıştırma ve doğrulama

Linux geliştirme:

```bash
cd /home/ayberk/Projects/B4
npm install
npm run build:engine
npm run dev
```

Tam doğrulama:

```bash
npm run build:engine
python3 -m unittest discover -s engine/tests -v
npm test
npm run check:engine
python3 scripts/update-calcs.py --check
npm run build
```

Son tam doğrulama: 45 Python engine testi, 142 Vitest testi ve production build başarılı. CALCS: 20 model / 114 node.

Windows için Node.js, Python 3 ve C++ compiler gerekir; native build script platforma uygun shared library üretir. Dağıtım aşamasında derlenmiş native motor ve Python runtime paketlenmelidir. Şu an repo geliştirme/web-shell düzenindedir; nihai Electron/Tauri/installer paketleme ayrı tamamlanmalıdır.

## 12. Çalışma disiplini

- Başlamadan `AGENTS.md` ve yerel `ideas.md` oku.
- Kullanıcının mevcut değişikliklerini silme/resetleme.
- Küçük, geri alınabilir commitler; mesajlar çok kısa lowercase English.
- Push yalnız kullanıcı isterse.
- Hesap node'u değiştiyse CALCS üret/check zorunlu.
- Her özellikte önce contract ve test, sonra UI.
- OpenRocket sonucuyla fark varsa sonucu zorla eşitleme; iki modelin varsayımlarını ve girdilerini tek tek karşılaştır.
- Güvenlik: token/secret hiçbir dosya, commit, remote URL veya terminal çıktısına yazılmaz.

## 13. Hesap ekibine arayüz devri

İlk 3DoF, kademe ve kurtarma hesapları için yeni bir kullanıcı girdi ekranı zorunlu değil. Mevcut UI şu verileri `.star` içinde saklıyor:

- `src/domain/project.ts` ve `src/domain/componentCatalog.ts`: parça hiyerarşisi, geometri, malzeme, kütle/CG override; serbest kanat dahil.
- `src/domain/motors.ts`, `src/domain/motorMass.ts`, `src/domain/flight.ts`: itki eğrisi, motor kütle profili, yuva yerleşimi, ateşleme olayı/gecikmesi, kademe ayrılması.
- `src/domain/recoveryInputs.ts` ve `src/domain/flight.ts`: paraşüt görevi, açılma irtifası/gecikmesi, Cd, alan, şişme süresi.
- `src/domain/environmentInputs.ts`: atmosfer/rüzgâr profilleri, türbülans ayarları; `src/domain/flight.ts`: ray, saha, zaman adımı ve azami süre.
- `src/domain/aerodynamics.ts`: kullanıcının roket katsayıları, referans ölçüleri ve akış noktası girdileri.

Bu girdilerin bulunması uçuş çözücüsünün onları kullandığı anlamına gelmez. `src/engine/flightSolver.ts::flightSolverIssues` şu an çoklu motoru, ayrılmayı, kurtarmayı ve sabit olmayan rüzgârı reddeder. `native/flight/axial.cpp` tek kademeli ve sabit gövde eksenli çözücüdür. Hesap ekibi yeni senaryoyu ayrı model olarak ekleyip `src/engine/flightSolver.ts` adaptörünü açık kapsamla güncellemelidir. Mevcut sonuç biçimi `src/domain/flight.ts::FlightSample` içindedir; yeni pitch/yaw, olay ve kurtarma telemetrisi için önce sürümlü çıktı sözleşmesi belirlenmeli, sonra 2D oynatıcı/grafikler bu alanları kullanmalıdır.

Hesap ekibinin başlaması için UI tarafında yapılacak en faydalı iş hazırdır: `src/components/engineering/SimulationWorkspace.tsx` içindeki Kontrol sekmesinden mevcut hesap girdisi JSON olarak indirilebilir. İlk 3DoF girdi/çıktı sözleşmesi netleşince çözücü modeli seçimi, olay çizgileri ve gerçek roket yönelimi sunumu ayrıca eklenir. Henüz uygulanmamış çözücü için etkin bir seçenek gösterilmemeli.
