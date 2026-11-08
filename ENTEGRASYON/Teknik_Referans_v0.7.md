# Roket uçuş simülasyonu
**Teknik referans · C++ v0.7 · 4 Kasım 2026**


## Nomenclature — Semboller ve birimler

Denklemlerde kalın harf vektör veya matrisi, üzerindeki nokta zamana göre türevi, şapka birim vektörü gösterir. Aşağıdaki birimler SI'dır. Açılar hesapta radyan, örnek geometrilerde açıkça belirtildiğinde derecedir. Denklemlerde ondalık ayırıcı noktadır.

| Sembol | Anlam | Birim |
|---|---|---|
| $h,H,R_e$ | Deniz seviyesine göre geometrik ve geopotansiyel irtifa; etkin Dünya yarıçapı | m |
| $T,T_C,p,\rho,\mu,a$ | Mutlak sıcaklık, Celsius sıcaklığı, statik basınç, yoğunluk, dinamik viskozite, ses hızı | K; °C; Pa; kg/m³; Pa·s; m/s |
| $R_a,\gamma,\lambda$ | Özgül gaz sabiti, özgül ısılar oranı, atmosferde sıcaklık eğimi | J/(kg·K); 1; K/m |
| $\mathbf g_N,g_0$ | Yer çerçevesinde etkin yerçekimi vektörü; standart yerçekimi | m/s² |
| $\mathbf v_B,\mathbf w_N,\mathbf v_{a,B},V$ | Gövde bileşenlerinde yer hızı; yer eksenlerinde rüzgâr; gövdede bağıl hava hızı ve büyüklüğü | m/s |
| $M,\mathrm{Re},Q$ | Mach sayısı, Reynolds sayısı ve dinamik basınç | 1; 1; Pa |
| $\alpha,\beta,\phi,\theta,\psi$ | Hücum, kayma, yuvarlanma, yunuslama ve sapma açıları | rad |
| $S,d,L,\ell$ | Referans alanı, referans çapı, parça uzunluğu, karakteristik uzunluk | m²; m; m; m |
| $s,s_0,s_{CG},s_{CP},r_0,r_1$ | Burundan kuyruğa artan istasyon, parça başlangıcı, kütle ve basınç merkezi istasyonları, ön/arka yarıçap | m |
| $N,b,c_r,c_t,\bar c,\Delta,t$ | Kanat sayısı, açıklık, kök/uç/ortalama aerodinamik veter, geri kayma, kanat kalınlığı | N boyutsuz, diğerleri m |
| $A_f,A_{wet},B$ | Tek kanadın planform alanı, etkin ıslak alan, sürüklemeye katkının alan eşdeğeri | m² |
| $D,Y,L,\mathbf F_a,\mathbf F_{net},\mathbf W$ | Sürükleme, yan kuvvet, kaldırma, aerodinamik kuvvet, bileşke dış kuvvet ve ağırlık | N |
| $C_A,C_D,C_L,C_Y,C_f,C_p$ | Eksenel direnç, sürükleme, kaldırma, yan kuvvet, sürtünme ve basınç katsayıları | 1 |
| $C_l,C_m,C_n,C_{N\alpha},C_{m\alpha}$ | Yuvarlanma, yunuslama, sapma moment katsayıları; normal kuvvet ve yunuslama momentinin açı eğimleri | İlk üçü 1; eğimler rad⁻¹ |
| $\varepsilon,\delta,\nu,M_1,M_2$ | Konik yarı açı, akış dönüş açısı, Prandtl–Meyer açısı, giriş/çıkış Mach sayıları | rad; rad; rad; 1; 1 |
| $m,\mathbf I,\mathbf H,\mathbf M,\boldsymbol{\omega}$ | Kütle, atalet tensörü, açısal momentum, moment, açısal hız | kg; kg·m²; kg·m²/s; N·m; rad/s |
| $u,v,w,p,q,r$ | Gövde hız bileşenleri ve yuvarlanma/yunuslama/sapma hızları | m/s; rad/s |
| $R,\mathbf k,I_{sp},\tau,t_b$ | Gövdeden yere dönüşüm matrisi, birim kuaterniyon, özgül itki, kademe yaşı ve yanma süresi | 1; 1; s; s; s |

**İndisler ve işaretler.** B ve N gövde ve yer çerçevesi, CG ve CP kütle ve basınç merkezi, ref referans noktası, i bileşen, ∞ serbest akım, 1 ve 2 yerel giriş ve çıkıştır. Gövde x buruna, istasyon s kuyruğa, yerin düşey ekseni aşağıya doğrudur. Pozitif moment sağ el kuralını izler.

**Aynı harfin farklı görevleri.** $M$ Mach, $\mathbf M$ moment; $Q$ dinamik basınç, $q$ yunuslama hızıdır. $p$ atmosfer bölümünde basınç, dinamikte yuvarlanma hızıdır. $T$ gaz bağıntılarında sıcaklık, motor bağıntılarında itki; $L$ geometride uzunluk, yüklerde kaldırmadır. Atmosferdeki $\lambda$ sıcaklık eğimi, boattail bölümündeki $\lambda$ boyutsuz uzunluk oranıdır. RK4 bölümünde $h$ zaman adımıdır (s); atmosferdeki $h$ irtifadır (m).

Bu belge bir tasarım kararının izini sürmek için yazıldı: Kullanıcı kanatçığı geriye taşıdığında önce hangi geometrik terim değişir, bu değişim momenti nasıl etkiler ve yörüngeye nerede yansır? Yanıt üç modelin birbirine verdiği değerlerde bulunur. Atmosfer hızı ve yük ölçeğini belirler; aerodinamik model bileşenlerden kuvvet ve moment çıkarır; DoF çözücüsü bu yüklerle yeni durumu hesaplar. Sonraki ara adımda aynı zincir yeniden değerlendirilir.

| Yol | Kullanım | Kanıt sınırı |
|---|---|---|
| Ürün: geometriden yük | Aktif tasarım → bileşen CP, CD, CL, Cm → araç yükleri. Kullanıcı roketi ve motoru seçer. | C++ arayüzü ve örnek sağlayıcı çalışır. Gerçek tasarım ekranının bağlantısı proje tarafında yapılır. |
| Karşılaştırma: katsayı tablosu | Yüzey başına Mach tablosu ve açısal hız türevleri ile yük üretir. | Uzman/test yoludur. Buradaki sönüm, geometri yolunun özelliği sayılmaz. |
| Simulink arşivi | İki kademeli modeldeki blok ve sinyal ilişkileri hareket denklemlerine referans oldu. | Statik inceleme yapıldı; Simulink yörüngesiyle tam eşleşme doğrulanmadı. |

**Ortak gösterim.** S alan [m²], d referans çapı [m], CG kütle merkezi, CP basınç merkezi, Q dinamik basınç [Pa], q yunuslama hızı [rad/s], M bağlama göre Mach sayısı veya moment vektörü [N·m]. B gövde, N yer çerçevesi, R gövdeden yere dönüşüm. İstasyon burundan arkaya, gövde x buruna, gövde z aşağıya artar. Dinamik çözümün kullandığı açı radyandır. Koordinat ve datum hizmeti ana simülasyonun sorumluluğundadır.

## Sistemin sınırı ve Simulink kararları

| Kaynak | C++ modelinde kullanım | Neden |
|---|---|---|
| SID 122, 1005 | Atmosfer ve hava hızını ayrı sağlayıcılar haline getirdik. Arşivde 980 m saha ofseti ve türbülans filtreleri var. | İrtifanın referansını karıştırmamak; spektrumu doğrulanmamış filtreyi standart kabul etmemek. |
| SID 190, 360 ve kök | Katsayı yolu ile geometriden hesap ayrıldı. SID 360’ın toplam Cd0 çıkışı göstergeye gider; taban sürükleme kolu ve kök tablolar etkin yola girer. | Blok adı tek başına uçuşta kullanılan denklemi göstermiyor. |
| SID 572 | Kuvvetleri QS, boyutsuz momentleri QSd ile ölçekliyoruz. | Arşivde yaw/roll kolunda uzunluk görünmüyor. Katsayı birimleri belgelenmeden aynen alınırsa kuvvet moment diye kullanılabilir. |
| SID 215, 313, 337 | Gövdede öteleme, dönme ve konum integrasyonu korundu. Düzlemsel 3DOF yeni bir indirgemedir. | Yunuslama ataleti ve momenti korumak; dik uçuşta Euler açısı tekilliğinden kaçınmak. |
| SID 175 ve kademe anahtarları | Motor, kütle ve atalet ayrı girdilerdir. dk4motor ve inertia birden fazla MAT dosyasında farklı değerler içerir. | Yükleme sırasının roket özelliklerini değiştirmesini önlemek. Arşivdeki 20 s ayrılma bütün tasarımlar için sabit değildir. |

