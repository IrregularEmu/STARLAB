# C++ uçuş modeli · kullanım ve teknik inceleme

Bu sürüm, ray çıkışından sonra tanımlı süre boyunca bir veya birden fazla rijit gövde konfigürasyonunun serbest uçuşunu çözer. Örnek iki kademenin 4.2 saniyelik bölümüdür. Veriler yazılım kontrolleri için sentetiktir. Birleşik roketten üst kademeye geçiş izlenir; ayrılan alt kademenin ayrı yörüngesi hesaplanmaz.

**Ürün entegrasyonu:** [Tasarımdan aerodinamik yüklere](ENTEGRASYON.md) sözleşmesi ve geometry.hpp varsayılan geometri yolunu tanımlar. Aşağıdaki senaryo metni ve 12 katsayılı satırlar eski karşılaştırma kipine aittir. Geometri kipinde kullanıcı çiziminden otomatik hesap yapılır.

## Çalıştırma

Windows x64 paketini bir klasöre açıp PowerShell'de o klasörden çalıştırın:

~~~powershell
.\kontroller.exe ornek.txt
.\ucus.exe ornek.txt 3 ucus3.csv yuzeyler3.csv
.\ucus.exe ornek.txt 6 ucus6.csv yuzeyler6.csv
~~~

Çıktı için yeni dosya adları kullanın. Program hata kodu 1 ile çıkarsa CSV o ana kadar yazılmış kısmi kayıttır. Kaynaktan derlemek için C++14 derleyicisiyle bağımlılık gerektirmeyen CMake projesi sunulmuştur:

~~~text
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
~~~

## Girdi ve çıktı sözleşmesi

| Model | Girdi (sıra ve birim) | Çıktı |
|---|---|---|
| Başlangıç | setup: adım [s], saha rakımı [m], başlangıç AGL [m], gövde x hızı [m/s], pitch [°], üç NED rüzgâr bileşeni [m/s], standard veya archive | İlk CG konumu ve yönelimi. NED kuzey, doğu, aşağı. |
| İsteğe bağlı başlangıç | initial: roll [°], yaw [°], u,v,w [m/s], p,q,r [rad/s]. setup sonrasında kullanılır; u,v,w ilk eksenel hızı değiştirir. | 6DOF genel başlangıç durumu. 3DOF düzlemi kuzey–aşağıdır. |
| Atmosfer | Saha rakımı ve CG'nin aşağı koordinatı; geopotansiyel yükseklik 0–20 km | T [K], P [Pa], ρ [kg/m³], ses hızı [m/s], μ [Pa·s]; μ yüzey Re hesabına gider |
| Rüzgâr | setup sabiti veya kademe içinde wind: yerel zaman [s], kuzey/doğu/aşağı [m/s]. Profil 0–kademe sonunu kapsar. | Bağıl gövde hızı → V [m/s], α,β [rad], Mach, dinamik basınç Q [Pa] |
| Kademe | phase: ad, süre, yanma süresi [s], sabit eksenel itki [N], ilk/son kütle [kg], ortak buruna doğru eksende CG konumu [m], ilk ve son asal ataletler [kg·m²] | Aktif kütle, atalet ve CG; ayrılmada yeni referans noktasının konumu ve hızı |
| Motor | İsteğe bağlı motor: yerel zaman [s], itki [N]. 0–yanma sonu arasında artan zamanlar; eğri sabit itki değerinin yerini alır. | Doğrusal enterpolasyonla gövde +x ekseninde CG'den geçen etkin itki |
| Aerodinamik yüzey | surface: isim, referans alanı [m²], referans uzunluğu [m], kuvvet/moment referans noktası x,y,z [m], toplam geliş açısı sınırı [°] | Yüzeye ait Re=ρVd/μ, gövde kuvveti [N], anlık CG etrafında moment [N·m] |
| Katsayı tablosu | row: Mach ve aşağıdaki 12 katsayı. Her yüzeyin kendi tablosu vardır. Mach düğümleri artar. | Mach'ta doğrusal enterpolasyon. Zarf dışındaki durum koşuyu hatayla bitirir. |
| DoF | Mod 3 veya 6, yükler, kütle, atalet, yerçekimi | Zaman serisi: N,E,D [m], gövde u,v,w [m/s], p,q,r [rad/s], gövdeden yere Hamilton kuaterniyonu, toplam aero kuvvet ve aero moment, itki, ağırlık ve bileşke kuvvet |

