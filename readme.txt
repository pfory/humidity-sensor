Vlhkom�r
--------
Za��zen� slou�� k m��en� vlhkosti a teploty a odes�l�n� hodnot na server xively.com. K p�enosu vyu��v� bezdr�tov� p�ipojen� nap�. k dom�c� s�ti. 
Konfigurace s�ov�ch adres, xively kl��� a intervalu m��en� je u�ivatelsky nastaviteln�.

P�ep�na�e
Pohled ze strany sou��stek 
---------
| O     |   P�ep�na� 3
| O     |   P�ep�na� 2
| O     |   P�ep�na� 1
---------
 ON  OFF
 x - libovoln� poloha p�ep�na�e
 

Za��zen� se m��e nach�zet ve �ty�ech re�imech podle nastaven� p�ep�na�� 1,2 a 3.
            | 2         1           3
------------|------------------------------            
flash       | x         x           OFF
nastaven�   | ON        ON          x
test m��en� | OFF       ON          x
m��en�      | OFF       OFF         ON


1. Flash FW
-----------
P�ep�na� 1 je v libovoln� poloze, P�ep�na� 2 je v libovoln� poloze, P�ep�na� 3 je v poloze OFF.

---------
|     O |   P�ep�na� 3
| x   x |   P�ep�na� 2
| x   x |   P�ep�na� 1
---------
 ON  OFF

Za��zen� mus� b�t p�ipojen� k nap�jen�, nap�jen� z USB je kv�li spot�eb� v re�imu sp�nku vy�azen� natrvalo z �innosti.

2.Nastaven�
-----------
P�ep�na� 1 je v poloze ON, P�ep�na� 2 je v poloze ON, P�ep�na� 3 je v libovoln� poloze.

---------
| x   x |   P�ep�na� 3
| O     |   P�ep�na� 2
| O     |   P�ep�na� 1
---------
 ON  OFF

Za��zen� se p�epne do re�imu AP (p��stupov� bod). V seznamu Wifi s�t� na PC se zobraz� nov� p��stupov� bod s n�zvem ESPHum, heslo pro p�ipojen� je "humidity007" bez uvozovek. 
Po zadan� adresy 192.168.4.1 v prohl�e�i se zobraz� formul�� pro nastaven� s�t�:
AP SSID - identifika�n� znak s�t� ke kter� se bude za��zen� p�ipojovat. Fusal2
AP PASSWORD - heslo k s�ti 1860918609
IP - adresa za��zen� nap�. 192.168.1.200. Adresa mus� b�t v rozsahu s�t�, ke kter� se p�ipojujeme. 192.168.0.93
Maska - s�ov� maska nap�. 255.255.255.0
Gateway - br�na nap�. 192.168.1.1, p��stupov� bod do internetu. 192.168.0.1
Prodleva mezi m��en�mi - prodleva v sekund�ch mezi jednotliv�mi m��en�mi. Doporu�uji nastavit v�ce ne� 300s kv�li v�dr�i bateri�.

P�ed ulo�en�m p�epnout P�ep�na� 2 (re�im) do polohy ON (m��en�).
Nastaven� se ulo�� a za��zen� se restartuje. Za��zen� je d�le v re�imu STA (stanice). 

3.Test M��en�
-------------
P�ep�na� 1 je v poloze ON, P�ep�na� 2 je v poloze OFF, P�ep�na� 3 je v libovoln� poloze.
---------
| x   x |   P�ep�na� 3
|     O |   P�ep�na� 2
| O     |   P�ep�na� 1
---------
 ON  OFF

Po zad�n� adresy IP za��zen� (nastaven� v bod� 1.Nastaven�) do prohl�e�e se zobraz� formul�� ve kter�m se zobrazuj� nam��en� �daje. 
Perioda m��en� je v tomto p��pad� 10s.

Nam��en� �daje:
Teplota �C
Vlhkost %Rh
Nap�t� baterie V

4.M��en�
--------
P�ep�na� 1 je v poloze OFF, P�ep�na� 2 v poloze OFF, P�ep�na� 3 je v poloze ON.
---------
| O     |   P�ep�na� 3
|     O |   P�ep�na� 2
|     O |   P�ep�na� 1
---------
 ON  OFF

Za��zen� provede m��en�, v�sledky ode�le na Xively a upadne do re�imu sp�nku s minim�ln� spot�ebou kolem 0,1mA. Po uplynut� nastaven� prodlevy se cel� cyklus opakuje.
V tomto p��pad� nen� mo�n� zobrazit webov� formul�� v prohl�e�i.
