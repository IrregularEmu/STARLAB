# B3 katalog aktarımı

Kaynak yalnız okunmuştur: `/home/ayberk/Projects/B3/roketsimdeneme1/`.

- `desktop_app/application/services/catalog_service.py`: masaüstü paletindeki 19 kayıt ve kategoriler.
- `B-4 Python/ui/component_schemas.py`: sekiz burun ailesi, ek parça sınıfları ve geometri alanları.
- `B-4 Python/core/component_compatibility.py`: bağlantı türleri.
- `src/malzeme/VeriTabanimalzeme.cpp`: tüm 82 sabit malzeme kaydı. Sayılar kaynaktaki haliyle korunmuştur; yeni fiziksel doğrulama yapılmamıştır. Kaynak dosya veri kümesinin OpenRocket/MatWeb ve diğer referanslardan uyarlandığını belirtir.

## Bileşenler

28 seçenek: kademe, alt sistem, pod grubu, paralel kademe, gövde, geçiş; sekiz burun; trapez/serbest/tüp kanat; motor yuvası, iç tüp, merkezleme halkası, bölme plakası, motor durdurucu, manşon, kütle bileşeni, paraşüt, kurtarma ipi, fırlatma kılavuzu ve ray düğmesi.

Burun aileleri konik, teğet ogiv, sekant ogiv, eliptik, parabolik, üs, Haack ve Von Kármán'dır. Kütle bileşenine eski örneğin kütlesi otomatik verilmez. Gerçek motorlar Motorlar & konfigürasyon ekranından atanır; eski masaüstü paletindeki gibi fiziksel motor kataloğu parça yuvasından ayrıdır.

Kütüphane arama, kategori, önizleme ve uyumlu hedef seçimi sunar. Boş projede gövde/burun gibi parçalar için Yeni kademe hedefi vardır; kademe ve parça tek geri alma adımında oluşur. Kanat gibi parçalar için önce uygun bir gövde gerekir.

Başlangıç ölçüleri mm'dir. `Part.geometry` profil, iç/arka çap, omuz, kanat, açık kurtarma boyutları ve tekrar/radyal yerleşim girdilerini saklar. `length` / `diameter` çizim zarfıdır; paraşüt ve ipin açık boyutları ayrıdır. Serbest kanat kaynak noktaları çizim zarfına ölçeklenir. B3 proje dosyası içe aktarma ve eski hesap motorunu çalıştırma bu aktarımın kapsamında değildir.

Profil/kanat planformları, geçiş ve temel iç tüp/halkalar çizilir. Tüp kanat dizilimi, pod/paralel kademe tekrarları, radyal kılavuz yerleşimi ve omuz ayrıntılarının tam CAD gösterimi henüz yoktur; girdileri kaydedilir, önizleme şematiktir. Çizimler fizik hesabının yerine geçmez.

## Malzemeler

| Tür | Adet | Yoğunluk |
| --- | ---: | --- |
| Hacimsel | 32 | kg/m³ |
| Yüzeysel | 8 | kg/m² |
| Çizgisel | 42 | kg/m |

Paraşütte yüzeysel, ipte çizgisel, diğer fiziksel parçalarda hacimsel katalog gösterilir. Seçim adı, katalog anlık kopyasını ve doğru yoğunluk alanını doldurur; kütle hesaplamaz. Özel malzeme ve elle yoğunluk girişi korunur. Karışık yoğunluk türlerinin çoklu seçiminde ortak malzeme/yoğunluk alanı gösterilmez.

`materialData` anlık kopyası `.star` içine gömülür. Güncel kullanıcı yoğunluğu `inputs.density` / `surfaceDensity` / `linearDensity` alanındadır; kaynak kopya, temizlenen/değiştirilen girdiyi yeniden doldurmak için kullanılmaz. Kaynakta kayma modülü sıfır olan kayıtlar `.star` içinde aynen korunur; işlem paketinde bilinmeyen modül `null` olur. Modülün birimi Pa'dır. Sayısal çözücü bağlı değildir.

## Yeni proje ve özet

Yerel kayıt yoksa yalnız kök kaydı olan boş proje açılır. Dosya → Yeni proje veya Ctrl/Cmd N yeni belge oluşturur. Oluşturma penceresi mevcut projeyi `.star` olarak indirmeyi de sunar. Belge değiştirilirken mevcut belge tek önceki-proje kaydına alınır; Dosya → Önceki projeyi aç geri getirir. Yerel kota hatasında yalnız oturum içi kopya korunur ve bildirim gösterilir. Dosya → Örnek roket aç eski iki kademeli örneği açar. Belge geçişinde undo geçmişi sıfırlanır.

Odak kapsamı fiziksel parça içermiyorsa canvas yalnız ortadaki + düğmesini gösterir. Sağ alt köşede tüm roket/boş seçim için çizim uzunluğu ve sayılar, seçim/alt sistem için kısa ad veya açılabilir liste vardır. Kütle toplanmaz veya malzemeden türetilmez: kökte elle girilmiş kütle varsa Girilen kütle, yoksa — gösterilir. Boş canvas'ta özet görünmez.
