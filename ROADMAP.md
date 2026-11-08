# STARLAB işlev kapsamı

Hedef OpenRocket düzeyinde bir roket tasarım ve analiz uygulamasıdır. Mevcut sürüm tam OpenRocket eşdeğeri değildir. Arayüzde örnek, harici veri ve gerçek hesap birbirinden ayrılır.

| Alan | Mevcut | Sonraki dilimler |
| --- | --- | --- |
| Tasarım | Boş/yeni proje, 28 seçenekli bileşen kütüphanesi, sekiz burun profili, 82 malzeme ve yoğunluk türleri, bağlamsal canvas özeti; kapsama odaklı ağaç/akış ve arama, Shift ile çoklu seçim, ortak özellik düzenleme, seçimden alt sistem, grup taşıma/ölçekleme, yeniden ebeveynleme, ortak 2D/3D, 2D taşıma/boyut tutamaçları, touchpad gezinmesi ve hizaya yapışma, yerel kayıt, et kalınlığı/yoğunluk, elle kütle/CG/CP/Cd, kaynak notu, proje genelinde undo/redo | Omuz/radyal/tekrar ayrıntılarının tam CAD çizimi, ileri kanat kesitleri, kullanıcının kalıcı malzeme kütüphanesi, kapsamlı ölçüm/override seçenekleri |
| Motor | Sentetik örnek katalog, özel motor editörü, RASP `.eng`, önizleme, boyut kontrolü, itki integrali | Gerçek ve kaynaklandırılmış motor kataloğu, `.rse`, gecikme seçenekleri, küme/çoğaltma, motor CG ve yakıt tüketimi |
| Konfigürasyon | Birden çok konfigürasyon, yuva ataması, ateşleme/ayrılma/kurtarma ayarları | Olayların çözücüde çalıştırılması, ayrılan kademelerin bağımsız simülasyonu, çoklu paraşüt aerodinamiği |
| Simülasyon | Senaryolar, saha/ray/atmosfer/rüzgâr ayarları, hesap girdisi ihracı, dış CSV sonuçları | Doğrulanmış uçuş çözücüsü, hava/yer modelleri, rüzgâr profili/türbülans, uçuş olayları, hata/uyarı değerlendirmesi |
| Analiz | Girilen kütle/aero verisi tabloları, geometri, eğri grafikleri, iki uçuş karşılaştırması, CSV | Hesap ekibinin sağlayacağı kuvvet/katsayı/CG/CP/kararlılık çıktıları için sunum, TWR, 3B yörünge/harita, daha çok değişken |
| Uyumluluk | Tam proje verili sürümlü `.star`, eski JSON okuma, birimli ve null destekli işlem girdisi paketi, RASP motorlar, sonuç CSV | `.ork`/`.rkt` proje uyumluluğu, CAD/GLB adaptörü, birim tercihleri, çok dil, masaüstü paketleri |

## Referans alınan akış

Arayüz ekibinin öncelikli eksikleri ve incelenen kaynak kodları: [OpenRocket arayüz incelemesi](docs/openrocket-ui-audit.md). Proje/girdi sözleşmesi: [.star biçimi](docs/star-format.md).

- [OpenRocket başlangıç ve konfigürasyon](https://openrocket.readthedocs.io/en/latest/setup/getting_started.html)
- [İtki eğrileri](https://openrocket.readthedocs.io/en/latest/user_guide/thrust_curves.html)
- [Simülasyon ve grafik kapsamı](https://openrocket.readthedocs.io/en/latest/user_guide/advanced_flight_simulation.html)

Hesap modülleri geldiğinde bağımsız test örnekleri, birim/eksen/olay tanımları ve referans sonuçlarla doğrulanmalıdır. Görsel motor/parça ağları fiziksel hesap modelinin yerine geçmez.
