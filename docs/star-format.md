# STARLAB proje ve girdi dosyaları

## `.star` proje dosyası

UTF-8 JSON belge, `format: "starlab"`, `schemaVersion: 1`, `project` alanı. `project.version` mevcut proje modelinin sürümüdür. `.star` bir ZIP veya yalnız roket geometrisi dosyası değildir.

Proje adı; tüm bileşenler, kimlikler ve ebeveyn/çocuk sırası; ölçüler, kütle, malzeme ve ek girdiler; özel motorların eğrileri; motor atamaları; ateşleme/ayrılma/kurtarma ayarları; senaryolar ve çevre/çözüm ayarları; varsa harici/çözücü sonuçları ve giriş imzaları saklanır. Kullanılan yerleşik örnek motorlar da dosyaya eklenir; örnek veri etiketi korunur. Dışarı aktarma açık projeyi değiştirmez.

Eski düz JSON proje dosyaları okunur. Desteklenmeyen sürüm, geçersiz veri, bozuk bağlantı veya 50 MiB üzerindeki dosya mevcut projeyi değiştirmez. Yerel otomatik kayıt ayrı olarak devam eder; tarayıcı depolama kotası dosya sınırından daha küçük olabilir. Yerel kayıt başarısızsa `.star` dışa aktarma kullanılabilir. Panel oranları ve görünüm tercihleri projeden bağımsız, yerel kullanıcı ayarlarıdır. Undo geçmişi dosyaya yazılmaz; proje açılınca sıfırlanır.

## Kullanıcı girdileri

`Part.geometry` ek parça alanlarını, `Part.materialData` seçilmiş katalog kaydının bağımsız kopyasını saklar. Geometri uzunlukları mm, açıları derece, profil katsayıları boyutsuzdur. Malzeme türüne göre hacimsel/yüzeysel/çizgisel yoğunluk kg/m³, kg/m² ve kg/m'dir; kayma modülü Pa'dır. `inputs.surfaceDensity` ve `inputs.linearDensity` ayrı alanlardır. İşlem paketi bu birimleri açıkça taşır ve kaynakta bilinmeyen kayma modülünü `null` verir. Ayrıntılar [katalog sözleşmesindedir](legacy-catalog.md).

`Part.inputs` isteğe bağlıdır. Tanımlanmayan alan bilinmiyordur; dosyada alanın bulunmaması normaldir. Arayüz boş bırakılan sayıları sıfıra dönüştürmez.

| Alan | Birim / anlam |
| --- | --- |
| `Part.mass` | g; `massProvided: true` açıkça girilen sıfırı da korur. Eski projelerde pozitif kütle verilmiş sayılır, sıfır bilinmiyor sayılır. |
| `massScope` | `component` yalnız seçili bileşen, `subtree` alt bileşenlerle birlikte ölçülmüş kütle. Toplamaya uygulanacak kuralı hesap ekibi belirler. |
| `wallThickness` | mm; gövde/burun/motor yuvasında et kalınlığı, kanat setinde kanat kalınlığı. Şematik 3D katı zarfı henüz et kalınlığını göstermez. |
| `density` | kg/m³, pozitif hacimsel yoğunluk. Alan/uzunluk yoğunluğu henüz yok. |
| `centerOfMass`, `centerOfPressure` | mm; seçili bileşenin yerel başlangıcından +X, kuyruğa doğru. Roket kökünde başlangıç burun ucudur. Kademe/alt sistemde kendi yerel başlangıcıdır. CG/CP alt bileşen kapsamı için ayrı override seçeneği henüz yok. |
| `dragCoefficient` | Boyutsuz Cd, elle girilir. Referans alanı/uzunluğu henüz ayrı alan değildir; entegrasyonda tanımlanmalıdır. |
| `mach`, `angleOfAttack` | Boyutsuz Mach ve derece; CP/Cd girdisinin ait olduğu tek akış koşulu. Boşsa koşul belirtilmemiştir. |
| `roughness` | µm, kullanıcı tarafından girilen yüzey pürüzlülüğü |
| `notes` | En fazla 10000 karakter; veri kaynağı ve açıklama |

Arayüz CG, CP, yoğunluktan kütle veya kararlılık hesabı yapmaz. Kütle ve aero analiz tabloları girilen değerleri gösterir; alt sistem kütleleriyle bileşen kütlelerini toplayıp çift sayım oluşturmaz.

## İşlem ekibine veri aktarımı

Araçlar → Hesap girdilerini dışa aktar, `engine/projectInputs.ts` üzerinden `format: "starlab.inputs"`, `schemaVersion: 1` üretir. `units` tüm sayısal alanların birimlerini; `coordinates` eksen ve referansı belirtir. Parça sayılarında bilinmeyenler açıkça `null`, girilmiş sıfır `0` olur. Proje, motor, konfigürasyon ve senaryo girdileri aktarılır; önceki sonuçlar girdi paketine konmaz. Geometri çiziminden veya görsel yer tutuculardan fizik girdisi türetilmez. Paket bağımsız bir kopyadır.

`createFlightRequest` aynı `PartInput` ve birim sözleşmesini kullanır. Yalnız seçili senaryonun motorlarını ve konfigürasyonunu taşır; mevcut ateşleme/kurtarma varsayılanları açıkça yazılır. Eksik girdilerin hangi modül için zorunlu olduğunu çözücü ekibiyle belirlemek gerekir. Bu paket şu anda çalışan bir çözücü veya tam CAD/üretim modeli değildir.

## Kısayollar

| Kısayol | İşlem |
| --- | --- |
| Ctrl/Cmd Z | Proje değişikliğini geri al |
| Ctrl/Cmd Shift Z veya Ctrl/Cmd Y | İleri al |
| Ctrl/Cmd S | `.star` dosyasını dışa aktar; tarayıcıda indirme başlatır |
| Ctrl/Cmd O | Proje dosyası aç |
| Delete | Seçili bileşeni silme onayı; metin alanında normal silme korunur |
| F | Tasarımda seçime odaklan; boş seçimde bir üst seviyeye çık |

Undo/redo tüm proje sayfalarını kapsar: boyutlar, alanlar, taşıma, ekleme/silme, motorlar, konfigürasyonlar ve senaryolar. Aynı alandaki aralıksız yazım gruplanır; 800 ms ara veya alan değişimi yeni adım açar. Son 80 adım bellekte tutulur. Modal form taslakları kendi metin düzenleme davranışını kullanır; projeye kaydedilince tek adım olur. Kamera ve panel oranı proje değişikliği değildir.

2D'de sürükleme/ölçü tutamaçları birim dönüşümüyle aynı `length`, `diameter`, `offset` alanlarını düzenler; fizik hesabı üretmez. Sürükleme sırasında proje kaydını değiştirmeyen önizleme kullanılır, bırakıldığında tek geçmiş adımı oluşturulur. Hizalama kılavuzları ve touchpad kamera gezinmesi dosyaya yazılmaz. Mevcut parça hiyerarşisi korunur; sürükleme ebeveyn değiştirmez.

Çoklu seçimden oluşturulan alt sistem normal bir `assembly` kaydıdır; kimlikler ve motor/uçuş referansları korunur. Grup geometrisi düzenlenince gerçek bileşen ölçüleri/yerel konumları kaydedilir; görsel bir ölçek matrisi fizik verisinin yerine kullanılmaz. Toplu alan değişikliği açıkça seçilen kayıtlara uygulanır; kademe/alt sistemin geometri tutamacı ise onun fiziksel alt ağacını kapsar. Seçimler, filtre ve odak kullanıcı arayüzü durumudur; dosya şeması değişmez.
