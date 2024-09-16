# ADSM-pink
## Project structure
This project consists of main application [ADSM-pink.c](ADSM-pink.c) and additional libraries responsible for diffrent functionalities of the robot:
 - `Lidar` calculates and checks CRC
 - `LidarTask` handles Lidar functionality
 - `MotorsControl` controls the motors
## Parts list
 - [brushless motors](https://botland.com.pl/silniki-bldc-bezszczotkowe/5188-silnik-bezszczotkowy-z-przekladnia-25dx43l-45-1-ze-sterownikiem-pwm-enkoder-6959420909636.html)
 - [Feather ESP32-S3 microcontroller](https://botland.com.pl/plytki-zgodne-z-arduino-adafruit/21579-feather-esp32-s3-modul-wifi-gpio-4mb-flash-2mb-psram-zgodny-z-arduino-adafruit-5477.html)
 - [Li-ion batteries](https://botland.com.pl/akumulatory-li-ion/23832-ogniwo-18650-li-ion-samsung-inr18650-25r-2500mah-20a.html)
 - [battery holder](https://botland.com.pl/koszyki-na-baterie/16519-koszyk-na-3-akumulatory-typu-18650-bez-przewodow-5904422378165.html)
 - [wheels](https://botland.com.pl/kola-z-oponami/521-kola-42x19mm-2szt-pololu-1090-5904422367282.html)
 - [step down DC converter](https://botland.com.pl/ladowarki-do-akumulatorow-li-ion/13824-ladowarka-do-akumulatorow-18650-xtar-mc2-plus-6952918340744.html)
 - [Lidar sensor](https://botland.com.pl/produkty-wycofane/21729-skaner-laserowy-360-stopni-d300-uart-12m-waveshare-22098.html)
## Connection schematic
Check [Feather ESP32-S3 pinout](https://github.com/adafruit/Adafruit-Feather-ESP32-S3-PCB/blob/main/Adafruit%20Feather%20ESP32-S3%20Pinout.pdf)


![connection schematic](https://cdn.discordapp.com/attachments/1230243707832373341/1285319182266925129/image.png?ex=66e9d645&is=66e884c5&hm=2ecaa8dd79c00f08fa64e0fc3540460b7d932db72064c8b37f41d210a6501edc&)

## Assembled robot "retarded roomba"
![assembled robot](https://cdn.discordapp.com/attachments/1230243707832373341/1285300340232814652/20240916_145805.jpg?ex=66e9c4b8&is=66e87338&hm=a2bc4cd78e1650d431f962bb03dabb57b8ec099162efeb90303eb58a2a56941f&)
