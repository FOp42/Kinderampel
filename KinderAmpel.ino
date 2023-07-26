#include <avr/sleep.h>
#include <avr/power.h>
#include <debounce.h>

// Typen fuer diverse Aufgaben einen Namen geben
typedef unsigned long zeit_t;
typedef uint8_t pin_t;
typedef uint8_t pegel_t;
typedef uint8_t mode_t;

// Konstanten, um Zeiten lesbarer zu schreiben
constexpr zeit_t SEKUNDE = 1000UL;
constexpr zeit_t MINUTE = SEKUNDE * 60UL;

#include "Einstellungen.h"

// Auswertung der Pegel Defines
#if (PEGEL_WENN_KNOPF_GEDRUECKT) > 2
constexpr pegel_t KNOPF_GEDRUECKT = HIGH;
constexpr mode_t KNOPF_MODUS = INPUT;
#else
constexpr pegel_t KNOPF_GEDRUECKT = LOW;
constexpr mode_t KNOPF_MODUS = INPUT_PULLUP;
#endif

#if (PEGEL_WENN_LED_LEUCHTET) > 2
constexpr pegel_t LED_AN = HIGH;
constexpr pegel_t LED_AUS = LOW;
#else
constexpr pegel_t LED_AUS = HIGH;
constexpr pegel_t LED_AN = LOW;
#endif

#if (PEGEL_SELBSTHALTUNG) > 2
constexpr pegel_t SELBSTHALTUNG_AKTIV = HIGH;
constexpr pegel_t SELBSTHALTUNG_INAKTIV = LOW;
#else
constexpr pegel_t SELBSTHALTUNG_AKTIV = LOW;
constexpr pegel_t SELBSTHALTUNG_INAKTIV = HIGH;
#endif

// Objekt Tasterentprellung
Button btn(0, NULL);
// Merker Knopf wurde gedrueckt
bool button_pressed;
// Zustand des Zustandsautomaten
enum
{
    STATE_AUTOS_GRUEN,
    STATE_AUTOS_GELB,
    STATE_AUTOS_ROT,
    STATE_FUSSGAENGER_GRUEN,
    STATE_FUSSGAENGER_ROT,
    STATE_AUTOS_ROTGELB,
    STATE_AUTOS_GRUEN_SPERRE,
    STATE_PRE_BLINK_GELB,
    STATE_BLINK_GELB,
    STATE_POST_BLINK_GELB,
    STATE_AUS
} state;
zeit_t state_since;
zeit_t blink_y_since;
zeit_t blink_b_since;
zeit_t now;
bool kein_gelb;

// Funktion, um Entprellfunktion mit Wert von Tastereingang zu fuettern
void update_button(void)
{
    btn.update((digitalRead(PIN_KNOPF) == KNOPF_GEDRUECKT) ? BTN_PRESSED : BTN_OPEN);
}

/**
 * Setup  Called once to initialise everything.
 */
void setup()
{
    // IO-Ports initialisieren
    digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
    pinMode(PIN_SELBSTHALTUNG, OUTPUT);
    pinMode(PIN_KNOPF, KNOPF_MODUS);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(PIN_LED_RV, LED_AUS);
    pinMode(PIN_LED_RV, OUTPUT);
    digitalWrite(PIN_LED_YV, LED_AUS);
    pinMode(PIN_LED_YV, OUTPUT);
    digitalWrite(PIN_LED_GV, LED_AUS);
    pinMode(PIN_LED_GV, OUTPUT);
    digitalWrite(PIN_LED_RP, LED_AUS);
    pinMode(PIN_LED_RP, OUTPUT);
    digitalWrite(PIN_LED_GP, LED_AUS);
    pinMode(PIN_LED_GP, OUTPUT);
    digitalWrite(PIN_LED_KNOPF, LED_AUS);
    pinMode(PIN_LED_KNOPF, OUTPUT);
    // warte, bis Taster losgelassen
    do
    {
        update_button();
    } while (BTN_PRESSED == btn.getState());
    // Variablen initialisieren
    button_pressed = false;
    state = STATE_AUTOS_GRUEN;
    blink_b_since = state_since = millis();
}

