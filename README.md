# Kinderampel

Code für Arduino Micro oder Arduino Leonardo, um eine Spielzeug - Fussgänger - Lichtzeichenanlage zu steuern.

- Liest einen Taster ein, mit dem der Fussgänger grün für sich anfordern kann
- gibt Ansteuersignale für Fussgängerampel (rot + grün) aus
- gibt Ansteuersignale für Fahrbahnampel (rot + gelb + grün) aus
- gibt Ansteuersignal für Anzeige "Signal kommt" in der Nähe des Tasters aus
- gibt Signal aus, um Stromversorgung bei Nichtbenutzung zu kappen

Wird der Taster über längere (konfigurierbare Zeit) nicht betätigt, fängt die Fahrbahnampel zunächst an gelb zu blinken, bevor der Stromsparmodus eingenommen wird.

Sollte auch mit anderen Arduino Boards funktionieren. Eventuell nach (kleineren) Anpassungen.
Pegel und Pinnummern sowie Zeiten in Datei "Einstellungen.h" konfigurierbar.
