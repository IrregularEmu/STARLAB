# STARLAB

Roket tasarım çalışma alanı. React, TypeScript ve Vite.

```sh
npm install
npm run dev
npm test
npm run build
```

Arayüz `../remotecode/ui-kit` içindeki `@stardrop/ui` paketini kullanır. Bu iki dizini birlikte tutun. UI kit kaynakları bu projede değiştirilmez.

Yerel proje yoksa boş başlanır. Dosya → Yeni proje (Ctrl/Cmd N), Örnek roket aç ve Önceki projeyi aç belge akışlarıdır. Boş canvas yalnız ortadaki + düğmesini gösterir. Kütüphanede sekiz burun ailesi dahil 28 bileşen seçeneği, malzeme seçiminde 82 kayıt vardır. Kaynaklar, birimler ve şematik önizleme sınırları [katalog notlarında](docs/legacy-catalog.md) açıklanmıştır.

`src/domain` proje ağacı, tipler, doğrulama ve komutları içerir. `src/engine` saf hesap modülleri için bağımlılık grafiği ve mock envanter modülleridir. `src/components` çizim katmanıdır. `src/i18n.ts` Türkçe mesaj kataloğu ve tipli çeviri girişidir; yeni locale buraya eklenebilir.

Boyutlar mm, girilen kütle g cinsindedir. Konum ebeveyne göre tanımlanır. SVG geometriyi şematik gösterir; özel alt sistemler kademe başlangıcına göre yerleştirilir. Arayüz ekibinin işi kullanıcı girdilerini toplamaktır; aerodinamik ve uçuş hesapları ayrı ekipten bağlanacaktır. Geometri, kütle, aero ve not alanları seçili bileşenin panelinde düzenlenir.

Proje sürümlü `.star` dosyası olarak içe/dışa aktarılır ve tarayıcıda otomatik saklanır. Eski JSON dosyaları da okunur. `.star` tüm mevcut proje verisini, motorları, konfigürasyonları, senaryoları ve sonuçları taşır. Araçlar menüsünden hesap ekibine ayrı girdi paketi çıkarılır. Birimler ve bilinmeyen değerlerin anlamı [dosya sözleşmesinde](docs/star-format.md) tanımlıdır. Bağımlılık grafiği gelecekte hesap modüllerini bağlamak için ayrılmıştır; şu an fizik çözücüsü değildir.

Ctrl/Cmd Z geri alır, Ctrl/Cmd Shift Z veya Y ileri alır. Ctrl/Cmd S `.star` indirir, Ctrl/Cmd O proje açar. Geçmiş bütün sayfalardaki proje değişikliklerini kapsar; panel/kamera değişikliklerini kapsamaz. Aynı alandaki yazım gruplanır, son 80 adım bellekte tutulur. Proje açılınca geçmiş sıfırlanır.

2D/3D görünümün alt araçlarında geri al, ileri al ve seçili bileşeni sil ikonları bulunur. Canvas'ta parça seçildikten sonra Delete silme onayı açar; alt bileşenler ve uçuş bağlantıları birlikte kaldırılır, geri al ile birlikte geri döner. Metin alanlarında Delete yazıyı siler; Ctrl/Cmd + Delete bileşen silmez. Roket kökü silinemez.

2D'de parçayı sürüklemek eksenel konumu, sağ tutamacı sürüklemek uzunluğu, üst tutamacı sürüklemek çapı değiştirir. Touchpad'de iki parmak kaydırma görünümü taşır; pinch yakınlaştırır. Boş alana çift tık veya Tüm roket düğmesi görünümü sıfırlar. Ölçüler sürükleme sırasında panelde canlı önizlenir, bırakılınca tek geçmiş adımı olarak kaydedilir; Esc, pencere odağı kaybı veya pointer iptali önizlemeyi geri alır. Şematik 3D ve işlem girdileri tamamlanan düzenlemeyi aynı proje verisinden okur.

