/***************************************************************************
 *   Copyright (C) 2005 by Reel Multimedia;  Author:  Markus Hahn          *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 ***************************************************************************
 *
 *  i18n.c: Internationalization
 *
 ***************************************************************************/

#include "i18n.h"

const tI18nPhrase Phrases[] = {
    {"Channel Scan",
     "Kanalsuche",
     "",
     "Scansione canali",
     "Kanaal scan",
     "",
     "",
     "",
     "Kanavahaku",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "����� �������",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Vyhledani kanalu",                        // �esky (Czech)
     },
    {"Search Transponders for DVB Channels",    //EN
     "Durchsucht Transponder nach DVB Kan�len.",        // DE
     "",                        // Slovenski
     "Ricerca Transponders di Canali DVB",      // IT
     "Doorzoek Transponders naar DVB kanalen",  // Nederlands
     "",                        // Portugu�s
     "",                        // Fran�ais
     "",                        // Norsk
     "Kanavahaku DVB-transpondereille", // FI
     "",                        // Polski
     "",                        // Espa�ol
     "",                        // �������� (Greek)
     "",                        // Svenska
     "",                        // Romaneste
     "",                        // Magyar
     "",                        // Catal�
     "",                        // ������� (Russian)
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Vyhledan Transpoderu pro DVB kanaly",                        // �esky (Czech)
     },
    {"Search Mode",
     "Suchmodus",
     "",
     "Modalita' di Ricerca",
     "Zoek mode",
     "",
     "",
     "",
     "Hakutapa",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavaci mod",                        // �esky (Czech)
     },
    {
     "Add new channels to",     //EN
     "Gefundene Kan�le",                        // DE
     "",                        // Slovenski
     "",                        // IT
     "",                        // Nederlands
     "",                        // Portugu�s
     "",                        // Fran�ais
     "",                        // Norsk
     "",                        // FI
     "",                        // Polski
     "",                        // Espa�ol
     "",                        // �������� (Greek)
     "",                        // Svenska
     "",                        // Romaneste
     "",                        // Magyar
     "",                        // Catal�
     "",                        // ������� (Russian)
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
    },
    {
     "end of channellist",      // EN
     "am Ende hinzuf�gen",      // DE
     "",                        // Slovenski
     "",                        // IT
     "",                        // Nederlands
     "",                        // Portugu�s
     "",                        // Fran�ais
     "",                        // Norsk
     "",                        // FI
     "",                        // Polski
     "",                        // Espa�ol
     "",                        // �������� (Greek)
     "",                        // Svenska
     "",                        // Romaneste
     "",                        // Magyar
     "",                        // Catal�
     "",                        // ������� (Russian)
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
    },
    {
     "new channellist",         // EN
     "in neue Liste einf�gen",  // DE
     "",                        // Slovenski
     "",                        // IT
     "",                        // Nederlands
     "",                        // Portugu�s
     "",                        // Fran�ais
     "",                        // Norsk
     "",                        // FI
     "",                        // Polski
     "",                        // Espa�ol
     "",                        // �������� (Greek)
     "",                        // Svenska
     "",                        // Romaneste
     "",                        // Magyar
     "",                        // Catal�
     "",                        // ������� (Russian)
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
    },
    {
     "bouquets",                //EN
     "in Bouquets einf�gen",    // DE
     "",                        // Slovenski
     "",                        // IT
     "",                        // Nederlands
     "",                        // Portugu�s
     "",                        // Fran�ais
     "",                        // Norsk
     "",                        // FI
     "",                        // Polski
     "",                        // Espa�ol
     "",                        // �������� (Greek)
     "",                        // Svenska
     "",                        // Romaneste
     "",                        // Magyar
     "",                        // Catal�
     "",                        // ������� (Russian)
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
    },
    {"scanning on transponder",
     "Suche auf Transponder",
     "",
     "Ricerca trasponder",
     "Scan op transponder",
     "",
     "",
     "",
     "haetaan transponderilta",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani transponderu",                        // �esky (Czech)
     },
    {"Scanning configured satellites",
     "Durchsuche eingerichtete Satelliten",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Hledani konfigurace satelitu",                        // �esky (Czech)
     },
    {"DiSEqC",
     "DiSEqC",
     "DiSEqC",
     "DiSEqC",
     "DiSEqC",
     "",
     "",
     "",
     "DiSEqC-kytkin",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "DiSEqC",                        // �esky (Czech)
     },
    {"FEC",
     "FEC",
     "",
     "FEC",
     "FEC",
     "",
     "",
     "",
     "FEC",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "FEC",                        // �esky (Czech)
     },
    {"None",
     "Keine",
     "",
     "Nessun",
     "Geen",
     "",
     "",
     "",
     "ei",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Zadny",                        // �esky (Czech)
     },
    {"Auto",
     "Auto",
     "",
     "Auto",
     "Auto",
     "",
     "",
     "",
     "auto",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Auto",                        // �esky (Czech)
     },
    {"SearchMode$Auto",
     "Automatisch",
     "",
     "Ricerca automatica",
     "Automatisch",
     "",
     "",
     "",
     "automaattinen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Automaticke prohledavani",                        // �esky (Czech)
     },
    {"Manual",
     "Manuell",
     "",
     "Manuale",
     "Handmatig",
     "",
     "",
     "",
     "manuaalinen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Rucne",                        // �esky (Czech)
     },
    {"Detailed search",
     "Detaillierte Suche",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Detailne prohledavat",                        // �esky (Czech)
     },
    {"Position",
     "Position",
     "",
     "Posizione",
     "Positie",
     "",
     "",
     "",
     "Sijainti",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Pozice",                        // �esky (Czech)
     },
    {"Tuner Error",
     "Tuner Fehler",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Chyba tuneru",                        // �esky (Czech)
     },
    {"Scanning on transponder",
     "Durchsuche Transponder",
     "",
     "Ricerca Transponders",
     "Scannen op transponder",
     "",
     "",
     "",
     "Haetaan transponderilta",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani transponderu",                        // �esky (Czech)
     },
    {"channels in current list",
     "Kan�le in aktueller Liste",
     "",
     "Canali presenti nella lista",
     "Kanalen in huidige lijst",
     "",
     "",
     "",
     "T�m�nhetkiset kanavat",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Aktuelni seznam kanalu",                        // �esky (Czech)
     },
    {"TV CHANNELS",
     "TV KAN�LE",
     "",
     "CANALI TV",
     "TV KANALEN",
     "",
     "",
     "",
     "TV-kanavat",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Televizni kanaly",                        // �esky (Czech)
     },
    {"No new channels found",
     "Keine neuen Kan�le gefunden",
     "",
     "Non sono stati trovati nuovi canali",
     "Geen nieuwe kanalen gevonden",
     "",
     "",
     "",
     "Uusia kanavia ei l�ydetty",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Zadne nove kanaly nenalezeny",                        // �esky (Czech)
     },
    {"Scanning aborted by User",
     "Suche abgebrochen",
     "",
     "Ricerca interrotta dall'Utente",
     "Scannen afgebroken door User",
     "",
     "",
     "",
     "Haku keskeytetty",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani zruseno uzivatelem",                        // �esky (Czech)
     },

    {"  TV CHANNELS                      RADIO",
     "  TV KAN�LE                          RADIO",
     "",
     "  CANALI TV                      RADIO",
     "  TV KANALEN                      RADIO",
     "",
     "",
     "",
     "  TV-kanavat                      Radio",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "TV KANALY                  RADIO",                        // �esky (Czech)
     },
    {"Symbolrate",
     "Symbolrate",
     "",
     "Symbolrate",
     "Symbolrate",
     "",
     "",
     "",
     "Symbolinopeus",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "����.��������",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Symbolrate",                        // �esky (Czech)
     },
    {"Frequency",
     "Frequenz",
     "",
     "Frequenza",
     "Frequentie",
     "",
     "",
     "",
     "Taajuus",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "�������",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Frekvence",                        // �esky (Czech)
     },
    {"Scanning %s\tPlease Wait",
     "Durchsuche %s\tBitte warten",
     "",
     "",
     "",
     "",
     "",
     "",
     "Haku k�ynniss� %s.\tOdota hetkinen...",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani %s\tProsim cekejte",                        // �esky (Czech)
     },
    {"Scanning %s (%iMHz) \tPlease Wait",
     "Durchsuche %s (%iMHz) \tBitte warten",
     "",
     "",
     "",
     "",
     "",
     "",
     "Haku k�ynniss� %s.\tOdota hetkinen...",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani %s (%iMHz) \tProsim cekejte",                        // �esky (Czech)
     },
    {"Scanning %s (%.3fMHz) \tPlease Wait",
     "Durchsuche %s (%.3fMHz) \tBitte warten",
     "",
     "",
     "",
     "",
     "",
     "",
     "Haku k�ynniss� %s.\tOdota hetkinen...",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani %s (%.3fMHz) \tProsim cekejte",                        // �esky (Czech)
     },
    {"Button$Change",
     "Ausw�hlen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Zmenit",                        // �esky (Czech)
     },
    {"Button$Start",
     "Start",
     "",
     "Start",
     "Start",
     "",
     "",
     "",
     "Aloita",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Start",                        // �esky (Czech)
     },
    {"Button$Backup",
     "Speichern",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Zpet",                        // �esky (Czech)
     },
    {"Backup",
     "Speichern",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Zpet",                        // �esky (Czech)
     },
    {"Button$Delete",
     "L�schen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Smazat",                        // �esky (Czech)
     },
    {"Button$New",
     "Neu",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Novy",                        // �esky (Czech)
     },
    {"Radio only",
     "nur Radio",
     "",
     "Solo radio",
     "Alleen Radio",
     "",
     "",
     "",
     "vain radio",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Pouze radio",                        // �esky (Czech)
     },
    {"TV only",
     "nur TV",
     "",
     "Solo TV",
     "Alleen TV",
     "",
     "",
     "",
     "vain TV",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Pouze TV",                        // �esky (Czech)
     },
    {"HDTV only",
     "nur HDTV",
     "",
     "Solo HDTV",
     "Alleen HDTV",
     "",
     "",
     "",
     "vain HDTV",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Pouze HDTV",                        // �esky (Czech)
     },
    {"Radio + TV",
     "Radio + TV",
     "",
     "Radio + TV",
     "Radio + TV",
     "",
     "",
     "",
     "radio + TV",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Radio + TV",                        // �esky (Czech)
     },
    {"Radio + TV + NVOD",
     "Radio + TV + NVOD",
     "",
     "Radio + TV + NVOD",
     "Radio + TV + NVOD",
     "",
     "",
     "",
     "radio + TV + NVOD",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Radio + TV + NVOD",                        // �esky (Czech)
     },
    {"Service type",
     "Serviceart",
     "",
     "Tipo servizio",
     "Service type",
     "",
     "",
     "",
     "Haettavat palvelut",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Servisni typ",                        // �esky (Czech)
     },
    {"enabled",
     "aktiviert",
     "",
     "abilitato",
     "ingeschakeld",
     "",
     "",
     "",
     "p��ll�",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Povoleno",                        // �esky (Czech)
     },
    {"disabled",
     "deaktiviert",
     "",
     "disabilitato",
     "uitgeschakeld",
     "",
     "",
     "",
     "pois",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Zakazano",                        // �esky (Czech)
     },
    {"Added new channels",
     "Neue Kan�le hinzugef�gt",
     "",
     "Aggiunti nuovi canali",
     "Nieuwe kanalen toegevoegd",
     "",
     "",
     "",
     "Uudet kanavat lis�tty",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Pridat nove kanaly",                        // �esky (Czech)
     },
    {"Frequency (kHz)",
     "Frequenz (kHz)",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Frekvence (kHz)",                        // �esky (Czech)
     },
    {"Frequency (MHz)",
     "Frequenz (MHz)",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Frekvence (MHz)",                        // �esky (Czech)
     },
    {"Expert",
     "Experten",
     "",
     "Esperto",
     "Expert",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Specialni",                        // �esky (Czech)
     },
    {"Channel sel.",
     "Kanalliste",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Vyber kanalu",                        // �esky (Czech)
     },
    {"DVB-S - Satellite",
     "DVB-S - Satellit",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "DVB-S - Satelit",                        // �esky (Czech)
     },
    {"DVB-C - Cable",
     "DVB-C - Kabel",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "DVB-C - Kabel",                        // �esky (Czech)
     },
    {"DVB-T - Terrestrial",
     "DVB-T - Terrestrisch",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "DVB-T - Terrestial",                        // �esky (Czech)
     },
    {"Press OK to proceede",
     "Dr�cken Sie OK um fortzufahren",
     "",
     "Premere OK per continuare",
     "Druk OK om te vervolgen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Stisknout OK pro pokracovani",                        // �esky (Czech)
     },
    {"Press OK to finish or Exit for new scan",
     "Dr�cken Sie OK zum Beenden oder Exit f�r eine neue Kanalsuche.",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Stisknout OK pro konec nebo Exit pro nove prohledavani",                        // �esky (Czech)
     },
    {"Standard",
     "Standard",
     "",
     "Standard",                // Italiano
     "Standaard",
     "",
     "Standart",
     "",
     "Vakio",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Standardni",                        // �esky (Czech)
     },
    {"Extended",
     "Erweitert",
     "",
     "",
     "",
     "",
     "Pr�cision",
     "",
     "Laaja",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Rozsirene",                        // �esky (Czech)
     },
    {"Retrieving transponder list from %s",
     "Erhalte Transponderliste von %s",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Hrvatski (Croatian)
     "",                        // Eesti
     "",                        // Dansk
     "Ziskat transponder ze seznamu %s",                        // �esky (Czech)
     },
    {"Terrestrial",
     "Terr.",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Terrestrial",                        // �esky (Czech)
     },
    {"Cable",
     "Kabel",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Kabel",                        // �esky (Czech)
     },
    {"Running services on transponder: %i / %i",
     "Aktive Dienste auf Transponder: %i / %i",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Informace o transponderu: %i / %i",                        // �esky (Czech)
     },
    {"Scanning %s (%iMHz)\t%s",
     "Durchsuche %s (%iMHz)\t%s",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani %s (%iMHz)\t%s",                        // �esky (Czech)
     },
    {"Scanning %s (%.3fMHz)\t%s",
     "Durchsuche %s (%.3fMHz)\t%s",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani %s (%.3fMHz)\t%s",                        // �esky (Czech)
     },
    {"Changes Done",
     "�nderung �bernommen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Zmeny provedeny",                        // �esky (Czech)
     },
    {"Changes failed",
     "�nderung fehlgeschlagen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Zmeny neprovedeny",                        // �esky (Czech)
     },
    {"Channel lists",
     "Kanallisten",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Seznam kanalu",                        // �esky (Czech)
     },
    {"Channel lists functions",
     "Kanallisten Funktionen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Please check your Timers!",
     "Bitte Timer �berpr�fen!",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Prosim zkontrolujte nastaveni casovace",                        // �esky (Czech)
     },
    {"Intelligent 6900/6875/6111",
     "Intelligent 6900/6875/6111",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Inteligentni 6900/6875/6111",                        // �esky (Czech)
     },
    {"Try all 6900/6875/6111",
     "Versuche alle 6900/6875/6111",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Zkuste vsechny 6900/6875/6111",                        // �esky (Czech)
     },
    {"Manual",
     "Manuell",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Rucne",                        // �esky (Czech)
     },
    {"Symbol Rates",
     "Symbolraten",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Symbol Rate",                        // �esky (Czech)
     },
    {"Scanning aborted",
     "Scan abgebrochen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Prohledavani zruseno",                        // �esky (Czech)
     },
    {"Enable Logfile",
     "Logfile einschalten",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Logfile povoleno",                        // �esky (Czech)
     },
    {"Skip",
     "Auslassen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Preskocit",                        // �esky (Czech)
     },
    {"Recording is running",
     "Nicht m�glich - Aufnahme l�uft",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "Nahravani je spusteno",                        // �esky (Czech)
     },
    {"Channel List successfully stored",
     "Kanalliste erfolgreich gespeichert.",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Channel List successfully loaded",
     "Kanalliste erfolgreich importiert.",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Functions",
     "Funktionen",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Import current channellist",
     "Aktuelle Kanalliste importieren",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Export current channellist",
     "Aktuelle Kanalliste exportieren",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Backup channel list to...",
     "Sichere aktuelle Kanalliste nach...",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Restore channel list from...",
     "Ersetze aktuelle Kanalliste durch...",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Restore failed!",
     "Ersetzen fehlgeschlagen!",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {"Backup failed!",
     "Sichern der Kanalliste fehlgeschlagen!",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },    
     {"Press OK to go back to rotor settings",
     "Dr�cken sie Ok um zu den Rotoreinstellungen zur�ckzukehren",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",
     "",                        // Eesti
     "",                        // Dansk
     "",                        // �esky (Czech)
     },
    {NULL}
};