Her kademe başında zaman sıfırlanır; CSV'deki zaman ise uçuş boyunca artar. Ayrılma zamanında biri eski, biri yeni konfigürasyon için iki satır vardır. Gövde x ekseni buruna, y sağa, z aşağıya bakar. Konumları tüm kademeler için aynı yapısal orijinden girin. Açılar girişte özellikle belirtilen alanlarda derece, katsayı hesaplarında radyandır.

## Aerodinamik yüzey katsayıları

Katsayılar tek bileşen veya bir kanatçık takımının toplam katkısı için, araç eksenlerine göre tanımlanır. Örnekte burun/gövde ve kanatçık takımı ayrı hesaplanır. Bir takımı toplam alanıyla verdiyseniz kanatçık sayısıyla tekrar çarpmayın. Her yüzeyin tablosunda sıra şöyledir:

| Sütun | Anlam |
|---|---|
| CDoff, CDon | Motor kapalı/açık eksenel akışta sürükleme katsayısı |
| CDa2 | Sürüklemeye eklenen (α²+β²) teriminin katsayısı |
| CLa, CLq | Kaldırmanın α ve boyutsuz yunuslama hızı qd/(2V) türevleri |
| CYb | Yanal kuvvetin β türevi |
| Cl0, Clp | Serbest yuvarlanma momentinin sabiti ve pd/(2V) türevi |
| Cma, Cmq | Yüzey referansındaki serbest yunuslama momentinin α ve qd/(2V) türevleri |
| Cnb, Cnr | Aynı noktadaki serbest sapma momentinin β ve rd/(2V) türevleri |

Kuvvetler QS ile, serbest momentler QSd ile ölçeklenir. Sonra (r_yüzey−r_CG)×F eklenir. Eğimler radyan başınadır. V→0 için sönüm çarpanı QS·d/(2V)=ρVSd/4 biçiminde hesaplanır. Yüzeylere gövde hızından ortak geliş açısı verilir. Yerel dönme akışını ayrıca hesaplayan veya tüm aracın sönümünü içeren veri kullanılırsa burada sönümü ikinci kez saymamak gerekir. Yüzey tablosu, geometriden otomatik katsayı tahmini veya yerel kanat ekseninde bir profil poları değildir. Bu sürüm Re'yi raporlar; katsayıları Re yönünde enterpole etmez.

## Teknik bulgular ve kontroller