| Veri sahibi | Aerodinamik modüle gelen | Kullanıldığı yer |
|---|---|---|
| Tasarım/kademe | Dış parçalar, istasyonlar, profil, çap, alan, açık taban sahibi | Aktif geometrinin katsayı ve moment hesabı |
| Çevre/Dünya | Yoğunluk, ses hızı, viskozite, basınç, sıcaklık, rüzgâr ve yerel g | Bağıl akış, genişleme, ağırlık. Geoit yükseklik datumu ve yerçekimi yasası ayrı seçimlerdir. |
| Motor/kütle | İtki vektörü, CG etrafında itki momenti, yanma durumu, etkin motor alanı; anlık m ve CG | Net yük, taban basıncı ve moment kolu |
| Çözücü | Zaman, aktif kademe, konum, hız, yönelim, açısal hız | Aynı fiziksel zamana ait servis çıktılarının değerlendirilmesi |
| İsteğe bağlı aerodinamik veri | Boattail kimliğiyle yerel Mach ve statik basınç | Süpersonik daralmadaki duvar basıncının daha temsil edici hesabı |

## 1. Atmosfer Modeli

**Fiziksel soru.** Roket bulunduğu konumda hangi havanın içinde hareket ediyor? Aynı araç aynı hızla iki farklı irtifada uçtuğunda yoğunluk değişir. Yoğunluk kuvvetin büyüklüğünü, ses hızı ise hangi Mach sayısında katsayı aranacağını belirler. Bu nedenle basınç, sıcaklık ve rüzgâr farklı görevler üstlenir.

Entegre kullanımda Dünya servisi seçilen referansa göre g vektörünü, atmosfer servisi tutarlı yükseklik datumuyla T, P, ρ, a, μ ve rüzgârı sağlar. Bu bölümdeki iki hesap bağımsız test sağlayıcısıdır. Yerçekimi alanı ile geoit yüksekliği ayrı girdilerdir.

### 1.1 Arşivdeki hesap · ATM-01

Girdi deniz seviyesinden h [m]; modelin saha ofseti 980 m. Alt dalda $T_C=15.04-0.00649h$ ve $P=101.29\left(\frac{T_C+273.1}{288.08}\right)^{5.256}$. Üst dalda $T_C=-56.4$ ve $P=22.65\cdot2.718^{1.73-0.000157h}$. Eşik 11000 m. P kPa, $\rho=\frac{P}{0.2869(T_C+273.1)}$ kg/m³. Ses hızı $a=331+0.61T_C$ [m/s], $M=\frac{V}{a}$. Bunlar arşivin yaklaşık bağıntılarıdır. Mutlak sıcaklığı, pozitif basıncı ve katman eşiğindeki iki tek taraflı değeri kontrol et. Üst irtifa geçerlilik sınırı belgelenene kadar üst dalı sınırsız uçuş modeli olarak kullanma. Standart atmosfer ayrı adlandırılmış bir sağlayıcı olabilir.

Örneğin roket saha seviyesinden 100 m yükselirse atmosfer bloğu 1080 m görür. Geometrik yükseklik ile kalkıştan itibaren yükselişi ayrı alanlar olarak tutmak, ofsetin iki kez eklenmesini önler. Arşiv denklemlerini yeniden üretirken basınç kPa’dır. Bir sonraki bölümdeki standart atmosfer denklemleri Pa kullanır.

### 1.2 Basınç ve yoğunluğun türetilmesi · ATM-02