Mıknatıs açıkken gövde/burun çapları diğer gövde/burunlarla, diğer türlerin çapları kendi türleriyle hizalanır. Aynı türdeki çaplar önceliklidir; üst ve alt kademe gövdeleri bu şekilde eşitlenebilir. Taşıma/uzunluk düzenlemede sabit kalan uyumlu parçaların başlangıç/bitiş kenarları gözetilir; seçili parçayla birlikte hareket eden elemanlar hedef yapılmaz. Ekranda 6 px yakında yapışır, 11 px dışına çıkınca bırakır; eşikler zoom'a göre mm'ye çevrilir. İnce kesikli kılavuz yalnız eşleşme sırasında görünür. Mıknatıs düğmesi kapatır, Alt geçici olarak atlar. Normal sürükleme 1 mm, Shift 10 mm, Alt 0,1 mm hassasiyet kullanır. Tam değerler panelden her zaman girilebilir.

Ek klavye desteği: canvas parçaları Tab ile seçilir; sol/sağ ok konumu, Alt + sol/sağ uzunluğu, Alt + yukarı/aşağı çapı değiştirir. Yukarı/aşağı ok önceki/sonraki parçaya geçer. Shift büyük adım kullanır. Bu kontroller mevcut şematik uzunluk/çap modelini düzenler; serbest kanat kontrol noktaları henüz yoktur.

## Alt sistem ve çoklu seçim

Başlangıçta kademeler katlıdır. Satırdaki giriş oku veya çift tıklama alt sisteme girer; F aynı odağı açar. Ağaç, akış, görünüm ve parça ekleme hedefi bu kapsama geçer. Üst ok veya yol çubuğu geri döner. Arama geçerli kapsamda adları süzer ve eşleşen parçaların üst yollarını gösterir. Dalları daralt düğmesi ağacı sadeleştirir. Kapsam değişince dışarıda kalan seçimler temizlenir.

Shift + tıklama ağaç/akış/2D/3D üzerinde seçime parça ekler veya çıkarır. Ctrl/Cmd A, ağaç veya canvas odaktayken geçerli kapsamın fiziksel parçalarını seçer. Seçim listesi sağ panelde görünür; listedeki ada tıklamak tek parçaya geçer, x seçimden çıkarır.

Çoklu seçimde yalnız ortak ve aynı anlamdaki alanlar gösterilir. Farklı değerler “Farklı” yer tutucusudur; sıfır veya bilinmiyor yerine geçmez. Bir alanı düzenlemek o değeri açıkça seçilen her parçaya uygular; diğer alanlar ve her parçanın kendi notları korunur. Et kalınlığı, yalnız tüm seçim kabuk türleri olduğunda; kanat kalınlığı yalnız tüm seçim kanat seti olduğunda gösterilir. Sayısal sınırlar tüm seçim için geçerlidir; geçersiz toplu işlem kısmen uygulanmaz. İsteğe bağlı alanı temizlemek tüm seçilenlerde o girdiyi kaldırır.

2D'de çoklu seçimin ortak kutusu vardır. Sağ tutamaç uzunlukları ve seçili parçaların eksenel aralıklarını birlikte, üst tutamaç çapları birlikte ölçekler. Oranlar korunur; minimum boyut/et kalınlığı sınırında ölçek sınırlanır. Taşıma tutamacı veya seçili parçadan sürükleme seçim grubunu taşır. Yerleşim çözümü ebeveyn/çocuk veya seri parçaları iki kez kaydırmaz. Tek seçili kademe/alt sistemin tutamaçları onun fiziksel alt ağacını düzenler. Kütle/CG/CP girdileri geometriyle birlikte otomatik hesaplanmaz veya ölçeklenmez. Mıknatıs grup dışındaki uyumlu ve sabit kalan hedefleri de gözetir. Bir sürükleme tek geri alma adımıdır; Esc tamamını iptal eder.

Ortak paneldeki Alt sistem oluştur, aynı kademe/alt sistem altında gruplanabilen seçimleri yeni bir `assembly` içine alır ve o çalışma alanını açar. Parça kimlikleri, uçuş bağlantıları ve mevcut dünya konumları korunur; yerel konumlar yeni hiyerarşiye göre düzenlenebilir. Adı sağ panelden değiştirilebilir. Gruplama, toplu özellik değişikliği ve alt ağaçlarla toplu silme geri alınabilir. Bunlar `.star` içinde normal proje verisi olarak saklanır; seçim ve açık çalışma kapsamı geçici arayüz durumudur.

## 2D ve 3D

