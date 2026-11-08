# Tasarımdan aerodinamik yüklere · entegrasyon sözleşmesi

**Kullanıcının işi** roketi çizmek, parçaları yerleştirmek ve motoru seçmektir. **Aerodinamik arka planın işi** aktif dış geometri ile uçuş koşullarını okuyup her parça için yük, CP ve katsayıları döndürmektir. Hesap SI birimleriyle ve simülasyonun ortak koordinat hizmetini kullanır. Parça istasyonu burundan kuyruğa doğru artar; bu tek yerleşim kuralı CP ve CG değerlerinin anlamını belirler.

## Sistemler arasındaki bağlantı

| Sahip | Otomatik sağlanan veri | Ne zaman ve neden? |
|---|---|---|
| Tasarım / kademe yöneticisi | Aktif tasarım, dışarıya açık parçaların kimliği ve tipi, boyutları ve istasyonları, ortak referans çapı, açık taban diskinin sahibi | Her değerlendirmede aktif tasarım seçilir. Ayrılmış parçalar ve içeride kalan omuzlar listeye girmez. |
| Motor kütüphanesi | Seçili motorun anlık gövde itki vektörü [N], CG etrafında motor momenti [N·m], ateşleme durumu, tabanda akıştan çıkarılacak etkin motor alanı [m²] | İtki bileşkeye eklenir. Motor açıkken etkin alan taban sürüklemesini azaltır. Alan, motor modelinin konvansiyonuyla sağlanmalıdır; otomatik olarak boğaz alanına eşit sayılmaz. |
| Kütle / yapı | Anlık kütle [kg], burundan ölçülen CG [m]; DoF çözücüsüne ayrıca atalet ve zaman türevi | Moment referansı değişirse aynı yükle farklı Cm elde edilir. |
| Atmosfer ve rüzgâr | ρ, ses hızı, μ, kayıt için T ve P; zaman ve konuma göre rüzgâr vektörü | Rüzgâr profili, ölçüm veya türbülans sağlayıcısı aynı girişe bağlanabilir. Ara adımı yeniden değerlendirmek rastgele yeni bir rüzgâr çekmemelidir. |
| Dünya / yerçekimi | Konum, yükseklik datumu ve seçilen Dünya modelinden üretilen **g vektörü [m/s²]** | Sabit g, enlem/yükseklik tabanlı normal yerçekimi veya başka bir alan modeli aynı arayüzü kullanır. Geoit yükseklik referansı ile yerçekimi yasası ayrı seçimlerdir. Etkin g merkezkaç etkisini içeriyorsa ana dinamik çözücü onu ikinci kez eklememelidir. |
| Durum / koordinat hizmeti | Global ve kademe zamanı, konum, yönelim, gövde hızı ve açısal hız; vektörlerin ortak eksene dönüşümü | Aerodinamik modülde ikinci bir Dünya veya koordinat sistemi kurulmaz. |

**C++ bağlantısı:** geometry.hpp içindeki Host::activeDesign(request) ve Host::conditions(request) ana sistemin sağlayacağı iki çağrıdır. geometry::evaluateAt(host,request) doğrudan ana çözücüden çağrılır. Alternatif olarak mevcut örnek çözümde Config::host atanır ve aynı sağlayıcı RK4 ara adımlarında kullanılır. Host çıktılarının aynı zamana, kademeye, CG'ye ve referansa ait olması gerekir. Ana uygulama kaynakları bu pakette olmadığı için gerçek tasarım ekranına bağlanacak adaptör proje tarafında yazılmalıdır. geometry_example.hpp bu bağlantıyı sentetik bir roketle çalıştırır.

## Geometriden katsayılar · ilk uygulama

Önerilen ayrım üç hesap içerir: (1) küçük açı normal kuvvet eğimi ve CP, (2) eksenel sürükleme, (3) kuvvetin CG'ye aktarılması. Bütün parça katsayıları aynı S=πd²/4 ve d ile normalize edilir. Bu yüzden CD, CL ve Cm katkıları doğrudan toplanabilir. Aşağıdaki ilk sürüm konik parçalar, ince ve uzun rijit araç, düzgün yüzey, Mach ≤ 3.0 ve toplam geliş açısı ≤ 10° için ön tasarım yaklaşımıdır. Bu sınırlar uygulamanın seçtiği muhafazakâr sayısal zarftır; sınırlar içindeki her geometri için deneysel doğruluk garantisi değildir. Referans yaklaşım: [OpenRocket teknik dokümanı, §3.2 ve §3.4](https://openrocket.sourceforge.net/techdoc.pdf).

