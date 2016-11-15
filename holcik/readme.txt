Pøepínaè 1 - reim nastavení - ON/mìøení - OFF
Pøepínaè 2 - reim spánku ON/OFF
Pøepínaè 3 - flash firmware ON/OFF

Zaøízení se mùe nacházet ve ètyøech reimech podle nastavení pøepínaèù 1,2 a 3.
            | 1 (reim) 2 (spánek)  3 (flash)
------------|------------------------------            
flash       | x         x           OFF
nastavení   | ON        OFF         ON
test mìøení | OFF       OFF         ON
mìøení      | OFF       ON          ON


1. Flash FW
-----------
Pøepínaè 1 (reim) je v libovolné poloze, pøepínaè 2 (spánek) je v libovolné poloze, pøepínaè 3 (flash) je v poloze OFF.
Zaøízení musí bıt pøipojené k napájení, napájení z USB je kvùli spotøebì v reimu spánku vyøazené natrvalo z èinnosti.

2.Nastavení
-----------
Pøepínaè 1 (reim) je v poloze ON (nastavení), pøepínaè 2 (spánek) je v poloze OFF, pøepínaè 3 (flash) je v poloze ON

Zaøízení se pøepne do reimu AP (pøístupovı bod). Po zadaní adresy 192.168.4.1 v prohlíeèi se zobrazí formuláø pro nastavení sítì:
AP SSID - identifikaèní znak sítì
AP PASSWORD - heslo k síti
IP - adresa zaøízení napø. 192.168.1.200
Maska - síová maska napø. 255.255.255.0
Gateway - brána napø. 192.168.1.1
Xively API key - klíè pro pøístup ke slubì Xively
Xively feed ID - identifikace feedu
Prodleva mezi mìøeními - prodleva v sekundách mezi jednotlivımi mìøeními
Pøed uloením pøepnout pøepínaè 1 (reim) do polohy ON (mìøení).
Nastavení se uloí a zaøízení se restartuje. Zaøízení je dále v reimu STA (stanice). 

3.Test Mìøení
-------------
Pøepínaè 1 (reim) je v poloze OFF (mìøení), pøepínaè 2 (spánek) je v poloze OFF, pøepínaè 3 (flash) je v poloze ON

Po zadání adresy IP zaøízení (nastavené v bodì 1.Nastavení) do prohlíeèe se zobrazí formuláø jako v pøípadì nastavení a je mono mìnit parametry sítì, xively pøístupu a prodlevy jako v pøípadì nastavení.
V pøípadì provedení zmìn je nutné údaje uloit. 
Dále se ve formuláøi zobrazují namìøené údaje. Perioda mìøení je v tomto pøípadì 10s.

Namìøené údaje:
Teplota °C
Vlhkost %Rh
Napìtí baterie V

4.Mìøení
--------
Pøepínaè 1 (reim) je v poloze OFF (mìøení), pøepínaè 2 (spánek) v poloze ON, pøepínaè 3 (flash) je v poloze ON

Zaøízení provede mìøení, vısledky odešle na Xively a upadne do reimu spánku s minimální spotøebou kolem 0,1mA. Po uplynutí nastavené prodlevy se celı cyklus opakuje.
V tomto pøípadì není moné zobrazit webovı formuláø v prohlíeèi.