Canvas üzerindeki 2D/3D düğmelerinden görünüm seçilir. 3D ilk kullanımda yüklenir (Three.js, React Three Fiber 8, WebGL2). Sol sürükleme döndürür, sağ sürükleme kaydırır, tekerlek yakınlaştırır. Tıklama parçayı seçer; F seçili parçaya girer. Boşluğa tıklayıp F ile bir üst seviyeye çıkılır. Yol çubuğu iki görünümde de ortaktır. Ölçüler bileşen panelinde düzenlenir.

`domain/geometry.ts` motor bağımsız yerleşim, profil ve sınır kutusu üretir. İki görünüm aynı veriyi okur; mm → metre dönüşümü yalnız 3D adaptöründedir. Yeni parça geometrileri `components/geometry3d.ts` kaydına eklenir. Geometri yalnız boyutlar değişince yenilenir; görünüm demand modunda boşta çizim yapmaz, DPR 1.5 ile sınırlıdır. Görünüm kapatılınca kaynaklar serbest bırakılır.

3D önizleme şematiktir: gövde/motor zarfı, seçilen burun profili, geçiş ve kanat planformu gösterilir. Kanat sayısı girilmişse kullanılır; eski kayıtlarda görsel varsayım üç kanattır. Kanat kalınlığı henüz görsel olarak 3 mm'dir; kurtarma parçaları paket zarfıdır. Pod/tüp kanat tekrarları ve omuz/radyal ayrıntıların tam CAD gösterimi beklemektedir. Hazır model içe aktarma, CAD ve 3D taşıma henüz eklenmedi. Ayrıntılar [katalog notlarındadır](docs/legacy-catalog.md).

## Hiyerarşi ve çalışma alanı

Ağaç ve Akış aynı proje verisini gösterir. Ağaçta satır ortasına bırakmak hedefin içine taşır; üst/alt kenarına bırakmak aynı seviyede sıralar. Akış kutuları üzerine bırakmak ebeveyni değiştirir. Tür kuralları, kök ve döngü kontrolleri iki görünümde ortaktır. Taşınan bileşenin çocukları ve yerel konumları korunur; dünya konumu yeni ebeveyne göre yeniden hesaplanır. Esc taşımayı iptal eder.

Akış görünümü özel SVG/HTML çizimidir. Boşluğu sürükleyerek, orta tuşla veya Space + sürükleyerek gezinilir. Tekerlek kaydırır; Ctrl/Meta + tekerlek imleç çevresinde yakınlaştırır. Sığdır düğmesi tüm hiyerarşiyi gösterir. Özellikler üst panelde doğrudan düzenlenir ve alt görünüm anında güncellenir.

STARLAB logosu `public/brand/starlab.png`, yerel Michroma ve lisansı `public/fonts` içindedir. Proje adı üst menüde düzenlenir; yeşil tik yerel kaydın tamamlandığını, turuncu döngü kaydın beklediğini veya başarısız olduğunu belirtir. Kayıt kısa bir gecikmeyle otomatik yapılır. Bu işaret uçuş hesabının geçerliliğini ifade etmez.

Panel ayraçları sürüklenerek bileşen/görünüm yüksekliği ve ağaç/özellik genişliği değiştirilebilir. Çift tık veya Home varsayılanı geri getirir; ok tuşları ince, Shift + ok daha büyük adımlarla ayarlar. Boyutlar projeden bağımsız olarak yerelde saklanır ve pencere küçülünce iki panel de görünür kalacak şekilde sınırlanır. Parça ekleme bileşen başlığındaki + düğmesindedir.

Her açılışta STARLAB logosu ve üst bölümde CSS ile parlayan yıldızlar iki saniye gösterilir; ardından 650 ms fade ile çalışma alanına geçilir. Çalışma alanı arka planda hazırlanır, geçiş bitene kadar etkileşim almaz. Azaltılmış hareket tercihinde yıldız ve geçiş animasyonları kapatılır.