/**
 * Called after setup() repeatedly in a loop.
 */
void loop()
{
    // wenn Knopf gedrueckt, Merker setzen
    update_button();
    if (BTN_PRESSED == btn.getState())
    {
        button_pressed = true;
    }
    // aktuelle Zeit in Variable
    now = millis();
    switch (state)
    {
        // Ruhezustand : Autos gruen, Fussgaenger rot
        case STATE_AUTOS_GRUEN:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_YV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AN);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            // wenn Knopf gedrueckt, in Zustand Ampel fuer Autos auf gelb schalten wechseln
            if (button_pressed)
            {
                state_since = now;
                state = STATE_AUTOS_GELB;
            }
            // ansonsten, wenn keiner mehr spielt, Ampel fuer Autos gelb blinken lassen und Fussgaengerampel aus (vorbereiten)
            else if ((now - state_since) > ZEIT_NORMALBETRIEB)
            {
                blink_y_since = state_since = now;
                state = STATE_PRE_BLINK_GELB;
            }
            break;
        // jetzt geht's los, erstmal bekommen die Fahrzeuge gelb
        case STATE_AUTOS_GELB:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_YV, LED_AN);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            // wenn Zeit um, weiter schalten auf rot fuer Fahrzeuge
            if ((now - state_since) > ZEIT_GELB_VOR_ROT)
            {
                state_since = now;
                state = STATE_AUTOS_ROT;
            }
            break;
        // Fahrzeuge bekommen rot
        case STATE_AUTOS_ROT:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AN);
            digitalWrite(PIN_LED_YV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            // wenn Zeit um, bekommen Fussgaenger gruen
            if ((now - state_since) > ZEIT_AUTOS_ROT_VOR_FUSSGAENGER_GRUEN)
            {
                state_since = now;
                state = STATE_FUSSGAENGER_GRUEN;
            }
            break;
        // gruen fuer Fussgaenger
        case STATE_FUSSGAENGER_GRUEN:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AN);
            digitalWrite(PIN_LED_YV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AUS);
            digitalWrite(PIN_LED_GP, LED_AN);
            // Merker Knopf gedrueckt loeschen (waehrend Fussgaenger gruen haben ist Knopf gesperrt)
            button_pressed = false;
            // wenn Zeit abgelaufen, wieder rot fuer Fussgaenger
            if ((now - state_since) > ZEIT_FUSSGAENGER_GRUEN)
            {
                state_since = now;
                state = STATE_FUSSGAENGER_ROT;
            }
            break;
        // Fussgaenger bekommen rot, Fahrzeuge haben noch immer rot
        case STATE_FUSSGAENGER_ROT:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AN);
            digitalWrite(PIN_LED_YV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            // wenn Zeit, dass auch noch der letzte ueber die Strasse gekrabbelt ist, abgelaufen ist, Fahrzeugen gelb und rot signalisieren
            if ((now - state_since) > ZEIT_FUSSGAENGER_ROT_VOR_AUTOS_ROTGELB)
            {
                state_since = now;
                state = STATE_AUTOS_ROTGELB;
            }
            break;
        // Fussgaenger rot, Fahrzeuge rot und gelb
        case STATE_AUTOS_ROTGELB:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AN);
            digitalWrite(PIN_LED_YV, LED_AN);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            // wenn Zeit abgelaufen, weiterschalten auf gruen fuer Fahrzeuge
            if ((now - state_since) > ZEIT_ROTGELB_VOR_GRUEN)
            {
                state_since = now;
                state = STATE_AUTOS_GRUEN_SPERRE;
            }
            break;
        // Fussgaenger haben rot, Fahrzeuge gruen, ein Druck auf den Knopf wird registriert, loest aber noch keinen neuen Zyklus aus
        case STATE_AUTOS_GRUEN_SPERRE:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_YV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AN);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            // nach Wartezeit zurueck auf Anfang, dadurch aendert sich nichts an der Anzeige, aber ein Knopfdruck loest einen neuen Zyklus aus
            if ((now - state_since) > ZEIT_SPERRE)
            {
                state_since = now;
                state = STATE_AUTOS_GRUEN;
            }
            break;
        // Ampel wurde laengere Zeit nicht gedrueckt : schalte um auf gelbes Blinken fuer Fahrzeuge, zunaechst noch mit rot fuer Fussgaenger
        case STATE_PRE_BLINK_GELB:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            button_pressed = false;
            // gelb blinken lassen (mit aus beginnen)
            if ((now - blink_y_since) > (GELB_BLINKDAUER / 2U))
            {
                digitalWrite(PIN_LED_YV, LED_AN);
                if ((now - blink_y_since) > GELB_BLINKDAUER)
                {
                    blink_y_since = now;
                }
            }
            else
            {
                digitalWrite(PIN_LED_YV, LED_AUS);
            }
            if (button_pressed)
            {
                state_since = now;
                button_pressed = false;
                state = STATE_POST_BLINK_GELB;
            }
            else
            {
                // wenn Zeit abgelaufen, weiter zu gelb Blinken und Fussgaenger aus
                if ((now - state_since) > ZEIT_PREBLINK)
                {
                    state_since = now;
                    state = STATE_BLINK_GELB;
                }
            }
            break;
        case STATE_BLINK_GELB:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AUS);
            digitalWrite(PIN_LED_GP, LED_AUS);
            if ((now - blink_y_since) > (GELB_BLINKDAUER / 2U))
            {
                digitalWrite(PIN_LED_YV, LED_AN);
                kein_gelb = false;
                if ((now - blink_y_since) > GELB_BLINKDAUER)
                {
                    blink_y_since = now;
                }
            }
            else
            {
                digitalWrite(PIN_LED_YV, LED_AUS);
                kein_gelb = true;
            }
            if (button_pressed)
            {
                state_since = now;
                button_pressed = false;
                state = STATE_POST_BLINK_GELB;
            }
            else
            {
                if (kein_gelb && ((now - state_since) > ZEIT_BLINK))
                {
                    state = STATE_AUS;
                }
            }
            break;
        case STATE_POST_BLINK_GELB:
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_AKTIV);
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AN);
            digitalWrite(PIN_LED_GP, LED_AUS);
            button_pressed = false;
            if ((now - blink_y_since) > (GELB_BLINKDAUER / 2U))
            {
                if (kein_gelb && ((now - state_since) > ZEIT_POSTBLINK))
                {
                    state_since = now;
                    state = STATE_AUTOS_GRUEN;
                }
                else
                {
                    kein_gelb = false;
                    digitalWrite(PIN_LED_YV, LED_AN);
                    if ((now - blink_y_since) > GELB_BLINKDAUER)
                    {
                        blink_y_since = now;
                    }
                }
            }
            else
            {
                digitalWrite(PIN_LED_YV, LED_AUS);
                kein_gelb = true;
            }
            break;
        // keiner schaut mehr, so viel wie moeglich ausschalten, um Stromverbrauch zu minimieren
        case STATE_AUS:
            digitalWrite(PIN_LED_RV, LED_AUS);
            digitalWrite(PIN_LED_YV, LED_AUS);
            digitalWrite(PIN_LED_GV, LED_AUS);
            digitalWrite(PIN_LED_RP, LED_AUS);
            digitalWrite(PIN_LED_GP, LED_AUS);
            digitalWrite(PIN_LED_KNOPF, LED_AUS);
            digitalWrite(LED_BUILTIN, LOW);
            digitalWrite(PIN_SELBSTHALTUNG, SELBSTHALTUNG_INAKTIV);
            button_pressed = false;
            cli();
            power_all_disable();
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_mode();
            break;
    }
    // wenn Merker Knopf gedrueckt gesetzt ist, Anzeige Signal kommt blinken lassen
    if (button_pressed)
    {
        if ((now - blink_b_since) > (WARTE_BLINKDAUER / 2U))
        {
            digitalWrite(PIN_LED_KNOPF, LED_AUS);
            if ((now - blink_b_since) > WARTE_BLINKDAUER)
            {
                blink_b_since = now;
            }
        }
        else
        {
            digitalWrite(PIN_LED_KNOPF, LED_AN);
        }
    }
    else
    {
        digitalWrite(PIN_LED_KNOPF, LED_AUS);
        blink_b_since = now;
    }
}
