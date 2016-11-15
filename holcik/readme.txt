P�ep�na� 1 - re�im nastaven� - ON/m��en� - OFF
P�ep�na� 2 - re�im sp�nku ON/OFF
P�ep�na� 3 - flash firmware ON/OFF

Za��zen� se m��e nach�zet ve �ty�ech re�imech podle nastaven� p�ep�na�� 1,2 a 3.
            | 1 (re�im) 2 (sp�nek)  3 (flash)
------------|------------------------------            
flash       | x         x           OFF
nastaven�   | ON        OFF         ON
test m��en� | OFF       OFF         ON
m��en�      | OFF       ON          ON


1. Flash FW
-----------
P�ep�na� 1 (re�im) je v libovoln� poloze, p�ep�na� 2 (sp�nek) je v libovoln� poloze, p�ep�na� 3 (flash) je v poloze OFF.
Za��zen� mus� b�t p�ipojen� k nap�jen�, nap�jen� z USB je kv�li spot�eb� v re�imu sp�nku vy�azen� natrvalo z �innosti.

2.Nastaven�
-----------
P�ep�na� 1 (re�im) je v poloze ON (nastaven�), p�ep�na� 2 (sp�nek) je v poloze OFF, p�ep�na� 3 (flash) je v poloze ON

Za��zen� se p�epne do re�imu AP (p��stupov� bod). Po zadan� adresy 192.168.4.1 v prohl�e�i se zobraz� formul�� pro nastaven� s�t�:
AP SSID - identifika�n� znak s�t�
AP PASSWORD - heslo k s�ti
IP - adresa za��zen� nap�. 192.168.1.200
Maska - s�ov� maska nap�. 255.255.255.0
Gateway - br�na nap�. 192.168.1.1
Xively API key - kl�� pro p��stup ke slu�b� Xively
Xively feed ID - identifikace feedu
Prodleva mezi m��en�mi - prodleva v sekund�ch mezi jednotliv�mi m��en�mi
P�ed ulo�en�m p�epnout p�ep�na� 1 (re�im) do polohy ON (m��en�).
Nastaven� se ulo�� a za��zen� se restartuje. Za��zen� je d�le v re�imu STA (stanice). 

3.Test M��en�
-------------
P�ep�na� 1 (re�im) je v poloze OFF (m��en�), p�ep�na� 2 (sp�nek) je v poloze OFF, p�ep�na� 3 (flash) je v poloze ON

Po zad�n� adresy IP za��zen� (nastaven� v bod� 1.Nastaven�) do prohl�e�e se zobraz� formul�� jako v p��pad� nastaven� a je mo�no m�nit parametry s�t�, xively p��stupu a prodlevy jako v p��pad� nastaven�.
V p��pad� proveden� zm�n je nutn� �daje ulo�it. 
D�le se ve formul��i zobrazuj� nam��en� �daje. Perioda m��en� je v tomto p��pad� 10s.

Nam��en� �daje:
Teplota �C
Vlhkost %Rh
Nap�t� baterie V

4.M��en�
--------
P�ep�na� 1 (re�im) je v poloze OFF (m��en�), p�ep�na� 2 (sp�nek) v poloze ON, p�ep�na� 3 (flash) je v poloze ON

Za��zen� provede m��en�, v�sledky ode�le na Xively a upadne do re�imu sp�nku s minim�ln� spot�ebou kolem 0,1mA. Po uplynut� nastaven� prodlevy se cel� cyklus opakuje.
V tomto p��pad� nen� mo�n� zobrazit webov� formul�� v prohl�e�i.