Logo çevresindeki yörünge/yıldız yükleyicisi belirsiz ilerleme gösterir; sahte yüzde üretmez. Açılış en az iki saniye sürer ve yerel fontun yüklenmesini, logonun decode edilmesini ve ilk çalışma alanı yerleşimini bekler. Eksik bir varlık uygulamayı kilitlemez; yanıt vermeyen kaynaklar için sekiz saniye sınırı vardır. Bu hazırlık görevleri `domain/startup.ts` üzerinden genişletilebilir. Açılışta kullanılmayan 3D paketi bu beklemeye dahil değildir; ilk 3D seçiminde ayrıca yüklenir.

## Motor, simülasyon ve analiz

Dört çalışma sekmesi aktiftir. Motor kataloğu arama, yuvaya uyum filtresi, boyutlu önizleme, itki grafiği, özel motor oluşturma/düzenleme ve RASP `.eng` içe/dışa aktarmayı destekler. Başlangıçtaki ST-54/75/98 motorları **sentetik örnek veridir**, gerçek ürün veya sertifikalı motor değildir. Motor kütleleri kg, çap/uzunluk mm, eğri zamanları s ve itki N cinsindedir. `.rse` henüz desteklenmez. İtki integrali trapez yöntemiyle veri eğrisinden hesaplanır; atmosfer düzeltmesi yapılmaz.

Konfigürasyonlar motor yuvalarına atamalar, ateşleme olayları/gecikmeleri, kademe ayrılması ve paraşüt açılma koşullarını saklar. Atanmış özel motor silinemez; bağlı simülasyonu olan konfigürasyon silinemez. Parça silindiğinde onun uçuş bağlantıları da temizlenir. Yeniden ebeveynleme parça kimliğini korur. Proje doğrulaması ve 50 MiB proje dosyası sınırı hatalı içe aktarmaların mevcut projeyi değiştirmesini engeller; motor/CSV içe aktarmalarının kendi sınırları vardır.

Simülasyon sayfasında senaryo oluşturma/kopyalama/silme, konfigürasyon seçimi, saha/ray/atmosfer/rüzgâr ve çözüm ayarları vardır. **Uçuş çözücüsü bağlı değildir; Çalıştır bu nedenle pasiftir.** Motor ve aero hesapları yerine sahte uçuş sonuçları üretilmez. Harici sonuçlar CSV ile yüklenebilir ve dışa aktarılabilir. İçe aktarılan veriler her zaman harici veri olarak belirtilir; mevcut tasarımı gerçekten temsil ettiği doğrulanmış sayılmaz. Girdiler değiştiğinde veri ayrıca güncel değil olarak işaretlenir.

Sonuç CSV başlığı ve birimler:

```csv
t,altitude,velocity,acceleration
0,0,0,0
1,10,20,5
```

Sütunlar sırasıyla s, m, m/s, m/s²; sayılar ondalık nokta kullanır. Zamanlar artmalı, dosyada 2–20000 örnek bulunmalıdır. İrtifa örneklerinin referansı harici veri üreticisinde tutarlı seçilmelidir; uygulama referans dönüşümü yapmaz.

`engine/flightSolver.ts` içindeki `createFlightRequest`, kullanıcı/proje verisini bağımsız bir hesap girdisine dönüştürür; `registerFlightSolver` C++/WASM/Tauri adaptörünün bağlanacağı sözleşmedir. `solve(request, signal)` giriş imzasını ve `{t, altitude, velocity, acceleration}` örneklerini döndürür. İmza güvenlik doğrulaması değil, girdinin değiştiğini izleyen bir sürüm işaretidir. Girdi veya seçili senaryo değişince devam eden hesap iptal edilir. Çözücü bağlıyken dahi uygun olmayan/atanmamış motorlarla çalışma başlatılmaz.

Analiz sayfası ortak geometriden boyutları, motor eğrilerini ve iki harici/hesaplanmış veri setini karşılaştırır. Kütle ve aero tabloları elle girilen verileri gösterir; toplam kütle/CG/CP/kararlılık sonucu üretmez. Kalınlık, yoğunluk, CG, CP, Cd, Mach, hücum açısı ve pürüzlülük alanları boş bırakılabilir; işlem paketinde bilinmeyenler `null` olur. Kaynak ve referans açıklamaları [dosya sözleşmesinde](docs/star-format.md), arayüz eksikleri kaynak bağlantılarıyla [OpenRocket incelemesinde](docs/openrocket-ui-audit.md), genel kapsam [ROADMAP.md](ROADMAP.md) içindedir.
