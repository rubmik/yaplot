# yaplot

## HARRASTUSPROJEKTI

Katselin netissä videoita DIY-piirtureista (engl. plotter), ja päätin kokeilumielessä ja harrastuksen vuoksi lähteä rakentamaan omaa.

Tarkoitus on ensin tehdä tietokoneella toimiva ohjelmisto, joka simuloi piirturin toimintaa, ja sen jälkeen portata se mikroprosessorille, jonka ympärille piirturi rakennetaan.

Valittu toteutustapa perustuu "käsivarteen", jonka päässä on kynä. Käsivarressa on tyvinivel ja kyynärnivel sekä mekanismi kynän laskulle ja nostolle. Toteutus on "typerä" verrattuna esimerkiksi NC-koneisiin, jotka liikkuvat tavallisesti akselien suuntaisilla johteilla. Vallitu toteutus on mm. epätarkempi (joskin tässä tapauksessa puhutaan alle millimetrin heitosta) ja ennen kaikkea matematiikaltaan ja ohjelmoinniltaan monimutkaisempi; etuna on lähinnä mekaaninen yksinkertaisuus. Kuitenkin juuri monimutkaisempien ongelmien ratkominen oli valinnan syynä. DIY-piirtureissa käsivarsi on tyypillinen ratkaisu sen mekaanisen yksinkertaisuuden vuoksi, ja niihin löytyy myös valmiita ohjelmistoja, jolloin moni harrastaja voi vain kävellä "valmiiseen pöytään".

Ensimmäisessä simulaatioprotossa käytin itse tehtyä G-koodin tulkkia, mutta päädyin toista protoa varten rakentamaan oman polkutulkin (SVG:n path-elementin d-kenttä), sillä laitteen syötteenä on joka tapauksessa vektorigrafiikka, jonka muuntaminen G-koodiksi on vähemmän mielenkiintoista. Tyypillisesti tällaisissa DIY-piirtureissa käytetään G-koodia piirturin ohjaamiseen, jolloin vektorigrafiikka täytyy muuntaa jollain työkalulla G-koodiksi laitetta varten. Oman polkutulkin kirjoittaminen on ollut mielenkiintoista, ja se on pakottanut perehtymään niin tulkkien toimintaan kuin ellipsien matemaattisiin ongelmiin.

Projekti on aloitettu loppukeväästä 2025, ja sitä on vähitellen muilta kiireiltä edistetty.

Projektin nimi "yaplot" tulee sanoista "yet another plotter", "jälleen uusi piirturi".

## TILANNE 18.11.2025

Muutosta ja muista kiireistä aiheutuneen tauon jälkeen olen taas ehtinyt edistää projektia. Polkutulkki on nyt alkeellisella tasolla yhdistetty simulaatioon, mutta vaaditaan vielä testejä ja säätöjä, jotta ohjelman halutunlainen toiminta saadaan taattua.

Ohjelmoinnin tauosta huolimatta käytännön toteutukseen liittyvää tutkimusta on saatu edistettyä. Erityisesti on tutkittu, millaisia komponentteja tarvitaan (askelmoottorit ja niiden ohjaimet varsiin, servo Z-liikkeeseen). Koska askelmoottorit ovat verrattain "painavia", ensimmäistä vartta voidaan ohjata suoraan, mutta toiseen tarvitaan hihnakäyttö. Koodipuolen ollessa pitkälti valmis laitteen fyysinen rakentaminen alkaa tulla ajankohtaisemmaksi.

Käytännön toteutuksessa on edessä myös ongelma, jota ei osattu huomioida oikein alussa. Ohjelman binääri vie nykyisellään noin 35 kilotavua (GCC:llä käännettynä), kun Arduino Unon Flash-muistissa on tilaa vain 32 kilotavua. Vaikka ohjelman voisikin saada puristettua tarpeeksi pieneksi, pahempi muistiongelma on RAM-puolella: testattaessa Valgrindillä ohjelma allokoi kymmeniä kilotavuja muistia jo hyvin yksinkertaisella polulla, mutta Arduino Unon RAM on vain 2 kilotavua. Toisin sanoen laskenta on tehtävä PC:llä, josta vain lähetetään liikkeet mikrokontrollerille sarjaväylän kautta.

Tehty:

- SVG-tiedostojen esiprosessori
- SVG-polkutulkki
- piirturin simulaatio

Tehtävänä:

- laitteen fyysinen rakentaminen
    - mekaaninen proto
    - elektroniikan lisääminen
- mikrokontrollerin ohjaaminen
    - moottorien ohjaukset mikrokontrollerilla
    - keskustelu sarjaväylän kautta
