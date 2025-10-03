# yaplot

## HARRASTUSPROJEKTI

Katselin netissä videoita DIY-piirtureista (engl. plotter), ja päätin kokeilumielessä ja harrastuksen vuoksi lähteä rakentamaan omaa.

Tarkoitus on ensin tehdä tietokoneella toimiva ohjelmisto, joka simuloi piirturin toimintaa, ja sen jälkeen portata se mikroprosessorille, jonka ympärille piirturi rakennetaan.

Valittu toteutustapa perustuu "käsivarteen", jonka päässä on kynä. Käsivarressa on tyvinivel ja kyynärnivel sekä mekanismi kynän laskulle ja nostolle. Toteutus on "typerä" verrattuna esimerkiksi NC-koneisiin, jotka liikkuvat tavallisesti akselien suuntaisilla johteilla. Vallitu toteutus on mm. epätarkempi (joskin tässä tapauksessa puhutaan alle millimetrin heitosta) ja ennen kaikkea matematiikaltaan ja ohjelmoinniltaan monimutkaisempi; etuna on lähinnä mekaaninen yksinkertaisuus. Kuitenkin juuri monimutkaisempien ongelmien ratkominen oli valinnan syynä. DIY-piirtureissa käsivarsi on tyypillinen ratkaisu sen mekaanisen yksinkertaisuuden vuoksi, ja niihin löytyy myös valmiita ohjelmoistoja, jolloin moni harrastaja voi vain kävellä "valmiiseen pöytään".

Ensimmäisessä simulaatioprotossa käytin itse tehtyä G-koodin tulkkia, mutta päädyin toista protoa varten rakentamaan oman SVG-tulkin, sillä laitteen syötteenä on joka tapauksessa vektorigrafiikka, jonka muuntaminen G-koodiksi on vähemmän mielenkiintoista. Tyypillisesti tällaisissa DIY-piirtureissa käytetään G-koodia piirturin ohjaamiseen, jolloin vektorigrafiikka täytyy muuntaa jollain työkalulla G-koodiksi laitetta varten. Oman SVG-tulkin kirjoittaminen on ollut mielenkiintoista, ja se on pakottanut perehtymään niin tulkkien toimintaan kuin ellipsien matemaattisiin ongelmiin.

Projektin nimi "yaplot" tulee sanoista "yet another plotter", "jälleen uusi piirturi".

## TILANNE 3.10.2025

Valmiina on käsivarren ohjaus simulaatiossa, ja suurin osa SVG-tulkista.

Tällä hetkellä työn alla olevana ongelmana on ratkaista virheet rotatoitujen ellipsien piirtämisessä.

Sen jälkeen vuorossa on SVG-tulkin yhdistäminen simulaatioon, jonka jälkeen alkaa laitteen rakentaminen ja simulaation porttaaminen laitteelle.