| Parça | Tasarım girdileri | Normal kuvvet ve CP | Sürükleme hesabı |
|---|---|---|---|
| Konik burun | Uç istasyonu, uzunluk L, taban yarıçapı | C_Nα=2A_taban/S; CP uçtan 2L/3 | Islak alan sürtünmesi + düşük hızlı konik basınç terimi |
| Silindirik gövde | Başlangıç istasyonu, uzunluk ve yarıçap | Doğrusal ince-gövde terimi sıfır; CP tanımsız | Gövde sürtünmesi; yalnız açık arka taban kendisine aitse taban katkısı |
| Genişleyen konik transition | Ön/arka yarıçap r₀,r₁, L | C_Nα=2π(r₁²−r₀²)/S > 0 | Eğim ve alan artışından basınç sürüklemesi + sürtünme |
| Daralan konik boattail | Aynı veri, r₁<r₀ | Aynı bağıntıdan negatif eğim; CP ağırlığı işaretini korur | Daralma oranına göre kuyruk basınç kaybı + sürtünme; kalan açık disk ayrı |
| Trapez kanatçık takımı | Sayı N, yarı açıklık b, kök/uç veter cᵣ,cₜ, uç hücum kenarı geri kayması Δ, kalınlık, montaj yarıçapı | Sonlu açıklık ve gövde–kanat etkileşim çarpanı ile eğim; planformdan CP | İki yüzün sürtünmesi + kare ön/arka kenar varsayımıyla basınç kaybı |

**Konik parçalar.** Başlangıç istasyonu s₀ için s_CP=s₀+L(r₀+2r₁)/[3(r₀+r₁)]. Bu, doğrusal değişen yarıçapta alan değişiminin birinci momentinden gelir. Sabit kesitte payda olarak kullanılacak normal kuvvet eğimi sıfır olduğundan CP alanını boş bırakırız. Ogive, parabolik ve Haack burunlarda aynı 2L/3 sonucu kullanılamaz. Bir sonraki genişletme alan profilinden ∫s dA / ∫dA ve şekle özel sürükleme olmalıdır.

**Kanatçıklar.** ℓ_m=√[b²+(Δ+(cₜ−cᵣ)/2)²] orta veter hattı uzunluğudur. Uygulanan eğim (1+r/(r+b))·4N(b/d)²/[1+√(1+(2ℓ_m/(cᵣ+cₜ))²)]. CP, kök hücum kenarından Δ(cᵣ+2cₜ)/[3(cᵣ+cₜ)]+(cᵣ+cₜ−cᵣcₜ/(cᵣ+cₜ))/6 uzaklıktadır. Bu takım modeli N≥3 eş aralıklı, aynı geometrili, kant açısı sıfır kanatçık içindir. Tek tek kanatçık yükü, kant kaynaklı roll ve dönme sönümü için panel konumu, yönü ve yerel hava hızı gerekir. Bunlar ilk geometri hesabına dahil değildir.

**CD, CL ve Cm.** Eksenel katsayı; düz plaka sürtünme yaklaşımı, ıslak alan, şekil basıncı ve varsa taban sürüklemesinden elde edilir. Doğrusal normal eğim geliş açısının yanal/düşey bileşenleriyle birleştirilir. Elde edilen kuvvet hava hızına göre izdüşürülerek CD ve CL üretilir. Gövde eksenindeki normal kuvvet katsayısı ile rüzgâr eksenindeki CL farklı büyüklüklerdir. M_CG=(r_CP−r_CG)×F, Cm=M_y/(QSd); sıfır hızda yükler sıfırdır. Araç CP’si Σ(C_Nα,i s_CP,i)/ΣC_Nα,i; payda sıfır veya iptal nedeniyle çok küçükse CP tanımsızdır. CP metre, katsayılar boyutsuzdur. CD/CL/Cm araç referansına göre bileşen katkısıdır. Simetrik eksenel yük CG doğrusu üzerinde uygulandığında moment oluşturmaz.

**Okuma ve çalıştırma.** Paket içinde:
~~~powershell
.\ucus.exe --geometry 6 ucus.csv bilesenler.csv
.\kontroller.exe ornek.txt
~~~
İlk komut beş parça tipini bir arada çalıştırır. bilesenler.csv her zaman noktasında parça kimliği, CP, CD, CL, Cm, kuvvet ve CG momentini içerir. Uçuş CSV’sindeki Mx/My/Mz aerodinamik momentlerdir; geometri API’sindeki netMoment motor momentini de içerir. Metin dosyasındaki row tabloları eski modelin karşılaştırma yoludur. Ürün akışında kullanıcıdan bu tablolar istenmez.

