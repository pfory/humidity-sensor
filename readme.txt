Vlhkomìr
--------
Zaøízení slouí k mìøení vlhkosti a teploty a odesílání hodnot na server xively.com. K pøenosu vyuívá bezdrátové pøipojení napø. k domácí síti. 
Konfigurace síovıch adres, xively klíèù a intervalu mìøení je uivatelsky nastavitelná.

Pøepínaèe
Pohled ze strany souèástek 
---------
| O     |   Pøepínaè 3
| O     |   Pøepínaè 2
| O     |   Pøepínaè 1
---------
 ON  OFF
 x - libovolná poloha pøepínaèe
 

Zaøízení se mùe nacházet ve ètyøech reimech podle nastavení pøepínaèù 1,2 a 3.
            | 2         1           3
------------|------------------------------            
flash       | x         x           OFF
nastavení   | ON        ON          x
test mìøení | OFF       ON          x
mìøení      | OFF       OFF         ON


1. Flash FW
-----------
Pøepínaè 1 je v libovolné poloze, Pøepínaè 2 je v libovolné poloze, Pøepínaè 3 je v poloze OFF.

---------
|     O |   Pøepínaè 3
| x   x |   Pøepínaè 2
| x   x |   Pøepínaè 1
---------
 ON  OFF

Zaøízení musí bıt pøipojené k napájení, napájení z USB je kvùli spotøebì v reimu spánku vyøazené natrvalo z èinnosti.

2.Nastavení
-----------
Pøepínaè 1 je v poloze ON, Pøepínaè 2 je v poloze ON, Pøepínaè 3 je v libovolné poloze.

---------
| x   x |   Pøepínaè 3
| O     |   Pøepínaè 2
| O     |   Pøepínaè 1
---------
 ON  OFF

Zaøízení se pøepne do reimu AP (pøístupovı bod). V seznamu Wifi sítí na PC se zobrazí novı pøístupovı bod s názvem ESPHum, heslo pro pøipojení je "humidity007" bez uvozovek. 
Po zadaní adresy 192.168.4.1 v prohlíeèi se zobrazí formuláø pro nastavení sítì:
AP SSID - identifikaèní znak sítì ke které se bude zaøízení pøipojovat. Fusal2
AP PASSWORD - heslo k síti 1860918609
IP - adresa zaøízení napø. 192.168.1.200. Adresa musí bıt v rozsahu sítì, ke které se pøipojujeme. 192.168.0.93
Maska - síová maska napø. 255.255.255.0
Gateway - brána napø. 192.168.1.1, pøístupovı bod do internetu. 192.168.0.1
Prodleva mezi mìøeními - prodleva v sekundách mezi jednotlivımi mìøeními. Doporuèuji nastavit více ne 300s kvùli vıdri baterií.

Pøed uloením pøepnout Pøepínaè 2 (reim) do polohy ON (mìøení).
Nastavení se uloí a zaøízení se restartuje. Zaøízení je dále v reimu STA (stanice). 

3.Test Mìøení
-------------
Pøepínaè 1 je v poloze ON, Pøepínaè 2 je v poloze OFF, Pøepínaè 3 je v libovolné poloze.
---------
| x   x |   Pøepínaè 3
|     O |   Pøepínaè 2
| O     |   Pøepínaè 1
---------
 ON  OFF

Po zadání adresy IP zaøízení (nastavené v bodì 1.Nastavení) do prohlíeèe se zobrazí formuláø ve kterám se zobrazují namìøené údaje. 
Perioda mìøení je v tomto pøípadì 10s.

Namìøené údaje:
Teplota °C
Vlhkost %Rh
Napìtí baterie V

4.Mìøení
--------
Pøepínaè 1 je v poloze OFF, Pøepínaè 2 v poloze OFF, Pøepínaè 3 je v poloze OFF.
---------
| O     |   Pøepínaè 3
|     O |   Pøepínaè 2
|     O |   Pøepínaè 1
---------
 ON  OFF

Zaøízení provede mìøení, vısledky odešle na Xively a upadne do reimu spánku s minimální spotøebou kolem 0,1mA. Po uplynutí nastavené prodlevy se celı cyklus opakuje.
V tomto pøípadì není moné zobrazit webovı formuláø v prohlíeèi.