| Konu | İnceleme sonucu | Kontrol |
|---|---|---|
| Arşivde yaw/roll momenti | SID 572'de iki kolda referans uzunluğu görünmüyor. Katsayı boyutsuzsa sonuç N'dir. C++ girişi boyutsuz moment katsayısı ister, bütün eksenlerde QSd uygular. | Birim ve moment uzunluğu testi geçti. Arşiv katsayılarının normalizasyonu bilinmiyor. |
| CG ve moment işareti | Yüzeydeki moment ile CG'ye taşınmış moment ayrı hesaplanır. | Gövdenin arkasındaki yükün çapraz çarpım işareti, sürükleme gücü ve CSV toplamı kontrol edildi. |
| 3DOF | İki öteleme ve yunuslama bağımsız boylamsal denklemlerle çözülür; yönelim kuaterniyonun düzlemsel alt kümesinde tutulur. 6DOF ile aynı fiziksel girdiler kullanılır. | Sabit yunuslama momenti, analitik balistik uçuş, düzlemsel eşdeğerlik ve düzlem dışı yük/rüzgâr reddi geçti. [MathWorks gövde denklemleri](https://www.mathworks.com/help/aeroblks/3dofbodyaxes.html) |
| Sürükleme ve referans alanı | Motor açık/kapalı katsayıları ayrı. Kademe yanma aralığı motor durumunu seçer; geçici motor kesintisi ayrı modellenecekse faz veya durum mantığı gerekir. | Her yüzeyin kuvveti kendi S alanıyla hesaplandı. [NASA, roket taban sürüklemesi](https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/rocket-aerodynamics/) |
| Kütle ve atalet | Kaynak MAT dosyalarında aynı adla farklı değerler vardır. Bu sürüm seçilen senaryo değerlerini kullanır. Yanma boyunca m ve I bağımsız, doğrusal verilerdir; kademede CG sabittir. | Atalet pozitifliği ve asal moment üçgen eşitsizlikleri kontrol edilir. Gerçek araç için zaman serileri, CG hareketi ve egzozun açısal momentum akısı hâlâ doğrulanmalıdır. |
| Dönme dinamiği | Asal eksenlerde Iω̇=M−ω×Iω−Īω. Ek açısal momentum akısı sıfır; itki CG doğrultusunda. | Saf yunuslama sayısal örneği ve yanal uyarımla sapma işareti geçti. Eğik itki, asimetrik atalet tensörü veya ayrılma impulsu bu sürümün dışında. |
| Olay ve integrasyon | RK4 adımı yanma bitimi, motor/rüzgâr düğümleri ve kademe sonuna bölünür. İmpulssuz ayrılmada yeni CG için Δv=ω×Δr uygulanır. | CSV'de olayın iki tarafındaki CG konum ve hız farkları doğrulandı. |
| Atmosfer ve rüzgâr | Arşiv ve standart profil ayrı seçeneklerdir. Filtrelenmiş türbülansın spektrumu doğrulanmadığı için ölçülmüş veya önceden üretilmiş rüzgâr dizisi yeniden oynatılabilir. | Deniz seviyesi, 11 km sınırı, hava hızı değişmezliği ve sıfır bağıl hız testleri geçti. |

**Ölçülen sonuçlar.** Windows x64'te derlenen kodla iki örnek mod da 4.2 saniyeyi tamamladı. 6DOF örneğinde 213 zaman kaydı alındı. Adımı 0.02 s'den 0.01 s'ye indirince son konum farkı yaklaşık 5.92×10⁻⁹ m oldu. CSV'de altı toplam yük bileşeni için yüzey toplamlarıyla en büyük sayısal fark yaklaşık 1.01×10⁻¹² idi (bileşene göre N veya N·m). Ek yanal rüzgâr/motor eğrisi senaryosu 6DOF'ta doğu koordinatını −0.9883 m'ye taşıdı; 3DOF aynı girdiyi reddetti.

Bu sonuçlar yazılımın denklemleri uygulamasını sınar. Arşivdeki 20 s ayrılma, türbülans filtreleri ve gerçek motor/atalet tablolarıyla Simulink yörünge eşleşmesi gösterilmedi. Serbest uçuş koşusu verilen süre sonunda biter; yere ulaşılırsa hata bildirir. Ray kısıtı, paraşüt, temas, yüksek açılı aerodinamik ve gerçek araç kalibrasyonu kapsam dışıdır.

## Her adımda çalışan kontroller

Kuvvet ve Moment Modeli motoru örnekler ve gövde eksenlerinde Fnet = Faero + T + W hesabını yapar. W=mRᵀg. İtki CG'den geçtiğinden itki momenti sıfırdır. Uçuş CSV’sindeki eski Fx,Fy,Fz yalnızca aerodinamiktir. Tx_N…Tz_N itki, Wx_N…Wz_N ağırlık, FnetX_N…FnetZ_N bileşkedir. Bütün bu sütunlar gövde ekseninde newton cinsindendir.

Her türev değerlendirmesinde bileşke ve hesaplanan ivme ayrı ayrı yer eksenlerine çevrilir. Her ikisi de bağımsız olarak kurulan R(Faero+T)+mg_N ile uyuşmalıdır. Kuvvet toleransı 10⁻⁸ N + 10⁻⁹(|Faero|+|T|+mg); dönme dengesi de CG etrafında benzer bir mutlak ve göreli toleransla kontrol edilir. Denge ihlali koşuyu durdurur ve kademe adı ile yerel zamanı bildirir.

Her nominal RK4 adımı bir tam adım ve iki yarım adımla hesaplanır. Fark sınırları konum için 0.1 mm, hız için 0.0001 m/s, açısal hız için 0.000001 rad/s ve işaretleri eşlenmiş birim kuaterniyonlar arasındaki mesafe için 0.0000005'tir. Eşik aşılırsa aralık ikiye bölünür; 12 bölme düzeyinde hâlâ geçmeyen adımda hata bildirilir. Kabul edilen sonuç iki yarım adımdan gelir. Bu eşikler yerel sayısal tutarlılığı ölçer; gerçek uçuş hatasının sınırı değildir.

kontroller.exe; analitik itki darbesi + süzülme çözümünü iki modda kontrol eder. Ayrıca ağırlığın ve itkinin iki kez sayıldığı iki sentetik hatalı kayıt üretir; ikisinin de denetim tarafından reddedildiğini doğrular.