Bir hava sütununun tabanındaki basınç, üstündeki havanın ağırlığını taşır. Hidrostatik denge geometrik irtifada $\frac{dp}{dh}=-\rho g(h)$ verir. Kuru ideal gaz için $p=\rho R_aT$. Bu iki ifadeyi birleştirince, sıcaklık profilinin basınç profilini neden belirlediği görülür. Standart atmosferde geometrik irtifa h, geopotansiyel irtifaya çevrilir: $H=\frac{R_e h}{R_e+h}$, Rₑ=6 356 766 m. Böylece $\frac{dp}{dH}=-\rho g_0$ yazılır. [U.S. Standard Atmosphere 1976, Bölüm I](https://ntrs.nasa.gov/api/citations/19770009539/downloads/19770009539.pdf)

Her katmanın taban değerleri H_b, T_b, p_b ve sıcaklık eğimi $\lambda=\frac{dT}{dH}$ olsun. Denklemlerde T kelvin, p paskal, Rₐ=287.05 J/(kg K), g₀=9.80665 m/s² alınır.

| Koşul | Sıcaklık | Basınç |
|---|---|---|
| λ≠0 | $T=T_b+\lambda(H-H_b)$ | $p=p_b\left(\frac{T}{T_b}\right)^{-\frac{g_0}{R_a\lambda}}$ |
| λ=0 | $T=T_b$ | $p=p_b\exp\left[-\frac{g_0(H-H_b)}{R_aT_b}\right]$ |

İkinci satır birincinin λ→0 limitidir. Bir katmanın son basıncı ve sıcaklığı bir sonraki katmana başlangıç değeri olur. Önerilen ilk kapsam 0–20 km geopotansiyel irtifadır. 0–11 km için λ=−0.0065 K/m, 11–20 km için λ=0. Deniz seviyesinde T=288.15 K, p=101325 Pa ile başlanır. Yoğunluk $\rho=\frac{p}{R_aT}$, kuru hava için ses hızı $a=\sqrt{\gamma R_aT}$, γ≈1.4. Reynolds sayısı kullanılacaksa dinamik viskoziteyi $\mu=1.716\times10^{-5}\left(\frac{T}{273.15}\right)^{3/2}\frac{273.15+110.4}{T+110.4}$ Pa·s ile hesapla. [Standart atmosfer, fiziksel özellikler ve tablolar](https://ntrs.nasa.gov/citations/19770009539)

**Kontrol.** Standart profilde H=11 km için iki dal yaklaşık 216.65 K ve 22.632 kPa vermelidir. Arşiv formüllerinde aynı sayısal eşikte alt dal −56.35 °C, üst dal −56.4 °C verir. Bu küçük farkı görünür tutmak hangi sağlayıcının çalıştığını anlamaya yardım eder. Önerim arşiv profilini karşılaştırma, standart profili yeni hesaplar için ayrı seçenek yapmak. Her koşu seçtiği profili kaydeder.

### 1.3 Rüzgâr ve atmosfer belirsizliği · ATM-03

Rüzgâr, hava kütlesinin yere göre **gittiği yönde** tanımlanan bir vektördür. Ölçüm “batıdan esiyor” diyorsa doğu bileşeni pozitiftir. Profil irtifaya göre ortalama rüzgârı verir. Türbülans seçilirse zamana bağlı dalgalanma eklenir. Arşivin 1005 numaralı alt sistemi üç gürültü kaynağı ve filtreler içerir. Filtreyi adlandırmadan önce hedef varyansı, zaman ölçeği ve eksenleri doğrulanmalı. Özellikle düşey filtrede görülen Lv/Lw farkı test edilmeli. Tekrarlanabilir karşılaştırmada aynı rüzgâr geçmişi iki çözücüye de verilir.

**Otomatik girişler:** saha, datum, rüzgâr profili ve seçilen atmosfer sağlayıcısından alınır.

## 2. Kuvvet ve Moment Modeli

**Fiziksel soru.** Hava roketi hangi yönde itiyor ve burnunu hangi yönde döndürüyor? Kuvvetin büyüklüğü tek başına bu soruları cevaplamaz. Aynı büyüklükteki yan kuvvet, kütle merkezinin önünde ve arkasında zıt dönme etkileri yaratır. Bu nedenle her yükün ekseni, uygulama noktası ve aktif kademe geometrisi bilinmelidir. [NASA Glenn, basınç merkezi ve moment](https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/generalized-center-of-pressure/)

**Varsayılan ürün hesabı.** Konik burun, silindirik gövde, trapez kanatçık, genişleyen transition ve daralan boattail geometriden hesaplanır. Her parçanın CD, CL, CG etrafındaki Cm ve tanımlıysa küçük açı CP istasyonu döner. Aynı referans alanıyla normalize edilen katsayılar toplanır; CP normal kuvvet eğimleriyle ağırlıklandırılır. Silindirik gövdenin doğrusal normal eğimi sıfır olduğunda CP boş kalır. Ayrıntılı geometrik bağıntılar ve Mach/açı sınırları entegrasyon sözleşmesindedir. Arşiv tablosu ve sönüm türevleri uzman karşılaştırma yoludur.

### 2.1 Roketin gördüğü akış · FLOW



$$
\mathbf v_{a,B}=\mathbf v_B-R^{\mathsf T}\mathbf w_N,\quad V=\lVert\mathbf v_{a,B}\rVert,\quad Q=\frac{\rho V^2}{2}
$$

 $(U,V_y,W)=\mathbf v_{a,B}$. $\alpha=\operatorname{atan2}(W,U),\quad\beta=\operatorname{atan2}(V_y,\sqrt{U^2+W^2})$. Yer hızını konum integrasyonu için koru. Rüzgâr sağlayıcısı yer çerçevesinde ortalama ve isteğe bağlı türbülans verir; aynı gerçekleşme iki modun karşılaştırmasında tekrar kullanılabilmeli. Arşivdeki gürültü filtrelerinin spektrumu doğrulanmadan bunları belirli bir standart türbülans modeli diye etiketleme. V=0 için bu yarı kararlı modelde yükler sıfır, açılar tanımsız; V’ye bölmeden dön. Küçük pozitif V’de sönüm terimlerini Q ile birlikte değerlendir; geçerli hız zarfını sağlayıcı bildirsin.

Reynolds sayısı $\mathrm{Re}=\frac{\rho V\ell}{\mu}$, atalet etkilerinin viskoz etkilere oranını karakterize eder. ℓ, katsayı verisi üretilirken kullanılan uzunluk olmalıdır. Sabit katsayılarda yoğunluk iki katına çıkarsa yük iki katına, hız iki katına çıkarsa yük dört katına çıkar. Hız değişimi Mach ve Re’yi de değiştirebildiğinden gerçek araçta ikinci oran tam dört olmayabilir.

### 2.2 Katsayı hangi fiziksel bilgiyi taşır? · COEF

Ortak çıktı (C_D,C_Y,C_L,C_l,C_m,C_n). Büyük L kaldırma, küçük l yuvarlanma. Simulink mantığını koruyan ayrım: C_D kademe, Mach ve motor durumundan; C_L hücum açısı ve yunuslama hızından; C_Y ve C_n kayma açısından; C_m normal yük ve yunuslama sönümünden; C_l asimetri ve yuvarlanma sönümünden beslenir. Örneğin $C_L=C_{L\alpha}\alpha+C_{Lq}\frac{qd}{2V},\quad C_Y=C_{Y\beta}\beta,\quad C_n=C_{n\beta}\beta$. Bunlar katsayı arayüzünün biçimidir. Kaynak modelde C_m statik terimi $-2(C_L\cos\alpha+C_D\sin\alpha)$ içerir; 2 katsayısının geometrik referansı ayrıca doğrulanır. Arşivdeki katsayı değerleri araç verisi olarak onay bekler. Veri paketinde her türev için eksen, referans, birim, hız/açı aralığı ve kademe belirtilir. Sınır dışı veya eksik veri bayrakla bildirilir.

Yüzeyin küçük bir parçasında basınç ve viskoz kayma bir kuvvet oluşturur. Dış normal n, yerel gösterge basıncı Δp ve kayma gerilmesi vektörü τ ile $d\mathbf F=(-\Delta p\,\mathbf n+\boldsymbol{\tau})\,dA$ yazılır. Tüm yüzeyde $\mathbf F=\int d\mathbf F$, seçilen O noktası etrafında $\mathbf M_O=\int(\mathbf r-\mathbf r_O)\times d\mathbf F$ elde edilir. Katsayılar bu integrallerin ölçeklendirilmiş karşılığıdır. S veya d değişirse aynı fiziksel yükü verecek katsayı da değişir. Örneğin C_D S çarpımı korunarak referans alanı dönüştürülebilir. [NASA Glenn, roket aerodinamiği](https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/rocket-aerodynamics/)

Küçük açı açılımında C_Lα, α’daki bir radyanlık yerel değişime karşı kaldırma eğimidir. C_Lq ise açısal hız etkisini temsil eder. qd/(2V) boyutsuzdur; yarım karakteristik uzunluktaki dönmenin oluşturduğu hızın uçuş hızına oranı gibi okunabilir. Türev derece başına verilmişse radyan başına değeri 180/π ile çarpılır. Bu lineerleştirme, ölçüldüğü açı ve Mach aralığıyla sınırlıdır. Transonik geçiş, akış ayrılması, yüksek hücum açısı veya ters akış ayrı veri gerektirir. OpenRocket’in bileşen tabanlı teorisi, küçük açı ve uçuş rejimi varsayımlarının nasıl ayrılabileceğine örnektir. [Niskanen, OpenRocket Technical Documentation, Bölüm 3](https://openrocket.info/documentation.html)

**Arşivde etkin yol.** SID 360’ın toplam sürükleme çıkışı bir göstergeye gider. Uçuşta kullanılan katsayı kökte kademe/Mach tablolarından seçilir ve motor durumuna göre taban sürüklemesi terimi çıkarılır. Bu ayrım fiziksel olarak anlamlıdır; motor gazları roket tabanındaki basıncı değiştirebilir. Ancak sayısal düzeltmenin bu araçta doğru olduğu ayrıca gösterilmelidir. SID 914’teki roll tablosu Mach 0.1–0.95 aralığındadır; başka bir tablonun Mach 3’e kadar uzanması bu tabloya ek geçerlilik sağlamaz. Geometri yolu ve arşiv katsayı yolu ayrı seçeneklerdir. [NASA Glenn, motor durumunun taban sürüklemesine etkisi](https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/rocket-aerodynamics/)

### 2.3 Kuvvetin yönü ve CG etrafındaki moment · LOAD



$$
D=QSC_D,\quad Y=QSC_Y,\quad L=QSC_L
$$

 Gövde bileşenleri için $\mathbf e_D=(\cos\alpha\cos\beta,\sin\beta,\sin\alpha\cos\beta),\quad\mathbf e_Y=(-\cos\alpha\sin\beta,\cos\beta,-\sin\alpha\sin\beta),\quad\mathbf e_L=(\sin\alpha,0,-\cos\alpha)$. $\mathbf F_{a,B}=-D\mathbf e_D+Y\mathbf e_Y+L\mathbf e_L$. Böylece $\mathbf F_{a,B}\cdot\mathbf v_{a,B}=-DV$.

Boyutsuz moment katsayıları için $\mathbf M_{ref}=QS(d_l C_l,d_m C_m,d_n C_n)$. Her referans uzunluğu [m] katsayı kaynağıyla eşleşmeli. $\mathbf M_{CG}=\mathbf M_{ref}+(\mathbf r_{ref}-\mathbf r_{CG})\times\mathbf F_{a,B}$. CG referanslı katsayıda kol sıfır. S [m²], geometri ve referans noktası her kademe için açıkça taşınır. Arşivde yunuslama için d=0.17 kullanılır.

e_D bağıl hız boyunca, e_Y ve e_L ona diktir. Bu yüzden sürükleme hava hızından enerji alır, diğer iki bileşenin bu hız doğrultusundaki işi sıfırdır. Bu sonuç, eksen dönüşümünü test etmek için yalnızca grafik incelemekten daha güçlü bir kontroldür.

**İşaret örneği.** CG’nin bir metre arkasındaki kanatçığa gövde ekseninde yukarı doğru 2 N uygulanırsa kol (−1,0,0) m, kuvvet (0,0,−2) N olur. Çapraz çarpım (0,−2,0) N·m verir. Bu burun aşağı momentidir. Aynı kuvvet CG’nin bir metre önüne taşınırsa işaret tersine döner. Katsayı zaten CG etrafındaki toplam momenti veriyorsa bu kolu bir kez daha eklemek aynı etkinin iki kez sayılmasıdır.

**Arşivdeki boyut denetimi.** SID 572’de yunuslama QS d C_m, sapma QS C_n ve yuvarlanma QS C_l biçiminde. Son iki katsayı boyutsuz ise sonuç N olur. Alternatif açıklama, uzunluğun katsayı içine gömülmüş olmasıdır. Katsayı tanımıyla bu açıklama sınanmalı. Sentetik testte Q=100 Pa, S=0.02 m², d=0.2 m, C=0.1 için beklenen moment 0.04 N·m’dir.

**Bileşke kuvvet.** Yüzeylerin toplamından sonra motor eğrisinden T alınır. Motor kapalıysa T=0. Mevcut uygulamada itki gövde +x ekseninde CG’den geçer. $\mathbf W_B=mR^{\mathsf T}\mathbf g_N,\quad\mathbf F_{net,B}=\mathbf F_{a,B}+(T,0,0)+\mathbf W_B$. İtki ve ağırlık birer kez eklenir. DoF Modeli yalnız F_net,B ve CG etrafındaki momenti kullanır.

### 2.A Rejim seçimi ve sıkıştırılabilirlik

**Neden rejim seçilir?** Basınç değişimini aynı geometri farklı hızlarda farklı yollardan üretir. Subsonik kanat bağıntısındaki $\sqrt{1-M^2}$ ile süpersonik bağıntıdaki $\sqrt{M^2-1}$ aynı yerde kullanılamaz. Dinamik basıncın bütün hızlarda $Q=\frac{\rho V^2}{2}$ olması akışın her hızda sıkıştırılamaz kabul edildiği anlamına gelmez. Sıkıştırılabilirlik etkisi çoğunlukla katsayıdadır.

| Hız | Uygulanan model | Yorum sınırı |
|---|---|---|
| 0 ≤ M ≤ 0,8 | Sonlu açıklıklı subsonik kanat; Mach'a bağlı taban ve ön kenar basıncı | M→0 limitinde sıkıştırılamaz davranış. Mach 0,3'te ayrı bir çözücüye geçilmez. Burun basınç katsayısı bu bantta sabit. |
| 0,8 < M < 1,3 | Uç değerler arasında sürekli enterpolasyon | Yerel şok ve ayrılma hesaplanmaz. Mach 1'de konik basınç için ayrıca bir dayanak değeri kullanılır. |
| 1,3 ≤ M ≤ 3 | Konik dalga sürüklemesi, ince kanat basınç farkı, taban kaybı, boattail genişlemesi | Küçük geliş açısı ve desteklenen geometriler. Üç boyutlu şok etkileşimi çözülmez. |

Genel harman için $x=\frac{M-0.8}{0.5},\quad w=x^2(3-2x),\quad C=(1-w)C_{sub}(0.8)+wC_{sup}(1.3)$. Bu seçim Mach 1'de tekil teorileri çalıştırmadan değer sürekliliği sağlar. İki dalın dış türevlerini eşleştirmez. Ölçülmüş transonik tepe garantisi yoktur.

### 2.B Normal kuvvet eğimi ve basınç merkezi

Tüm ürün geometrisinde ortak çap d, alan $S=\frac{\pi d^2}{4}$ kullanılır. Burundan kuyruğa artan istasyon s, gövdenin buruna doğru artan x ekseninin ters yönündedir. $A(r)=\pi r^2$, s₀ parçanın ön istasyonu, L uzunluğu. C_Nα sıfır açı çevresinde normal kuvvet katsayısının radyan başına eğimidir. CP burada bu eğimin uygulama noktasıdır; her açıda ve her hareket için tek bir evrensel basınç merkezi anlamına gelmez.

| Parça | Kodun kullandığı bağıntı | Fiziksel gerekçe |
|---|---|---|
| Konik burun | $r_0=0,\quad C_{N\alpha}=\frac{2A(r_1)}{S},\quad s_{CP}=s_0+\frac{2L}{3}$ | Kesit artışı normal kuvvet oluşturur; alan değişiminin birinci momenti uygulama noktasını verir. |
| Silindirik gövde | $r_0=r_1,\quad C_{N\alpha}=0$; CP boş | Doğrusal ince gövde teorisinde sabit kesitin normal eğimi yoktur. Gövde yine sürtünme yükü taşır. |
| Konik transition ve boattail | $C_{N\alpha}=\frac{2[A(r_1)-A(r_0)]}{S},\quad s_{CP}=s_0+\frac{L(r_0+2r_1)}{3(r_0+r_1)}$ | Daralan parçada işaret negatiftir. Mutlak değer almak toplam araç CP'sini yanlış yere taşır. |
| Trapez kanatçık takımı | Aşağıdaki kanat eğimi ve veter üstündeki CP | Sonlu açıklık, süpürme ve gövde etkisi vardır; dönel simetrik gövde formülü burada kullanılamaz. |

### 2.C Kanatçık takımı

N aynı ve eşit aralıklı kantsız kanatçık sayısı (N≥3), b gövdeden dışarı açıklık, cᵣ/cₜ kök/uç veteri, Δ uç hücum kenarının köke göre geri kayması, r montaj yarıçapı, t kalınlık. Bir kanadın alanı $A_f=\frac{b(c_r+c_t)}{2}$; orta veter çizgisi $\ell_h=\sqrt{b^2+\left(\Delta+\frac{c_t-c_r}{2}\right)^2}$; ortalama aerodinamik veter $\bar c=\frac{2(c_r^2+c_rc_t+c_t^2)}{3(c_r+c_t)}$. Gövde etkileşimi $K=1+\frac{r}{r+b}$.

- **Subsonik eğim:** 

$$
C_{N\alpha}=K\frac{4N(b/d)^2}{1+\sqrt{1+(1-M^2)\left(\frac{2\ell_h}{c_r+c_t}\right)^2}}
$$

 Aynı kanat sıkıştırılabilir subsonik akışta farklı eğim üretir.
- **Süpersonik eğim:** 

$$
C_{N\alpha}=K\frac{N}{2}\frac{A_f}{S}\frac{4}{\sqrt{M^2-1}}
$$

 İnce plakanın iki yüzündeki doğrusal basınç farkı $4/\sqrt{M^2-1}$ terimini, simetrik takımın izdüşümü N/2 çarpanını verir. Kanat uçlarındaki üç boyutlu Mach konileri ayrıca çözülmez.
- **Düşük hızda CP:** 

$$
x_0=\frac{\Delta(c_r+2c_t)}{3(c_r+c_t)}+\frac{1}{6}\left(c_r+c_t-\frac{c_rc_t}{c_r+c_t}\right)
$$

 Bu uzaklık kök hücum kenarına göredir.
- **Hızla CP hareketi:** $s_{CP}=s_0+x_0+(f-0.25)\bar c$. M≤0,5 için f=0,25. M≥2 için $k=\frac{2b^2}{A_f}\sqrt{M^2-1},\quad f=\frac{k-0.67}{2k-1}$. 0,5<M<2 arasında f, 0,25 ile f(2) arasında yukarıdaki kübik ağırlıkla harmanlanır. Kod bu ampirik bağıntıya girerken k>1 ister. CP'nin hareketi CG kolunu, dolayısıyla aynı kuvvette bile Cm'yi değiştirir.

Bu bağıntılar küçük açılar için bir takım hesabıdır. Yüzey hızına ω×r eklenmez; tekil kanat sapması, kant ve roll sönümü geometri yolunda yoktur. [OpenRocket teknik dokümanı, bölüm 3.2](https://openrocket.sourceforge.net/techdoc.pdf)

### 2.D Sürükleme bütçesi: hangi yüzey neyi öder?

Kod önce alan boyutunda bir basınç katkısı B [m²] toplar, ardından $C_A=\frac{B+C_f A_{wet}}{S}$ hesaplar. Böylece farklı boyuttaki parçalar aynı araç alanında karşılaştırılır. C_A gövde ekseni boyunca dirençtir; CD ise aşağıda hesaplanan hava hızı doğrultusundaki izdüşümdür.

**Yüzey sürtünmesi.** $\mathrm{Re}=\frac{\rho V\ell}{\mu}$. Dönel gövdede ℓ=L ve $A_{wet}=\pi(r_0+r_1)\sqrt{L^2+(r_1-r_0)^2}$. Kanatçıkta ℓ=c̄ ve etkin alan $Nb(c_r+c_t)\left(1+\frac{2t}{\bar c}\right)$. Son çarpan kalınlık düzeltmesidir. Rüzgâra maruz alan ve viskozite bu nedenle ayrı girdilerdir. CSV'de geometri yolunun bilgi amaçlı Reynolds sayısı her parça için p.length ile yazılır; kanatçık sürtünmesinde kullanılan c̄ ile aynı olmayabilir.

- Laminer ortalama katsayı $C_{f,l}=\frac{1.328}{\sqrt{\mathrm{Re}}}$.
- Türbülanslı ortalama katsayı $C_{f,t}=\frac{0.455}{(\log_{10}\mathrm{Re})^{2.58}(1+0.144M^2)^{0.65}}$.
- Re≤300 000 ise laminer değer; Re≥600 000 ise türbülanslı değer alınır. Arada $b_R=\frac{\ln(\mathrm{Re}/300000)}{\ln 2}$ ile doğrusal harman yapılır. V=0'da bölme yapılmaz ve fiziksel yük sıfırdır. Bu eşikler pürüzlülük, yüzey birleşimi veya ölçülmüş geçiş noktası modeli değildir.

**Konik burun ve genişleyen transition.** $\varepsilon=\arctan\left(\frac{r_1-r_0}{L}\right),\quad B=C_p[A(r_1)-A(r_0)]$. Subsonikte $C_p=0.8\sin^2\varepsilon$. Süpersonikte $C_p=2.1\sin^2\varepsilon+\frac{0.5\sin\varepsilon}{\sqrt{M^2-1}}$. Bunlar düşük hızdaki şekil kaybı ile konik dalga sürüklemesini temsil eder. İki ara harman 0,8→1 ve 1→1,3 uygulanır; M=1 referansı sinε'dir. Transition için aynı katsayının alan farkına uygulanması bir yaklaşımdır; öndeki parçanın şok alanını çözmez. Ogive/Haack burun desteklenmez. [OpenRocket teknik dokümanı, Ek B](https://openrocket.sourceforge.net/techdoc.pdf)

**Kanatçık kenarları.** Kare kesitli ön ve arka kenar varsayılır. $B=Ntb\left[\frac{C_{stag}}{1+(\Delta/b)^2}+C_{base}\right]$. İlk terim süpürülmüş ön kenarın basıncını, ikincisi arka kenar kaybını temsil eder. $C_{stag}=\frac{\Pi-1}{0.7M^2}$. M≤1 için $\Pi=(1+0.2M^2)^{3.5}$; M>1 için $\Pi=(1.2M^2)^{3.5}\left(\frac{2.4}{2.8M^2-0.4}\right)^{2.5}$. Süpersonik dal normal şok sonrası durma basıncını, subsonik dal izentropik durmayı kullanır. M<10⁻⁴ için iptal hatasını önlemek üzere C_stag=1 alınır. Yuvarlatılmış veya profilli kanat için veri gerekir.

**Açık arka taban.** Subsonikte $C_{base}=0.12+0.13M^2$, süpersonikte $\frac{0.25}{M}$, arada rejim harmanı. $B_{base}=C_{base}(A_{disk}-A_{motor,eff})$; etkin motor alanı yalnız yanarken çıkarılır. Bu düz disk kaybı tek bir parçaya atanır. Boattail'in eğimli yan yüzeyinin basınç kaybı ayrıca hesaba girer. Aynı diski iki parçaya vermek sürüklemeyi iki kez sayardı. [OpenRocket teknik dokümanı, bölüm 3.4](https://openrocket.sourceforge.net/techdoc.pdf)

### 2.E Boattail: önceki parça, yerel giriş ve genişleme

**Önce geometri.** Kod, boattail'in ön istasyonunda biten ve yarıçapı oraya uyan dış gövde parçasını arar; iki eşleşme belirsizlik hatasıdır. Her iki karşılaştırma toleransı 10⁻⁷ m'dir. $\delta=\theta_{in}-\theta_{tail},\quad\theta=\operatorname{atan2}(r_1-r_0,L)$. Komşu bulunamazsa θ_giriş=0 alınır ve sonuç etiketlenir. M>0,8 için kod 0≤δ≤20° ister. Bu sayısal zarf akışın ayrılmayacağını garanti etmez.

**Subsonik dal.** $\lambda=\frac{L}{2(r_0-r_1)},\quad k_\lambda=\max\left(0,\min\left(1,\frac{3-\lambda}{2}\right)\right)$. $B_{tail}=[A(r_0)-A(r_1)](0.12+0.13M^2)k_\lambda$. Uzun, yumuşak daralmaya ve kısa omuza aynı kaybı vermemek için bu ampirik düzeltme vardır.

**Süpersonik dal.** Düzlemsel, yapışık, izentropik genişleme yaklaşımı kullanılır. $\gamma=1.4,\quad b_M=\sqrt{M^2-1}$. Prandtl–Meyer fonksiyonu $\nu(M)=\sqrt{6}\arctan\left(\frac{b_M}{\sqrt{6}}\right)-\arctan b_M$. Çıkış için $\nu(M_2)=\nu(M_1)+\delta$ çözülür; üst sınır genişletilir, sonra 42 ikiye bölme adımı uygulanır. $\frac{p_2}{p_1}=\left(\frac{1+0.2M_1^2}{1+0.2M_2^2}\right)^{3.5}$. Süpersonik dönüşte statik basıncın azalması sürüklemeye katkı verir. Denklem geçişte doğrudan M=1'de kullanılmaz. [NASA Glenn, centered expansion fan](https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/centered-expansion-fan/)

**Verinin kaynağı sonucu değiştirir.** Serbest akım yaklaşımında katsayı $e=\frac{1-p_2/p_1}{0.7M_1^2}$ olup M₁ serbest akımdan gelir. Yerel girişte önce yerel $q_1=0.7p_1M_1^2$ ile duvar basıncı bulunur; sonra $e_{local}=\frac{p_\infty-p_{wall}}{Q_\infty}$ hesaplanır. Böylece önceki parçanın basınç artışı korunur ve katsayı tüm araçla aynı Q'ya göre normalize edilir. Sonuç $B_{tail}=e[A(r_0)-A(r_1)]$. M>1,2 iken parçaya ait geçerli bir yerel değer varsa 1,2–1,3 arasında ona kademeli geçilir; M≥1,3'te tamamen kullanılır.

| Çıktı etiketi | Nasıl okunmalı? |
|---|---|
| subsonic_empirical | Ampirik düşük hız kaybı. |
| freestream_proxy | Komşunun çıkış eğimi bulundu. Dalın yüksek hız tarafı serbest akım girişine dayanır. |
| axial_freestream_proxy | Önceki yüzey yok; giriş akışına sıfır eğim verildi. |
| local_blend / local_supersonic | Sırasıyla yerel veriye geçiş / yerel süpersonik giriş. Eklenen _axial_proxy soneki eğimin hâlâ varsayıldığını bildirir. |

**Somut kontrol.** Ön transition 0,12 m boyunca 0,07→0,08 m yarıçapa genişliyor; boattail sonraki 0,12 m'de 0,08→0,05 m daralıyor. $\delta=\arctan\frac{0.01}{0.12}+\arctan\frac{0.03}{0.12}\approx18.80^\circ$. Ön parça aynı çapta silindir olsaydı δ≈14,04° olurdu. Bu fark yalnızca ilk eğimi eklemektir. Yerel Mach ve basınç sağlanmadığı sürece öndeki sıkıştırma dalgası hesaplanmış sayılmaz.

### 2.F Parçadan kuvvet ve momente: ürün ve karşılaştırma yolları

**Geometri yolu.** $\hat{\mathbf e}=\frac{\mathbf v_a}{V},\quad\mathbf e_L=(\sin\alpha,0,-\cos\alpha)$. Her parça için gövde eksenindeki kuvvet katsayı vektörü $\mathbf c_F=(-C_A,-C_{N\alpha}\hat e_y,-C_{N\alpha}\hat e_z)$. $\mathbf F_i=QS\mathbf c_F,\quad C_{D,i}=-\mathbf c_F\cdot\hat{\mathbf e},\quad C_{L,i}=\mathbf c_F\cdot\mathbf e_L$. Momenti normalize etmek için $\mathbf c_M=\frac{(s_{CG}-s_{CP},0,0)\times\mathbf c_F}{d},\quad C_{m,i}=c_{M,y},\quad\mathbf M_i=QSd\mathbf c_M$. CG'ye göre verilen motor momenti ayrıca eklenir. Simetrik gövdenin eksenel yükü tek başına yunuslama üretmez. Gövde CP'si tanımsız olduğunda eksenel kuvvet yine hesaplanır.

**Tablo yolu.** Arşiv çalışmaları veya uzman katsayıları için bir yüzeyin kendi S_i,d_i ve moment referansı vardır. Komşu Mach düğümleri arasında $f=\frac{M-M_0}{M_1-M_0},\quad C=C_0+f(C_1-C_0)$. Tablo dışında hata verilir. $Q_i=QS_i,\quad K_i=\frac{\rho VS_id_i}{4}$ tanımlarıyla kodun boyutlu yükleri aşağıdadır.

| Büyüklük | Hesap | İşlev |
|---|---|---|
| D | $Q_i[C_{D,off/on}+C_{D,a2}(\alpha^2+\beta^2)]$ | Yanma durumunu ve geliş açısına bağlı kaybı seçer. |
| L, Y | $L=Q_i C_{L\alpha}\alpha+K_i C_{Lq}q,\quad Y=Q_i C_{Y\beta}\beta$ | Statik kaldırma/yanal yük ve hız kaynaklı kaldırma. |
| Serbest roll, pitch, yaw momentleri | $d_i(Q_iC_{l0}+K_iC_{lp}p),\quad d_i(Q_iC_{m\alpha}\alpha+K_iC_{mq}q),\quad d_i(Q_iC_{n\beta}\beta+K_iC_{nr}r)$ | Sabit asimetri, açıyla değişen moment ve açısal sönüm. |
| Gövde yükleri | $\mathbf F_i=-D\mathbf e_D+Y\mathbf e_Y+L\mathbf e_L,\quad\mathbf M_{CG,i}=\mathbf M_{ref,i}+(\mathbf r_{ref}-\mathbf r_{CG})\times\mathbf F_i$ | Farklı noktalardaki yükleri aynı referansa taşır. |

K_i, Q_i d_i/(2V) ile cebirsel olarak aynıdır; kod V'ye bölmeden hesaplar. Bu tablonun sönüm özellikleri geometri moduna kendiliğinden aktarılmaz. Gerçek kullanıcı tasarımından gelen yol ilkidir; ikinci yol elle katsayı gerektiren karşılaştırma seçeneğidir.



### 2.4 Kararlılık ne zaman söylenebilir?

Burundan kuyruğa artan s istasyonuyla statik marj $\mathrm{SM}=\frac{s_{CP}-s_{CG}}{d}$’dir. Küçük açı, simetrik araç ve pozitif normal kuvvet eğimi koşullarında CP’nin CG’nin gerisinde olması bozulan hücum açısını azaltan bir moment oluşturur. Ortak referans alanına dönüştürülmüş bileşen eğimleri için $s_{CP}=\frac{\sum_i s_i C_{N\alpha,i}}{\sum_i C_{N\alpha,i}}$. Payda sıfıra yaklaşırsa bu temsil kullanışsızlaşır; doğrudan moment türevi değerlendirilir. Aynı eksen ve referanslarla $C_{m\alpha}\approx-\mathrm{SM}\,C_{N\alpha}$ olur. Bu ilişki CP’nin küçük açıdaki normal kuvvet türevinden tanımlandığı durum içindir. [NASA Student Launch, bileşenlerin normal kuvvet katkıları](https://www.nasa.gov/wp-content/uploads/2023/09/nasa-sl-2024-arw-rocketry-by-the-numbers-508.pdf)

Statik marj başlangıçtaki moment yönünü açıklar. Salınımın sönüp sönmediğini görmek için C_mq, I_y, hız ve öteleme ile dönmenin bağlaşımı da gerekir. Testte önce q=0 iken küçük bir pozitif α verip geri çağırıcı momenti ölç; ardından küçük bir q bozuntusu verip salınım zarfını izle. Kademe ayrıldığında hem CG hem aerodinamik geometri değişebildiğinden bu değerlendirme tekrarlanır.

**Otomatik girişler:** aktif tasarımın boyutları ve parça yerleşimi, motor çalışma bilgisi, kütle ve CG. Referans veriler yalnız doğrulama veya uzman geçersiz kılma için gerekir.

## 3. DoF Modeli

**Fiziksel soru.** Yükler verildiğinde roket nereye gider ve nasıl döner? Serbestlik derecesi bağımsız konum ve yönelim koordinatlarının sayısıdır. Hızlar da çözümde tutulduğundan durum değişkenlerinin sayısı daha fazladır. Her iki modda roketin boyutları, kütle merkezi, dönme ataleti ve aerodinamik momenti vardır.

| | 3DOF · düzlemsel rijit gövde | 6DOF · uzayda rijit gövde |
|---|---|---|
| Koordinatlar | Yatay yer değiştirme, düşey yer değiştirme, yunuslama | Üç yer değiştirme ve üç dönme |
| Aerodinamik cevap | Sürükleme, kaldırma ve yunuslama momenti | Üç kuvvet ve üç moment bileşeni |
| Kullanılabileceği durum | Başlangıç, rüzgâr, itki ve yükler aynı düzlemle uyumlu | Yanal rüzgâr, yuvarlanma, sapma, asimetri |
| Arşivle ilişki | Aşağıdaki indirgeme yeni tasarım seçimidir | Temel akış SID 215, 313 ve 337’de vardır |

### 3.1 Altı serbestlik derecesinin türetilmesi · DYN-6

Newton’un öteleme yasası yer eksenlerinde yazıldığında $m\mathbf a_N=\sum\mathbf F_N$ olur. Dönen gövde eksenlerinde aynı vektörün türevi ek bir ω×v_B terimi içerir. Bu nedenle aşağıdaki eksi işaretli terim bir koordinat dönüşümü etkisidir. Dönmede başlangıç noktası açısal momentum $\mathbf H_B=\mathbf I\boldsymbol{\omega}$’dır. Sabit I ile gövde türevi Iω̇, yerden görülen ek terim ω×H_B’dir. [MathWorks, 6DOF ve kütle/atalet varsayımları](https://www.mathworks.com/help/aeroblks/6dofquaternion.html)



$$
\dot{\mathbf r}_N=R\mathbf v_B,\quad \dot{\mathbf v}_B=\frac{\mathbf F_{net,B}}{m}-\boldsymbol{\omega}\times\mathbf v_B
$$

 Sabit ataletli fazda $\mathbf I\dot{\boldsymbol{\omega}}=\mathbf M_{CG}+\mathbf M_T-\boldsymbol{\omega}\times(\mathbf I\boldsymbol{\omega})$. Asal eksenlerde örneğin $I_y\dot q=M_y+(I_z-I_x)rp$. Kaynaktaki Euler açı kinematiği: $\dot\phi=p+(q\sin\phi+r\cos\phi)\tan\theta,\quad\dot\theta=q\cos\phi-r\sin\phi,\quad\dot\psi=\frac{q\sin\phi+r\cos\phi}{\cos\theta}$. 85° başlangıçta düşeye yakınlık önemlidir. C++ uygulamasında kuaterniyon k, Hamilton skaler önce, $\dot{\mathbf k}=\frac{1}{2}\mathbf k\otimes(0,\boldsymbol{\omega})$ ve birim norm ile aynı R üretilir. Euler yalnız kayıt/karşılaştırma için kullanılabilir. Yanma fazında I(t), egzozun açısal momentum akısı ve itki momenti için dinamik modülün ayrıca belirlediği varsayımlar kaydedilir. Sadece I(t) değiştirmek genel değişken kütle denklemini doğrulamaz.

Asal eksenlerde diğer iki bileşen $I_x\dot p=M_x+(I_y-I_z)qr,\quad I_z\dot r=M_z+(I_x-I_y)pq$ olur. Burada M toplam dış momenti gösterir. Kütle merkezi etrafında yazıldığından yerçekimi öteleme denklemine girer. Gövdeden yere kuaterniyon sözleşmemiz, MathWorks blok çıkışındaki yerden gövdeye dönüşümün tersidir. Entegrasyonda norm ve dönüşüm yönünü ayrı test et; 90° civarında kuaterniyon düzgün kalırken Euler çıktılarındaki iki açının tek tek yorumu belirsizleşebilir. [MathWorks, kuaterniyon ve dönüşüm yönü](https://www.mathworks.com/help/aeroblks/6dofquaternion.html)

### 3.2 Üç serbestlik derecesine indirgeme · DYN-3

Serbest koordinatlar yatay x, aşağı z ve yunuslama θ. Durum (x,z,θ,u,w,q); m, I_y, CG, α, L, D ve M_y korunur. Simulink 6DOF denklemlerinden v=p=r=φ=ψ=β=0 ve düzlem dışı yükler sıfır alınır. Girdiler uçuş düzlemiyle uyumlu olmalıdır. Yan rüzgâr veya roll/yaw momenti varsa bu modun uygulanabilirlik ihlalini raporla.

- $\dot x=u\cos\theta+w\sin\theta,\quad\dot z=-u\sin\theta+w\cos\theta,\quad\dot\theta=q$
- $\dot u=\frac{F_{net,x}}{m}-qw,\quad\dot w=\frac{F_{net,z}}{m}+qu$
- Sabit ataletli fazda $\dot q=\frac{M_{a,y}+M_{T,y}}{I_y}$. Hava hızından α hesaplanır; gövde yönü hız yönüne kilitlenmez. İtki ve ağırlık F_net içinde taşınır.

θ burnun yataya göre açısıdır; α burnun hava hızına göre açısıdır. Roketin burnu dönerken hız doğrultusunun anında aynı miktarda dönmesi gerekmez. Bu ayrım düzlemsel modelde de kaldırmanın ve yunuslama sönümünün hesaplanabilmesini sağlar. Düzlem, hücum açısını ve yunuslamayı serbest bırakır. Örneğin başlangıçta θ=0, w=q=0 ve yukarı yönlü yük yoksa ẇ=g pozitif çıkar; gövde z eksenimiz aşağı yönlüdür. [MathWorks, düzlemsel gövde denklemleri](https://www.mathworks.com/help/aeroblks/3dofbodyaxes.html)

### 3.3 Yanma, atalet değişimi ve kademe olayı · STAGE

Arşivde sept=20 s. Model bu saatte motor, atalet ve başka girdiler arasında geçiş yapıyor. C++ çözücüsü olayın sol ve sağ limitlerini kaydeder; aktif kademe, zaman başlangıcı, m, CG, I, S, uzunluklar ve katsayı kümesi birlikte değişmeli. Ayrılma impulsu veya açısal hız sıçraması için fiziksel veri aranır. Olayı geçen integrasyon adımı bölünür. Ray ve kurtarma ayrı uçuş fazlarıdır.

Kaynak kütle bloğu tüketimi $\dot m=-\frac{T}{I_{sp}g_0}$ biçiminde hesaplar. Burada T itki [N], I_sp özgül itki [s]. Bu denklem verilen motor eğrisiyle tutarlı etkin I_sp varsayar. Arşivde iki dosya aynı adla farklı inertia ve dk4motor serileri içerir. İzin verilen veri kaynağı her kademe için dosya ve değişken adıyla seçilmelidir.

Sürekli kütle değişiminde $\frac{d(\mathbf I\boldsymbol{\omega})}{dt}=\mathbf I\dot{\boldsymbol{\omega}}+\dot{\mathbf I}\boldsymbol{\omega}$’dır. Açık bir sistemde çıkan kütle de momentum taşır. Bu yüzden tutulacak terimler, itkinin momentum akısını zaten içerip içermediğiyle birlikte belirlenir. Örneğin net etkin momentin tanımlandığı ve ek açısal momentum akısının sıfır alındığı düzlemsel değişken atalet modelinde $\dot q=\frac{M_y-\dot I_y q}{I_y}$ olur. Bu özel denklem tüm motor düzenleri için bir kapanış sağlamaz. Özellikle egzoz etkisini hem ölçülmüş itki hem ek akı olarak saymak hatalıdır. [MathWorks, değişken kütleli gövde modelleri](https://www.mathworks.com/help/pdf_doc/aeroblks/aeroblks.pdf)

Kademe değişiminin sayısal testi aynı fiziksel noktayı izlemelidir. Konumu CG üzerinden kaydediyorsak yeni kademenin CG’si ayrılma anında başka bir noktadadır. İmpuls yokken aynı gövde üzerindeki iki nokta için $\mathbf v_2=\mathbf v_1+\boldsymbol{\omega}\times(\mathbf r_2-\mathbf r_1)$ bağıntısı geçerlidir; konumdaki referans değişimini fiziksel sıçrama sanmamak gerekir. Ayrılma mekanizması impuls veriyorsa onun doğrusal ve açısal etkisi ayrıca modellenir.

### 3.4 Bir zaman adımını baştan sona oku

Durgun havada, deniz seviyesinde ρ=1.225 kg/m³ ve a≈340.3 m/s olsun. Sentetik bir noktada V=100 m/s, S=0.02 m², d=0.2 m, C_D=0.5, C_m=−0.1 ve moment referansı CG olsun. Atmosferden çıkan değerlerle Mach≈0.294, Q=6125 Pa bulunur. Kuvvet modeli D=61.25 N ve M_y=−2.45 N·m üretir. I_y=4 kg·m², diğer açısal hızlar ve itki momenti sıfırsa DoF modeli q̇=−0.6125 rad/s² verir. Bu, o andaki açısal ivmedir. Bir sonraki değerlendirmede yeni yönelim α’yı değiştirir ve yükler yeniden hesaplanır.

### 3.5 Kabul ve doğrulama

| Uyarım | Beklenen |
|---|---|
| Durgun hava, sıfır yer hızı | Q=0, kuvvet ve moment sıfır, sayısal tekillik yok |
| ρ=1.2, V=10, S=0.01, C_D=0.5, α=β=0 | F_x=−0.30 N |
| Q=100 Pa, S=0.02 m², d_i=0.2 m, üç moment katsayısı 0.1, referans CG | her moment 0.04 N m |
| M_y=2 N m, I_y=4 kg m², diğer moment ve açısal hızlar sıfır | iki modda ilk q̇=0.5 rad/s² |
| Aynı düzlemsel başlangıç ve yük geçmişi | 6DOF ile 3DOF ortak durumlarının farkı adım küçültüldüğünde integratör toleransına inmeli |
| Vektör rüzgârı ve yer hızı aynı miktarda artırılır | bağıl akış ve aerodinamik yükler değişmez |
| Saf yan rüzgâr veya kalıcı roll uyarımı | 3DOF kısıt ihlali; 6DOF yanal/dönel cevap üretir |
| Kademe olayının hemen öncesi/sonrası | beklenen veri paketi ve zaman ekseni; pozitif m ve pozitif tanımlı I |

Bu örnekler denklemleri sınar. Araç performansı için referans uçuş verisi, kademe geometrileri ve katsayıların geçerlilik zarfı bekleniyor.

Arşivin kapalı çevrim çıktıları bu ortamda MathWorks hizmet bağlantısı 5201 hatası nedeniyle doğrulanamadı. Simulink ile gelecek karşılaştırmada tek zaman noktasında ρ, V, α, Q, F ve M seviyesinde yapılmalı. Sonra ortak düzlemsel durumda iki çözücünün tüm zaman serileri karşılaştırılmalı. Önerilen sayısal kontrol, aynı fiziksel girdilerle toleransı sıkılaştırarak ortak durum farkının azaldığını göstermektir. Gerçek uçuş doğrulamasında ise zaman eşlenmiş ölçüm, sensör eksenleri, saha koşulları ve belirsizlikler gerekir. Yalnız apojenin tutması, yanlış sürükleme ile yanlış kütlenin birbirini telafi etmesini dışlamaz.


### 3.6 Sayısal integrasyon ve hata denetimi

**Neden dört kez yük hesaplıyoruz?** Roketin hız ve yönelimi değişirken hücum açısı, Mach sayısı ve hatta aktif aerodinamik dal değişebilir. Başlangıç yüklerini bütün adım boyunca sabit tutmak bu geri beslemeyi geciktirir. Durum y için f(t,y), o anda yeniden çağrılan atmosfer, yükler ve dinamik türevleridir.

| RK4 işlemi | Denklem |
|---|---|
| Başlangıç | $k_1=f(t,y)$ |
| Birinci orta nokta | $k_2=f\left(t+\frac h2,y+\frac h2 k_1\right)$ |
| İkinci orta nokta | $k_3=f\left(t+\frac h2,y+\frac h2 k_2\right)$ |
| Son nokta | $k_4=f(t+h,y+h k_3)$ |
| Aday durum | $y'=y+\frac h6(k_1+2k_2+2k_3+k_4)$ |

Her adayda kuaterniyon birim norma döndürülür. Bir tam adım sonucu iki yarım adımla karşılaştırılır. Mutlak fark eşikleri konumda 10⁻⁴ m, hızda 10⁻⁴ m/s, açısal hızda 10⁻⁶ rad/s ve kuaterniyonda 5×10⁻⁷ kiriş uzaklığıdır. k ile −k aynı yönelimi temsil ettiği için karşılaştırmadan önce işaretler eşlenir. Geçişte iki yarım adım kabul edilir; geçmezse aralık ikiye ayrılır. On iki bölme seviyesinde de geçmeyen adım hatayla sonuçlanır. Bu yerel fark, gerçek araç konumunun hata sınırı değildir.

**Fiziksel tutarlılık ayrı sınanır.** R F_net ile R(F_a+T)+mg_N ve R[m(v̇_B+ω×v_B)] aynı olmalıdır. Kuvvet farkı için tolerans 10⁻⁸ N + 10⁻⁹(‖F_a‖+‖T‖+m‖g_N‖). Moment artık vektörü $\mathbf I\dot{\boldsymbol{\omega}}+\dot{\mathbf I}\boldsymbol{\omega}+\boldsymbol{\omega}\times(\mathbf I\boldsymbol{\omega})-\mathbf M_a-\mathbf M_T$ olup sınırı 10⁻⁹ N·m artı yük ölçeğinin 10⁻⁹ katıdır. Bu kontroller her türev çağrısında çalışır. Aynı yanlış katsayının hem yük hem ivme hesabında tutarlı kullanılması bu denetimden geçebilir.

**Motor, kütle ve olay zamanlaması.** Örnek çözücüde kademe yaşı τ, yanma süresi t_b. $f_b=\operatorname{clamp}(\tau/t_b,0,1),\quad m=m_0+f_b(m_1-m_0),\quad\mathbf I=\mathbf I_0+f_b(\mathbf I_1-\mathbf I_0)$. Yanarken $\dot{\mathbf I}=\frac{\mathbf I_1-\mathbf I_0}{t_b}$, sonra sıfır. Yanmasız fazda parametreler sabittir. Harici geometri yolunda kütleyi sağlayıcı verir; paket çözücüsü atalet için halen faz tablosunu kullanır. İtki eğrisini örneklemek kütle eğrisini otomatik türetmez. Sonraki kayıt zamanı adım sonu, yanma bitimi, motor/rüzgâr düğümü ve faz sonu arasındaki en erken zamandır. Geçiş anına kadar RK4 aynı fiziksel dalda kalır. Ayrılmada $\mathbf r_{new}=\mathbf r_{old}+R\Delta\mathbf r_B,\quad\mathbf v_{new}=\mathbf v_{old}+\boldsymbol{\omega}\times\Delta\mathbf r_B$ uygulanır. Aynı küresel zamana iki satır yazılması bu nedenle doğrudur. Ayrılma impulsu, diğer kademenin bağımsız yörüngesi ve paraşüt bu çözücüye dahil değildir.

### 3.7 Doğrulama kaydı ve sonuçların okunması

Rapor hazırlanırken paketle gelen test programı örnek senaryo üzerinde yeniden çalıştırıldı. İki kasıtlı arıza, ağırlığın ve itkinin iki kez eklenmesi, beklendiği gibi kuvvet dengesi hatası verdi. Program bu iki olayı yakaladıktan sonra başarılı çıktı.

| Kanıt | Gözlenen sonuç | Ne gösterir? |
|---|---|---|
| İtki darbesi + süzülme | Her iki mod analitik son konumla uyuştu | İtki yönü, ağırlık, motor kesimi ve eksen dönüşümü birlikte doğru uygulanıyor. |
| Adım yarılama | Tablo yolunun test senaryosunda son konum farkı 5,92091×10⁻⁹ m | Bu özel koşuda zaman adımı çözümü yakınsamış. |
| Sabit açılı Mach taraması | 301 nokta; 0,5/0,8/1/1,3/2 eşiklerinin iki tarafında CP, CD, CL, Cm kontrolleri geçti | Programdaki dallar değer sürekliliğini sağlıyor. |
| Bilinen referans değerleri | Mach 2 taban katsayısı 0,125; ν(2)≈26,37976° kontrol edildi | Ampirik taban bağıntısı ve genişleme fonksiyonu sayısal referansı üretiyor. |
| Parça, CG ve dünya modeli | Konik CP, boş silindirik CP, boattail eğim işareti, motor açık taban alanı, CG aktarımı ve harici g testleri geçti | Arayüzlerde işaret ve referans kuralları korunuyor. |
| Boattail yerel giriş | 18,80° ve 14,04° geometrileri, yerel basınç normalizasyonu ve proxy etiketi kontrol edildi | Yerel veri yolu ayrı sentetik testle doğrulanmıştır. |

**Bir kaydı okuyalım.** Mevcut örnek 6DOF üç-rejim uçuşunun 1,20 s satırında M=2,03868, araç CD=0,40533, burundan ölçülen CP=0,69586 m. Bunlar sentetik tasarımın sayısal sonuçlarıdır. Aynı zamana ve parça kimliğine ait yüzey satırlarından CD ve Cm doğrudan toplanabilir. CP istasyonları toplanamaz; normal eğimlerle ağırlıklandırılır. Örnek Mach uçuşunda yerel boattail giriş verisi sağlanmamıştır; yüksek hız sonuçları freestream_proxy dalından gelir.

| Çıktı | Okunacak alanlar | Yaygın yorum hatası |
|---|---|---|
| Uçuş CSV | t_s, phase, Mach, Q_Pa, regime, CP_m, CD, CL, CmCG, konum, hız ve kuaterniyon | down_m pozitif aşağıdır; irtifa saha rakımı eksi bu değerdir. |
| Kuvvet bileşenleri | Fx/Fy/Fz aerodinamik; Tx/Ty/Tz itki; Wx/Wy/Wz ağırlık; FnetX/Y/Z bileşke | Fnet'e yeniden ağırlık eklemek yanlıştır. |
| Momentler | Uçuş CSV Mx/My/Mz aerodinamik CG momentleri; API netMoment ayrıca motor momentini içerir | İki momenti aynı alan gibi okumamak gerekir. |
| Yüzey CSV | Kimlik, Re, yükler, CP_m, CD, CL, CmCG, tail_turn_deg ve tail_inlet_method | Boş CP, sıfır metre anlamına gelmez. Boattail etiketi girişin kaynağını gösterir. |

**Gerçek uçuşa geçiş için doğrulama sırası.** Önce aynı geometri, referans alanı, CG, eksen ve Mach ile tek parça yükleri karşılaştırılmalı. Sonra bağımsız araç CD(M), CP(M), moment ve sönüm verileriyle birleşik model ölçülmeli. En son zaman eşlenmiş uçuş verisiyle ivme, açısal hız, hız ve irtifa karşılaştırılmalı. Arşivden alınan blok bağlantıları, deneysel araç doğrulaması yerine geçmez. Bu raporla birlikte gerçek araç için bir hata yüzdesi beyan edilmiyor.


## Uygulama ve izlenebilirlik

Parametre ve bağlantı sözleşmeleri [entegrasyon belgesinde](ucus_cpp/ENTEGRASYON.md), etkin modelin kodu [geometri](ucus_cpp/geometry.hpp) ve [dinamik çözücü](ucus_cpp/flight.hpp) dosyalarında, tekrar çalıştırılabilir kontroller [test dosyasında](ucus_cpp/tests.cpp) yer alır. [Simulink incelemesi](SIMULINK_INCELEME.md) arşiv kanıtını ve karşı hipotezleri içerir. Örnek veriler [uçuş CSV](Mach_Ucus.csv) ve [bileşen CSV](Mach_Parcalar.csv) olarak okunabilir.

Paket klasöründe kontroller.exe ornek.txt ile kontroller, ucus.exe --mach-flight 6 ucus.csv parcalar.csv ile üç rejimli örnek çalıştırılır. 6 yerine 3 verildiğinde aynı tasarım düzlemsel çözücüyle değerlendirilir.
