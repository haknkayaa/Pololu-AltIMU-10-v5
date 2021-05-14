 
# Pololu AltIMU-10 v5 Gyro, İvme Ölçer, Pusula ve Yükseklik Sensör Modülü

Bu repository AltIMU-10 kompleks sensör mödülünün çalışmalarını içermektedir. 

İvme ölçer ve Gyro gibi sensörler ile çalışmak zaman ve emek isteyen çalışmalardır.
Ve bazen oldukça sıkıcı olabilmektedir. Bu sensörler ile yapmış olduğum çalışmaları 
burada derli toplu olması açısında açık kaynak olarak geliştiricilere sundum.

Faydalı olması dileğiyle.

### Kalibrasyon

İvme ölçer, gyro gibi sensörler oldukça hassas ve gürültülü çalışan modüllerdir.
Doğrudan kullanımı stabilite sağlayamadığı için çeşitli filtreler uygulanmalıdır.

Kalman filtresi, normalizasyon, ağırlıklı standart sapma gibi örnek 
çalışmaları repo içerisinde
bulabilirsiniz.

Kalibrasyon örneği (Kalman Filter);
![calibration](https://github.com/haknkayaa/Pololu-AltIMU-10-v5/blob/main/images/calibration.jpg)

- Mavi çizgi sensörden okunan işlenmemiş ivme değeri (tek bir eksen)
- Kırmızı çizgi Kalman filtresi sonrası hesaplanan değer (tek bir eksen)

### Görselleştirme

Gerçek zamanlı seri port üzerinden alınan verileri bilgisayar üzerinde 3d görselleştirilmesi
için Processing 3.5.4 kullanılmıştır.

"*visualization*" klasörü içerisinde processing projelerini çalıştırırken COM port seçimlerini
düzenlemeyi unutmayın.

Render ile görselleştirilmiş processing GUI ekran görüntüsü;

![calibration](https://github.com/haknkayaa/Pololu-AltIMU-10-v5/blob/main/images/processing.gif)


### Geliştirme Ortamı
 - MCU: Teensy 3.6 
 - Build, Compiler ve Debugger: CMake 3.19.3, MinGW64 6.0
 - IDE: Jetbrains CLion IDE 2021,
 - FrameWork: PlatformIO Core and CLI
 - Görselleştirme/GUI:  Processing 3.5.4
 - OS: Windows 10 Pro 

### İletişim

[Hakan KAYA](https://www.hakankaya.kim) - mail@hakankaya.kim