**Sınırlar ve sonraki doğrulama.** Örnek adaptör düz Dünya atmosferi ve sabit g kullanır; asıl API harici g ve rüzgâr alır. Ayrı bir geoit/jeodezi kütüphanesi uygulanmamıştır. Demo çözücü faz içinde sabit CG ve köşegen atalet kullanır; hareketli CG için ana çözücü evaluateAt üzerinden anlık kütle/CG sağlar ve kendi değişken kütle dinamiğini yürütür. Motor olay zamanları ve RK4 aralıklarının bölünmesi ana simülasyonun sorumluluğudur. Yüksek hücum açısı, pürüzlülük, transonik taban akışı ve profil çeşitliliği için veri veya doğrulanmış modeller gerekir. Önce küçük açı ve düşük Mach'ta bileşen eğimi/CP, ardından CD bütçesi, en son tam uçuş karşılaştırılmalıdır.


## Üç rejimde uçuş · v0.7

| Model bandı | Geometriden gelen hesap |
|---|---|
| M ≤ 0.8 | Kanat normal kuvvet eğiminde β²=1−M² ile sonlu açıklık/süpürme düzeltmesi. Gövdede konik basınç, taban kaybı ve Reynolds/Mach bağımlı sürtünme. |
| 0.8 < M < 1.3 | İki uçtaki katsayılar 3u²−2u³ ağırlığıyla bağlanır; konik basınç için M=1 noktasında sin(ε) ara değeri kullanılır. Bu yaklaşık geçiş yerel şok konumu veya gerçek sürükleme tepesini çözmez. |
| 1.3 ≤ M ≤ 3 | Konik basınç katsayısı 2.1sin²ε+0.5sinε/√(M²−1); genişleyen transition için alan farkına ölçeklenir. İnce kanatta iki taraflı lineer eğim 4/√(M²−1), takıma N/2 projeksiyonuyla uygulanır. Taban katsayısı 0.25/M. Boattail için düzlemsel Prandtl–Meyer genişlemesinden basınç açığı hesaplanır. |

Kanat CP'si M=0.5'e kadar çeyrek ortalama aerodinamik veterdedir. 0.5–2 arasında yaklaşık düzgün geçiş, M≥2 için (ARβ−0.67)/(2ARβ−1) veter oranı kullanılır. Denklem düşük açıklık oranında güvensizse kod durur. Toplam geliş açısı sınırı 10°, yüksek hızda boattail dönüş açısı sınırı 20°'dir. İnce kanat teorisi üç boyutlu uç ve şok etkileşimlerini, boattail yaklaşımı ayrılmış akışı çözmez. Eksenel gövde normal kuvvet eğimi ince gövde varsayımını korur. Zarf sayısal kullanım sınırıdır; gerçek araç validasyonu değildir.

Örnek: **ucus.exe --mach-flight 6 ucus.csv parcalar.csv** (3DOF için 6 yerine 3). Uçuş kaydı her anda regime, Mach, CP_m, CD, CL, CmCG içerir. Eşikler yerel akışın evrensel sınırları değil, kullanılan mühendislik modellerinin seçim sınırlarıdır. Kaynaklar: [OpenRocket §3.2, §3.4 ve Ek B](https://openrocket.sourceforge.net/techdoc.pdf), [NASA Glenn genişleme modeli](https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/centered-expansion-fan/).
### v0.7 · Boattail girişi
Her boattail için aynı istasyon ve yarıçapta biten dış gövde parçası aranır. İmzalı dönüş önceki yüzeyin bitiş eğimi ile kuyruğun eğimi arasındaki farktır. Örnek transition → boattail yaklaşık 18,80°, aynı kuyruk silindirden sonra 14,04° verir. Eşleşme yoksa eksenel giriş varsayılır ve sonuç etiketlenir. Yüksek hız kolu 0–20° genişleme için geçerlidir; sıkıştırma dönüşünü veya ayrılmış akışı çözmez.
Ana servis Conditions.tailInlets listesinde parça kimliği, yerel Mach ve statik basınç (Pa) gönderebilir. M>1,2'de sağlanan yerel Mach >1 olmalıdır; yerel Prandtl–Meyer hesabından duvar basıncı çıkarılır. Sürükleme katsayısı (p_serbest−p_duvar)/Q_serbest üzerinden normalize edilir. 1,2–1,3 arasında yerel katkı kademeli devreye girer. Veri yoksa serbest akım yaklaşımı kullanılır. Parça CSV'sinde tail_turn_deg ve tail_inlet_method sütunları gerçek kullanılan dalı bildirir. Yerel koşullu sonuç da bağlı, düzlemsel akış yaklaşımıdır.