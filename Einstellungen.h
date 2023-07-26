#pragma once

#define PEGEL_WENN_KNOPF_GEDRUECKT 0  // Was liegt am Eingang an, wenn Knopf gedrueckt : 0 fuer Ground (0V); 5 fuer Versorgungsspannung (ca. 5V)
#define PEGEL_WENN_LED_LEUCHTET 5     // Was muss am Ausgang anliegen, damit LED leuchtet : 0 fuer Ground (0V); 5 fuer Versorgungsspannung (ca. 5V)
#define PEGEL_SELBSTHALTUNG 5         //  Was muss am Ausgang anliegen, damit Schaltung weiter bestromt wird : 0 fuer Ground (0V); 5 fuer Versorgungsspannung (ca. 5V) (optionales Feature)

// Arduino Pinnummern, an denen Peripherie angeschlossen ist
constexpr pin_t PIN_KNOPF = 10;         // Pinnummer an den der Druckknopf angeschlossen ist
constexpr pin_t PIN_LED_RV = 3;         // Pinnummer an den rote Leds Ampel Fahrzeuge angeschlossen sind
constexpr pin_t PIN_LED_YV = 4;         // Pinnummer an den gelbe Leds Ampel Fahrzeuge angeschlossen sind
constexpr pin_t PIN_LED_GV = 5;         // Pinnummer an den gruene Leds Ampel Fahrzeuge angeschlossen sind
constexpr pin_t PIN_LED_RP = 6;         // Pinnummer an den rote Leds Ampel Fussgaenger angeschlossen sind
constexpr pin_t PIN_LED_GP = 7;         // Pinnummer an den gruene Leds Ampel Fussgaenger angeschlossen sind
constexpr pin_t PIN_LED_KNOPF = 8;      // Pinnummer an den Leds Schriftzug "Signal kommt" angeschlossen sind
constexpr pin_t PIN_SELBSTHALTUNG = 9;  // Pinnummer an dem Signal ausgegeben wird, Schaltung weiter mit Spannung versorgen

// Zeiten alle Angaben in Millisekunden
// um Sekunden in Millisekunden zu wandeln mit SEKUNDE multiplizieren
// um Minuten in Millisekunden zu wandeln mit MINUTE multiplizieren
constexpr zeit_t WARTE_BLINKDAUER = 1 * SEKUNDE;                        // Periodendauer Blinken der Signal kommt Anzeige
constexpr zeit_t GELB_BLINKDAUER = 1 * SEKUNDE;                         // Periodendauer Blinken mit gelb fuer Fahrzeuge
constexpr zeit_t ZEIT_NORMALBETRIEB = 10 * MINUTE;                      // Zeit, die Ampel in Normalbetrieb bleibt, wenn Taster nicht betaetigt
constexpr zeit_t ZEIT_GELB_VOR_ROT = 1 * SEKUNDE;                       // Zeitdauer fuer die gelb zwischen gruen und rot (Fahrzeuge) leuchtet
constexpr zeit_t ZEIT_AUTOS_ROT_VOR_FUSSGAENGER_GRUEN = 1 * SEKUNDE;    // Zeit, die Fahrzeuge schon rot haben, bevor Fussgaenger gruen bekommen
constexpr zeit_t ZEIT_FUSSGAENGER_GRUEN = 6 * SEKUNDE;                  // Dauer der Gruenphase fuer Fussgaenger
constexpr zeit_t ZEIT_FUSSGAENGER_ROT_VOR_AUTOS_ROTGELB = 3 * SEKUNDE;  // Zeit die nachdem Fussgaenger wieder rot haben gewartet wird, bevor Fahrzeuge rot und gelb bekommen
constexpr zeit_t ZEIT_ROTGELB_VOR_GRUEN = 1 * SEKUNDE;                  // Dauer rot und gelb fuer Fahrzeuge
constexpr zeit_t ZEIT_SPERRE = 30 * SEKUNDE;                            // wie lange haben Autos nach einem Zyklus mindestens gruen, bevor ein neuer Zyklus gestartet werden kann
constexpr zeit_t ZEIT_PREBLINK = 2 * SEKUNDE;                           // Dauer Uebergang zum gelben Blinken fuer Fahrzeuge, Fussgaenger haben rot
constexpr zeit_t ZEIT_BLINK = 3 * MINUTE;                               // wie lange wird gelb geblinkt, wenn Taster nicht betaetigt, bevor Anzeige sich abschaltet
constexpr zeit_t ZEIT_POSTBLINK = 2 * SEKUNDE;                          // wie lange blinkt die Ampel fuer Fahrzeuge noch gelb bei rot fuer Fussgaenger, bevor wieder Normalbetrieb
