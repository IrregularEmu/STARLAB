# OpenRocket arayüz karşılaştırması

İnceleme: 28 Ekim 2026. Kapsam kullanıcıdan veri toplama, proje düzenleme ve sonuç sunma. Uçuş/aerodinamik yöntemlerini uygulamak bu arayüz ekibinin kapsamı değildir. Kaynak kod karşılaştırması OpenRocket `unstable` dalına aittir; yayımlanan sürümlerin kapsamı değişebilir.

Katalog güncellemesi: 28 bileşen seçeneği, sekiz burun ailesi, temel geometri alanları ve 82 malzemeli hacimsel/yüzeysel/çizgisel katalog eklendi. Aşağıdaki geometri/katalog eksikleri ileri düzenleme, tam CAD çizimi ve kalıcı kullanıcı kütüphanesi açısından okunmalıdır. [Katalog kapsamı](legacy-catalog.md).

## Mevcut temel

STARLAB'da bileşen ağacı/akış, alt sistem odaklama, ortak 2D/3D seçim, panel boyutlandırma, doğrudan özellik düzenleme, motor ve uçuş konfigürasyonları, senaryolar, harici sonuç grafikleri vardır. Bu dilimde elle girilen CG/CP, kütle kapsamı, yoğunluk, kalınlık, Cd, akış koşulu, notlar, sürümlü `.star` dosyası ve proje genelinde undo/redo eklendi.

CG/CP alanları **kullanıcı girdisidir**. Hesaplanmış merkezler veya kararlılık sonuçları değildir. Aero alanları tek bir Mach/hücum açısı koşulunu kaydeder. Hesap ekibinin sonuçları geldiğinde ayrı kaynak ve birim bilgisiyle sunulmalıdır.

## En büyük eksikler

| Öncelik | Eksik | Arayüzde yapılacak iş | Hesap ekibine bağımlılık |
| --- | --- | --- | --- |
| 1 | Parçaya özgü ayrıntılı geometri | Kanat sayısı/kök-uç veter/açıklık/süpürme/kant; burun profil türü/omuz; geçişlerin iki çapı; halka/kapak/bağlayıcı gibi parça türleri | Alan sözleşmesini birlikte belirlemek; form ve çizim için çözücü gerekmez |
| 1 | Konum referansları ve yönelim | Ön/arka/ebeveyn referansı seçimi, radyal/açısal yerleşim, çoğaltılan parçalar ve motor kümeleri | Eksen ve referans sözleşmesi gerekir |
| 1 | Veri tamlığı görünümü | Eksik alanı ilgili parçaya bağlayan açılabilir liste; zorunlu girdiler modül sözleşmesinden gelsin | Hangi modülün hangi girdiyi istediği gerekir |
| 1 | Aero veri tablosu | Mach/AoA'ya bağlı Cd/CP vb. tablolar; CSV içe aktarma, birim/referans/sütun eşleme ve veri önizleme | Tablo şeması gerekir; enterpolasyon çözücüye aittir |
| 2 | Malzeme ve parça hazırları | Kullanıcı malzeme kütüphanesi, yoğunluk türü/birimi, hazır parça arama ve kaydetme | Katalog verileri gerekir; fiziki hesap gerekmez |
| 2 | Düzenleme araçları | Kopyala/yapıştır/çoğalt. Çoklu seçim, ortak alanlar, kapsamlı ağaç araması ve seçimden alt sistem oluşturma artık mevcut. | Yok |
| 2 | Birim tercihleri | Alan bazında birim seçimi; dosya/işlem sınırında tanımlı birimlere dönüştürme | Ortak birim sözleşmesi gerekir |
| 2 | Ölçülmüş veri kapsamı | Kütle dışında CG/Cd için ayrı alt bileşen kapsamı, ölçüm kaynağı, belirsizlik ve atalet alanları | Kapsam ve atalet eksenleri birlikte tanımlanmalı |
| 2 | Görünüm araçları | CG/CP girdisi ve hesap sonucunun farklı işaretleri, kesit/ön/arka görünüm, kademe görünürlüğü, ölçüm araçları | Girdi işaretleri için yok; hesap sonucu için çıktı sözleşmesi |
| 2 | Sonuç sunumu | Daha çok değişken, çoklu eksen, olay işaretleri, birim seçimi ve bölüm/kademe sonuçları | Zaman serileri ve olay çıktıları gerekir |
| 3 | Proje araçları | Proje açıklaması/yazar/sürüm, son projeler, kurtarma geçmişi, `.ork`/`.rkt` aktarımı, SVG/üretim çıktıları | Dosya eşleme ve masaüstü dosya erişimi gerekir |

Önerilen bir sonraki dilim: **parçaya özgü geometri formları**. Önce kanat ve burun, ardından geçiş ve iç parçalar. En büyük veri açığı bunlarda; yeni hesap yöntemi eklemeden arayüzü belirgin biçimde tamamlar.

## Kaynaktan alınabilecek mimari fikirler

- Ortak bileşen özelliklerini ortak editörde tutmak, parça türüne özgü alanları ayrı editörlerde eklemek. `RocketComponentConfig`, `BodyTubeConfig` ve `TrapezoidFinSetConfig` bu ayrımı gösteriyor. STARLAB'da ortak `PartInputs` üzerinden aynı yönde ilerlenebilir.
- Birim ve aralık bilgisini alan modelinde tutmak. OpenRocket formları `DoubleModel` / `UnitSelector` kullanıyor; bizde alan tanımları ileride tek kayıt üzerinden doğrulama ve form üretebilir. Fizik hesapları bu kayda gömülmemeli.
- Proje dosyasını yalnız geometriye indirgememek. `OpenRocketSaver` bileşen ağacını, simülasyonları, özel veri tanımlarını ve belge tercihlerini saklıyor. `.star` bugün tüm mevcut proje verisini taşır; görünüm/panel tercihleri kullanıcıya ait yerel ayarlardır.
- Elle girilen ölçümle çözücü sonucunun kaynağını ayırmak. OpenRocket'teki override kavramı referanstır; STARLAB'da şu anda hesap sonucu bulunmadığı için alanlara hesap sonucu etiketi verilmez.

## İncelenen kaynaklar

- [Temel tasarım ve özellik sekmeleri](https://openrocket.readthedocs.io/en/latest/user_guide/basic_rocket_design.html)
- [Kütle/CG/Cd kapsamı](https://github.com/openrocket/openrocket/blob/unstable/docs/source/user_guide/overrides_and_surface_finish.rst)
- [Gelişmiş parça ve kurtarma girdileri](https://openrocket.readthedocs.io/en/latest/user_guide/advanced_rocket_design.html)
- [Grafik değişkenleri, olaylar ve aero tabloları](https://openrocket.readthedocs.io/en/latest/user_guide/advanced_flight_simulation.html)
- [Ortak bileşen editörü](https://github.com/openrocket/openrocket/blob/unstable/swing/src/main/java/info/openrocket/swing/gui/configdialog/RocketComponentConfig.java)
- [Gövde editörü](https://github.com/openrocket/openrocket/blob/unstable/swing/src/main/java/info/openrocket/swing/gui/configdialog/BodyTubeConfig.java)
- [Kanat editörü](https://github.com/openrocket/openrocket/blob/unstable/swing/src/main/java/info/openrocket/swing/gui/configdialog/TrapezoidFinSetConfig.java)
- [Proje kaydedicisi](https://github.com/openrocket/openrocket/blob/unstable/core/src/main/java/info/openrocket/core/file/openrocket/OpenRocketSaver.java)
