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
 *  scan.c provides scanning through given tansponder lists
 *
 ***************************************************************************/


#include <stdio.h>
#include <time.h>
#ifndef DEVICE_ATTRIBUTES
#include <sys/ioctl.h>
#include <linux/dvb/frontend.h>
#endif

#include <vdr/device.h>
#include <vdr/sources.h>

#include "channelscan.h"
#include "csmenu.h"
#include "scan.h"
#include "debug.h"

//#define SCAN_DELAY 25           // test tp 19.2E 12207/12522
#define SCAN_DELAY 3        // test tp 19.2E 12207/12522
#define DVBS_LOCK_TIMEOUT 2000 // 3000ms earlier

#define DBGSCAN " debug [scan]"

//#define DEBUG_SCAN(format, args...) printf (format, ## args)
#ifndef DEBUG_SCAN
#define DEBUG_SCAN(format, args...)
#endif

#if 0
#define DEBUG_printf(format, args...) printf (format, ## args)
#else
#define DEBUG_printf(format, args...)
#endif
using std::cout;

cScan::cScan()
{
    origUpdateChannels = Setup.UpdateChannels;
    DEBUG_SCAN(DBGSCAN "  %s  \n", __PRETTY_FUNCTION__);

    ::Setup.UpdateChannels = 0; // to prevent VDRs own update
    cTransponders & transponders = cTransponders::GetInstance();
    sourceCode = transponders.SourceCode();

    newChannels = 0;
#ifndef DEVICE_ATTRIBUTES
    fd_frontend = -1;
#endif
    cardnr = -1;
    transponderNr = 0;
    channelNumber = 0;
    frequency = 0;
    foundNum = 0;
    nitScan = false;

    nitFilter_ = NULL;
    PFilter = NULL;
    SFilter = NULL;
    EFilter = NULL;
}

//--------- Destructor ~cScan -----------------------------------

cScan::~cScan()
{
    Cancel(5);

    DEBUG_SCAN(DBGSCAN "  %s  \n", __PRETTY_FUNCTION__);
    ::Setup.UpdateChannels = origUpdateChannels;


    if (cMenuChannelscan::scanState <= ssGetChannels)
        cMenuChannelscan::scanState = ssInterrupted;
    scanning_on_receiving_device = false;

#ifndef DEVICE_ATTRIBUTES
    if (fd_frontend > 0)
        close(fd_frontend);
#endif

    if (nitFilter_) {
        cDevice::GetDevice(cardnr)->Detach(nitFilter_);
        delete nitFilter_;
        nitFilter_ = NULL;
    }

    if (PFilter) {
        cDevice::GetDevice(cardnr)->Detach(PFilter);
        delete PFilter;
        PFilter = NULL;
    }
    if (SFilter) {
        cDevice::GetDevice(cardnr)->Detach(SFilter);
        delete SFilter;
        SFilter = NULL;
    }
    if (EFilter) {
        cDevice::GetDevice(cardnr)->Detach(EFilter);
        delete EFilter;
        EFilter = NULL;
    }
    DEBUG_SCAN("DEBUG [channelscan]  %s end cTransponders::Destroy(); \n", __PRETTY_FUNCTION__);
}

void cScan::ShutDown()
{
    if (cMenuChannelscan::scanState <= ssGetChannels)
        cMenuChannelscan::scanState = ssInterrupted;

    scanning_on_receiving_device = false;
    Cancel(5);
}

bool cScan::StartScanning(cScanParameters * scp)
{

    // activate Network Information  on auto scan
    //  assume nit scan
    scanParameter_ = *scp;
    if (scp->frequency == 1) {
      if (scp->type == SAT || scp->type == SATS2)   // we don`t differ between  S and S2 here
        if (scp->nitscan == 1) {  nitScan = true;
         } else { nitScan = false;
             }	    
#if 1
        if (scp->type == TERR)
            nitScan = true;
#endif
    }

    DEBUG_SCAN(DBGSCAN "  %s  %s \n", __PRETTY_FUNCTION__,
               nitScan ? "AUTO" : "MANUELL");

    cTransponders & transponders = cTransponders::GetInstance();
    // nit scan takes transponder lists from SI Network Information Table

    if (transponders.GetNITStartTransponder()) {
        DEBUG_SCAN(DBGSCAN " tp --   ssGetTransponders (NIT) \n");
        cMenuChannelscan::scanState = ssGetTransponders;
    } else {
        DEBUG_SCAN(DBGSCAN " tp --   ssGetChannels  \n");
        cMenuChannelscan::scanState = ssGetChannels;
    }

    if (cMenuChannelscan::scanState != ssGetTransponders && transponders.v_tp_.size() == 0) {
        DEBUG_SCAN(DBGSCAN "  %s   return FALSE  \n", __PRETTY_FUNCTION__);
        esyslog(ERR " Empty Transponderlist");
        cMenuChannelscan::scanState = ssNoTransponder;
        return false;
    }

    //  terrestrial auto
    detailedSearch = scp->detail;
    //  cable auto
    srModes = scp->symbolrate_mode;     ///< auto, all fix

    // Reset internal scan states
    lastLocked = 1;             // for safety
    srstat = -1;

    cDevice::PrimaryDevice()->StopReplay();

    if(cPluginManager::GetPlugin("mcli"))
        cardnr = scp->type;
    else
        cardnr = scp->card;
    DEBUG_SCAN(DBGSCAN " Stop Replay Card Number %d", cardnr);


    DEBUG_SCAN(" \033[0;43m SET DEBUG [scan.c] scanning_on_receiving_device   TRUE \033[0m\n");
    scanning_on_receiving_device = true;

#ifndef DEVICE_ATTRIBUTES
    char buffer[265];
    snprintf(buffer, sizeof(buffer), "/dev/dvb/adapter%d/frontend0", cardnr);

    fd_frontend = open(buffer, O_RDONLY | O_NONBLOCK);
    if (fd_frontend <= 0) {
        esyslog("cant open device: %s ", buffer);
        cMenuChannelscan::scanState = ssDeviceFailure;
        return false;
    }
#endif

    Start();
    return true;
}

//-------------------------------------------------------------------------
uint16_t cScan::getSignal()
{
    uint16_t value = 0;
#ifndef DEVICE_ATTRIBUTES
    CHECK(ioctl(fd_frontend, FE_READ_SIGNAL_STRENGTH, &value));
#else
    if (device) {
	    uint64_t v=0;
        device->GetAttribute("fe.signal",&v);
        value = v;
    }
#endif
    return value;
}

uint16_t cScan::getSNR()
{
    uint16_t value = 0;
#ifndef DEVICE_ATTRIBUTES
    CHECK(ioctl(fd_frontend, FE_READ_SNR, &value));
#else
    if (device) {
	    uint64_t v=0;
        device->GetAttribute("fe.snr",&v);
        value = v;
    }
#endif
    return value;
}

uint16_t cScan::getStatus()
{
    fe_status_t value;
    memset(&value, 0, sizeof(fe_status_t));
#ifndef DEVICE_ATTRIBUTES
    CHECK(ioctl(fd_frontend, FE_READ_STATUS, &value));
#else
    if (device) {
	    uint64_t v=0;
        device->GetAttribute("fe.status",&v);
        value = (fe_status_t)v;
    }
#endif
    return value;
}

//-------------------------------------------------------------------------

void cScan::ScanServices()
{
  //const time_t  tt = time(NULL);
  time_t tt_;
  char *strDate;

 // DEBUG_printf("\n%s\n",__PRETTY_FUNCTION__);
    cMenuChannelscan::scanState = ssGetChannels;
#ifdef WITH_EIT
    DEBUG_SCAN("DEBUG [channelscan]: With EIT\n");
    EFilter = new cEitFilter();
#endif
    PFilter = new PatFilter();
    SFilter = new SdtFilter(PFilter);
    PFilter->SetSdtFilter(SFilter);

    device->AttachFilter(SFilter);
    device->AttachFilter(PFilter);
#ifdef WITH_EIT
    device->AttachFilter(EFilter);
#endif

    time_t start = time(NULL);

    int foundSids = 0;
    foundNum = totalNum = 0;
    // Heuristic: Delay scan timeout if Sids or Services withs PIDs are found
  tt_ = time(NULL);
  DEBUG_printf("\n%s beforeloop:%4.2fs:",__PRETTY_FUNCTION__,(float)difftime(tt_,tt));
int i = 0;
    while (!PFilter->EndOfScan() && (
                                        //(time(NULL) - start < SCAN_DELAY && cMenuChannelscan::scanning) ||
                                (time(NULL) - start < SCAN_DELAY
                                 && cMenuChannelscan::scanState == ssGetChannels
//                                         || (time(NULL) -PFilter->LastFoundTime() < SCAN_DELAY)
)))
    {

        PFilter->GetFoundNum(foundNum, totalNum);
        if (totalNum && !foundSids)
        {
            start = time(NULL);
            foundSids = 1;
        }
        //DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
        usleep(200 * 1000);
        //usleep(0 * 1000); // inside loop
        // no difference in time/performance when usleep is commented out

  i++;
  tt_ = time(NULL);
  DEBUG_printf("\n%s %d INloop:%4.2fs:",__PRETTY_FUNCTION__,i, (float)difftime(tt_,start));
    }
  tt_ = time(NULL);
  DEBUG_printf("\n%s afterloop:%4.2fs",__PRETTY_FUNCTION__,(float)difftime(tt_,tt));

/* if (cMenuChannelscan::scanState >= ssInterrupted && !PFilter->EndOfScan())
    {
        DEBUG_SCAN("DEBUG [channelscan]: ScanServices aborted %d \n",
                   cMenuChannelscan::scanState);
    }
*/
  tt_ = time(NULL);
  DEBUG_printf("\n%s afterIF endofscan() :%4.2fs:",__PRETTY_FUNCTION__,(float)difftime(tt_,tt));

    //DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
   // usleep(200 * 1000);
    PFilter->GetFoundNum(foundNum, totalNum);
    tt_ = time(NULL);
    DEBUG_printf("\n%s after GetFountNum() :%4.2fs:",__PRETTY_FUNCTION__,(float)difftime(tt_,tt));

    device->Detach(PFilter);
    device->Detach(SFilter);
#ifdef WITH_EIT
    device->Detach(EFilter);
#endif
    tt_ = time(NULL);
    DEBUG_printf("\n%s after detach filters :%4.2fs:",__PRETTY_FUNCTION__,(float)difftime(tt_,tt));

    if (PFilter)
        delete PFilter;
    PFilter = NULL;
    if (SFilter)
        delete SFilter;
    SFilter = NULL;
    if (EFilter)
        delete EFilter;
    EFilter = NULL;
    if (nitScan) {
        esyslog("%d: nitscan",nitScan);
        ScanNitServices();      // XXX
    }
  const time_t ttout = time(NULL);
  asprintf(&strDate,"%s", asctime(localtime(&ttout)));
  DEBUG_printf("\n%s OUT:%4.2fms: %s",__PRETTY_FUNCTION__,(float)difftime(ttout,tt)*1000,strDate);
}

//-------------------------------------------------------------------------
void cScan::ScanNitServices()
{
    esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);
    DEBUG_SCAN("\033[0;41m DEBUG [cs]; ScanNITService \033[0m\n");
    nitFilter_ = new NitFilter;
    time_t start = time(NULL);

    device->AttachFilter(nitFilter_);

    // updates  status bar in cMenuScanActive
    if (cMenuChannelscan::scanState == ssGetTransponders)
        transponderNr = 0;

 
    //esyslog ("%s starting nitFilter loop",__FUNCTION__);
    while (!nitFilter_->EndOfScan() &&
           (time(NULL) - start < (SCAN_DELAY*4) &&
            (cMenuChannelscan::scanState == ssGetTransponders ||
             cMenuChannelscan::scanState == ssGetChannels)))
    {
        //esyslog ("%s IN nitFilter loop",__FUNCTION__);


        if (nitFilter_->Found()) {
            start = time(NULL);
    	    esyslog("%s ",__FUNCTION__);
        }
        // we don`t know how many tranponder we get
        if (cMenuChannelscan::scanState == ssGetTransponders) {
            transponderNr = (int)(time(NULL) - start);
        }
        DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
        usleep(200 * 1000); // inside loop
    }

    esyslog ("%s OUT nitFilter loop",__FUNCTION__);
    device->Detach(nitFilter_);

    if (nitFilter_)
        delete nitFilter_;
    nitFilter_ = NULL;

    // display 100% in status bar
    // while (initalscan &&  transponderNr < 100 && cMenuChannelscan::scanning)

    while (transponderNr < 100 && cMenuChannelscan::scanState == ssGetTransponders) {
         DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
         usleep(10*1000); // inside loop: only counter incremented!
                              // sleep longer so the progress bar doesnot start from 0 again
         transponderNr += 2;
    }
    if (cMenuChannelscan::scanState == ssGetTransponders) {
        transponderNr = 0;
        printf("\n%s transponderNr SET to zero\n",__PRETTY_FUNCTION__);
    }

}

static const char *getUserString(int Value, const tDvbParameterMap *Map)
{
  const tDvbParameterMap *map = Map;
  while (map && map->userValue != -1) {
        if (map->driverValue == Value)
           return map->userString ? trVDR(map->userString) : "---";
        map++;
        }
  return "---";
}

cString getCoderate(int value)
{
  return cString::sprintf("%s", getUserString(value, CoderateValues));
}

cString getModulation(int value)
{
  return cString::sprintf("%s", getUserString(value, ModulationValues));
}

//-------------------------------------------------------------------------

void cScan::ScanDVB_S(cTransponder * tp, cChannel * c)
{
 //const time_t tt = time(NULL);
  int maxmods = device->NumProvidedSystems() == 2? 4 : 2;

   // esyslog("%s cTransponder* tp = %x  cChannel *c = %x", __PRETTY_FUNCTION__);
    esyslog("maxmods = %d",maxmods);

    if (maxmods == 4)
        DEBUG_SCAN("\033[0;41m DEBUG [cs]: ScanDVB_S2 \033[0m\n");
        ;

    // skip HD Transonders on SD Tuner
    if ( !device->NumProvidedSystems() == 2 && static_cast < cSatTransponder * >(tp)->System() == 1)
        return;

  unsigned  int nRadio = radioChannelNames.size();
  unsigned  int nTv = tvChannelNames.size();

    for (int mod = 0; mod < maxmods; mod++) {
        if (tp->Scanned())
            break;
        switch (mod) {
        case 0:                //we do not overwrite given
            if (tp->Modulation() == 999) {
                tp->SetModulation(QPSK);
            static_cast < cSatTransponder * >(tp)->SetSystem(SYS_DVBS);		
            static_cast < cSatTransponder * >(tp)->SetRollOff(ROLLOFF_35);
             //   static_cast < cSatTransponder * >(tp)->SetFEC(FEC_AUTO);
            } else if (tp->Modulation() == QPSK) {
             //   static_cast < cSatTransponder * >(tp)->SetFEC(FEC_AUTO);
            static_cast < cSatTransponder * >(tp)->SetSystem(SYS_DVBS);
            static_cast < cSatTransponder * >(tp)->SetRollOff(ROLLOFF_35);
            }

            DEBUG_SCAN(" DEBUG [DVB_S]; trying QPSK: freq: %d srate:%d   mod: %d  \n",
                       tp->Frequency(), tp->Symbolrate(), tp->Modulation());
            break;
        case 1:
            tp->SetModulation(QPSK);
            break;

        case 2:
            tp->SetModulation(PSK_8);
            //static_cast < cSatTransponder * >(tp)->SetFEC(FEC_2_3);
            static_cast < cSatTransponder * >(tp)->SetRollOff(ROLLOFF_35);
	    static_cast < cSatTransponder * >(tp)->SetSystem(SYS_DVBS2);
            DEBUG_SCAN (" DEBUG [DVB_S]; trying PSK8: freq: %d srate:%d   mod: %d  \n", tp->Frequency(), tp->Polarisation(), tp->Symbolrate(), tp->Modulation());

            break;
        case 3:
            tp->SetModulation(QPSK);
            //static_cast < cSatTransponder * >(tp)->SetFEC(FEC_9_10);
            static_cast < cSatTransponder * >(tp)->SetRollOff(ROLLOFF_35);
            static_cast < cSatTransponder * >(tp)->SetSystem(SYS_DVBS2);	    
            DEBUG_SCAN (" DEBUG [DVB_S]; trying QPSK_S2: freq: %d srate:%d   mod: %d  \n", tp->Frequency(), tp->Symbolrate(), tp->Modulation());
            break;
        default:
	    DEBUG_SCAN("No such modulation %d", mod);
            break;
        }
        tp->SetTransponderData(c, sourceCode);


        DEBUG_SCAN ("\033[0:44m DEBUG [DVB_S] - channel: freq: %d pol: %c  srate:%d   mod: %d  \033[0m\n", c->Frequency(), c->Polarization(), c->Srate(), c->Modulation());

#if VDRVERSNUM >= 10713
        cDvbTransponderParameters dtp(c->Parameters());
#endif
        if (!device->SwitchChannel(c, false)) {
            esyslog(ERR "SwitchChannel(%d)  failed", c->Frequency());
            DEBUG_SCAN(ERR "SwitchChannel(%d)  failed\n", c->Frequency());
/* #if HAVE_ROTOR
            esyslog(ERR "try Switch rotor ");
            DEBUG_SCAN(ERR " try Switch rotor \n");

            struct
            {
                cDevice *device;
                cChannel *channel;
            } data;

            data.device = device;
            data.channel = &c;

            cPlugin *p = cPluginManager::GetPlugin("rotor");
            if (p)
            {
                isyslog("Info [channelscan] Switch rotor \n");
                Plugin->Service("Rotor-SwitchChannel", &data);
                DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
                usleep(100 * 1000); // inside loop, inside if // wait for rotor
            }
#endif */

        } else {                  // switch succeeded
            if (cDevice::GetDevice(cardnr)->HasLock(DVBS_LOCK_TIMEOUT)) {
              //esyslog("...switchchannel else && HasLock()");
#if VDRVERSNUM >= 10713
                DEBUG_SCAN
                    ("\033[0;44m  debug  [scan] -- Got LOCK @ transponder %d  mod  %s fec %s  ---------- \033[0m\n",
                     c->Transponder(), *getModulation(dtp.Modulation()), *getCoderate(dtp.CoderateH()));
#else
                DEBUG_SCAN
                    ("\033[0;44m  debug  [scan] -- Got LOCK @ transponder %d  mod  %d fec %d  ---------- \033[0m\n",
                     c->Transponder(), c->Modulation(), c->CoderateH());
#endif

#if VDRVERSNUM >= 10713
                dsyslog
                    ("[scan] -- Got LOCK @ transponder %d  mod  %s fec %s  ---------- \033[0m\n",
                     c->Transponder(), *getModulation(dtp.Modulation()), *getCoderate(dtp.CoderateH()));
#else
                dsyslog
                    ("[scan] -- Got LOCK @ transponder %d  mod  %d fec %d  ---------- \033[0m\n",
                     c->Transponder(), c->Modulation(), c->CoderateH());
#endif

                if (cMenuChannelscan::scanState == ssGetTransponders) {
                    esyslog("Starting ScanNitServices");
                    ScanNitServices();
                } else if (cMenuChannelscan::scanState == ssGetChannels) {
                  DEBUG_printf("\nCalling ScanServices()\n");
                    ScanServices();
                    DEBUG_printf("\nScanServices Done\n");
                }
                DEBUG_printf("\n nTv=%d  tvChannelNames.size()=%d\n",nTv, tvChannelNames.size());

                if (nRadio < radioChannelNames.size() || nTv < tvChannelNames.size()) {
                  tp->SetScanned();
                  DEBUG_printf("\nSET SCANNED\n");
                }
            } else {
#if VDRVERSNUM >= 10713
                DEBUG_SCAN ("\033[0;48m  debug  [scan] -- NO LOCK @ transponder %d  mod  %s fec %s ---------- \033[0m\n",
                     c->Transponder(), *getModulation(dtp.Modulation()), *getCoderate(dtp.CoderateH()));
#else
                DEBUG_SCAN ("\033[0;48m  debug  [scan] -- NO LOCK @ transponder %d  mod  %d fec %d ---------- \033[0m\n",
                     c->Transponder(), c->Modulation(), c->CoderateH());
#endif
                dsyslog (" debug  [channelscan] ------ NO LOCK after %ims @ transponder %d  -------------\n",
                     DVBS_LOCK_TIMEOUT, c->Transponder());
               // return;
            }

        }
    }
  const time_t ttout = time(NULL);
  char *strDate;
  asprintf(&strDate,"%s", asctime(localtime(&ttout)));
  DEBUG_printf("\n%s OUT:%4.2fsec: %s",__PRETTY_FUNCTION__,(float)difftime(ttout,tt),strDate);
}

//-------------------------------------------------------------------------

// detail bit 0: wait longer
// detail bit 1: search also +-166kHz offset

void cScan::ScanDVB_T(cTransponder * tp, cChannel * c)
{
    int timeout = 1000;
    int retries = 0;
    int response, n, m;
    int frequency_orig = tp->Frequency();
    int offsets[3] = { 0, -166666, 166666 };

    tp->SetFrequency(frequency_orig);

    for (n = 0; n < (detailedSearch & 2 ? 3 : 1); n++) {

        DLOG("DEBUG [channelscan]:  DVT_T detailedSearch %X ",
             detailedSearch);
        if (cMenuChannelscan::scanState >= ssInterrupted)
            return;

        tp->SetFrequency(frequency_orig + offsets[n]);
        frequency = tp->Frequency();
        tp->SetTransponderData(c, sourceCode);

        // retune with offset
        if (!device->SwitchChannel(c, false))
            esyslog(ERR "SwitchChannel(c)  failed");

        DLOG("%i Tune %i \n", retries, frequency);
        DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
        usleep(1500 * 1000); // inside loop
        if (lastLocked) {
          DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
          sleep(2);           // avoid false lock
        }
        //
        for (m = 0; m < (detailedSearch & 1 ? 8 : 2); m++) {
            response = getStatus();
            DLOG("DEBUG [channelscan]:  DVT_T detailedSearch  %X  m %d ",
                 detailedSearch, m);
            DLOG("%i RESPONSE %x\n", retries, response);

            if ((response & 0x10) == 0x10) {                   // Lock
                break;
            }

            if ((response & 15) > 2) {    // Almost locked, give it some more time
              DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
              sleep(1);
            }
            DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
            sleep(1);
        }

        if (cMenuChannelscan::scanState >= ssInterrupted) {
            break;
        }

        if (device->HasLock(timeout)) {
            DLOG(DBG "  ------ HAS LOCK ------");
            ScanServices();
            lastLocked = 1;
            return;
        }
        lastLocked = 0;
        retries++;
    }
}

//-------------------------------------------------------------------------
/*
  Scan algorithm for DVB-C:

  Try 256QAM after 64QAM only if signal strength is high enough
  If initial mod is != 64QAM, then no 64QAM/256QAM auto detection is done

  "intelligent symbolrate scan": srModes  0
  Try 6900/6875/6111 until lock is achieved, then use found rate for all subsequent scans

  "try all  symbolrate scan": srModes  1
  Try 6900/6875/6111  on every transponder

  "fixed symbolrate scan": srModes 2 on manual scan

  Wait additional 3s when last channel had lock to avoid false locking
 */

void cScan::ScanDVB_C(cTransponder * tp, cChannel * c)
{
    int timeout = 500;
    int str1;
    int srtab[3] = { 6900, 6875, 6111 };
    int fixedModulation = 0;

    frequency = tp->Frequency();
    DLOG("DEBUG channelscan DVB_C f:  %f, Modulation %d SR %d\n",
         frequency / 1e6, tp->Modulation(), tp->Symbolrate());

    if (tp->Modulation() != QAM_64)
        fixedModulation = 1;

    for (int sr = 0; sr < 3; sr++) {
        DEBUG_SCAN("DEBUG [channelscan]: srModes %d ", srModes);

        switch (srModes) {
        case 0:                // auto
            if (srstat != -1)   // sr not found
            {
                DEBUG_SCAN("Use default symbol rate %i\n", srtab[srstat]);
                DEBUG_SCAN("Use  probed  symbol rate  %i\n", srtab[srstat]);
                tp->SetSymbolrate(srtab[srstat]);
                break;
            }
            // there is intensionaly no break !!!
        case 1:                // try all  symbol rates
            DEBUG_SCAN("try symbol rate  %i\n", srtab[sr]);
            tp->SetSymbolrate(srtab[sr]);
            break;
        case 2:                // fix sybol rate
            DEBUG_SCAN("try symbol rate  %i\n", tp->Symbolrate());
            /// already set
            break;
        default:
            DEBUG_SCAN(" Erorr, check csmenu.c DVB_C \n");
            break;

        }
        // Reset modulation
        if (!fixedModulation)
            tp->SetModulation(QAM_64);

        // try 64QAM/256QAM
        for (int mods = 0; mods < 2; mods++)
        {
            if (!cMenuChannelscan::scanState == ssGetChannels)
                return;

            tp->SetTransponderData(c, sourceCode);

            if (!device->SwitchChannel(c, false)) {
                esyslog(ERR "SwitchChannel(c)  failed");
            }
            DEBUG_printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
            usleep(500 * 1000); // inside loop

            str1 = getSignal();

            if (lastLocked) {
                DEBUG_SCAN("Wait last lock %d \n", mods);
                DEBUG_printf("\nSleeping 3s %s %d",__PRETTY_FUNCTION__,__LINE__);
                sleep(3);       // avoid stiky last lock
            }

            if (device->HasLock(timeout)) {
                DLOG(DBG "  ------------- HAS LOCK -------------     ");
                //DEBUG_SCAN("LOCK @ %.1f\mods",tp->Frequency()/1.0e6);
                ScanServices();
                lastLocked = 1;

                if (srModes == 0 && srstat == -1)
                    srstat = sr;        // remember SR for the remaining scan

                return;
            }

            lastLocked = 0;

            if (!fixedModulation && mods == 0 /*TB && str1 > 0x4000*/) {                   //>  25% STR
                /* TB: (str1>0x4000) causes that prem HD isn't found here */
                //DEBUG_SCAN("QAM256 %x\mods",str1);
                tp->SetModulation(QAM_256);
            } else {
                break;
            }
        }
        // leave SR try loop without useable signal strength (even in "try all" mode)
        if (/*TB str1 < 0x4000 ||*/ (srModes == 0 && srstat != -1) || srModes == 2)
            break;
    }
}

//---------  cMainMenu::Action()  -----------------------------------

void cScan::Action()
{
    // the one and only "scanning = true" !
            
    int i = 0;
    if (cPluginManager::GetPlugin("mcli")) {
        int sys = cardnr;
        cardnr = 0;
        bool gotIt = false;

        device = cDevice::GetDevice(cardnr);
        while (device) {

            switch (sys) {
                case 0: //TERR
                    if (device->ProvidesSource(cSource::stTerr))
                        gotIt = true;
                    break;
                case 1: // CABLE
                    if (device->ProvidesSource(cSource::stCable))
                        gotIt = true;
                    break;
                case 2: // SAT
                    if (device->ProvidesSource(cSource::stSat))
                        gotIt = true;
                    break;
                case 3: // SATS2
                    if (device->ProvidesSource(cSource::stSat))
                        gotIt = true;
                    break;
                default:
                    break;
            }
            if(gotIt == true)
                break;
            i++;
            device = cDevice::GetDevice(++cardnr);
        }
    } else
        device = cDevice::GetDevice(cardnr);
    std::auto_ptr < cChannel > c(new cChannel);

    cTransponders & transponders = cTransponders::GetInstance();

    transponderMap.clear();
    hdTransponders.clear();
    TblVersions.clear();

#define EXTRA_FILTERS 1
#if EXTRA_FILTERS
    // FIX for empty channellist 
    // Add a filter - so, netceiver can access PID with OpenFilter() and have a tuner-lock

#define PAT_F 0
#if PAT_F
    PatFilter* tmp_pfilter=NULL;
    SdtFilter* tmp_sfilter=NULL;
#endif
    NitFilter* tmp_nfilter=NULL;

    //Attaching filters to devices not necessary for this FIX ? 
    if (nitScan) // not for manual scan
    {
      tmp_nfilter = new NitFilter;
      device -> AttachFilter(tmp_nfilter);
    }
#if PAT_F
    else
    {   /* manual scan, then start these filters*/
      tmp_pfilter = new PatFilter;
      tmp_sfilter = new SdtFilter(tmp_pfilter);
      tmp_pfilter -> SetSdtFilter(tmp_sfilter);

      device -> AttachFilter(tmp_pfilter);
      device -> AttachFilter(tmp_sfilter);
      tmp_pfilter->SetStatus(false);
      tmp_sfilter->SetStatus(false);
    }
#endif
     // end FIX
#endif

    if (cMenuChannelscan::scanState == ssGetTransponders) {
        cTransponder *t = transponders.GetNITStartTransponder();
        if (t && device->ProvidesSource(cSource::stSat)) {
            // fetch sat Transponders
//           esyslog("%s !!!!!! FETCH tps START !!!!! ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);
           // ScanDVB_S(t, c.get()); // why ScanDVB_S() when ScanNitServices() gets the tp list!?
          /* TB: feed a dummy channel into VDR so it doesn't turn the rotor back to it's old position */
#ifdef REELVDR
//          cChannel dummyChan;
//          dummyChan.SetNumber(1);
//          dummyChan.SetSatTransponderData(scanParameter_.source, 9 /*scanParameter_.frequency*/, scanParameter_.polarization, scanParameter_.symbolrate, scanParameter_.bandwidth);
          //MoveRotor(cardnr, scanParameter_.source);
//          device->SetChannel(&dummyChan, false, false);
//          WaitForRotorMovement(cardnr);
#endif
          ScanNitServices();
//             esyslog("%s !!!!!! FETCH tps END !!!!! ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);
        }

        if (cMenuChannelscan::scanState == ssGetTransponders) {
            if (transponders.v_tp_.size() < 1) {
                // give up,  load legacy transponder list
                scanParameter_.frequency = 2;
                transponders.Load(&scanParameter_);
            }

            cMenuChannelscan::scanState = ssGetChannels;
            AddTransponders();
        } else {
          
#if EXTRA_FILTERS
          if(tmp_nfilter) {
            device->Detach(tmp_nfilter);
            delete tmp_nfilter;
          }
#if PAT_F
          if(tmp_pfilter) { device->Detach(tmp_pfilter); delete tmp_pfilter; }
          if(tmp_sfilter) { device->Detach(tmp_sfilter); delete tmp_sfilter; }
#endif
          
#endif
            return;
        }
    }

    constTpIter tp = transponders.v_tp_.begin();

    time_t startTime = time(NULL);

    while (Running() && cMenuChannelscan::scanState == ssGetChannels) {


        DEBUG_SCAN ("\033[0;34m DEBUG [channelscan]: loop through  transponders ++++  size %d ++++++++\033[0m\n",
             (int)transponders.v_tp_.size());
        printf("\n\033[0;34m DEBUG [channelscan]: loop through  transponders ++++  size %d ++++++++\033[0m\n", (int)transponders.v_tp_.size());
        unsigned int oldTransponderMapSize = transponderMap.size();
        int i=1, ntv,nradio;
        time_t t_in,t_out;
	    std::vector < cTransponder * > scannedTps;
        while (tp != transponders.v_tp_.end()) {
            //if (!cMenuChannelscan::scanning) break;
            if (cMenuChannelscan::scanState != ssGetChannels)
                break;
            ntv = tvChannelNames.size();
            nradio = radioChannelNames.size();

            // get counter
            transponderNr = tp - transponders.v_tp_.begin();
            channelNumber = (*tp)->ChannelNum();
            frequency = (*tp)->Frequency();
            DEBUG_SCAN
                ("\033[0;44m DEBUG [channelscan]: scan f: %d  \033[0m \n",
                 frequency);

            if (device->ProvidesSource(cSource::stTerr))
                ScanDVB_T(*tp, c.get());
            else if (device->ProvidesSource(cSource::stSat)) {

              DEBUG_printf("\nCalled ScanDVB_S() \n");
              t_in = time(NULL);
	      // loop over a previously scanned transponders
	      bool alreadyScanned = false;
	      FILE *fp = fopen ("/tmp/alreadyScanned.txt","a");
	      for (constTpIter prevTp=scannedTps.begin(); prevTp!=scannedTps.end(); prevTp++)
                      /* TB: checking equality of frequencies is not enough! */
                      /*     Polarization has also to be checked... */
                      /*     Modulation and FEC also ... */
		      if((dynamic_cast < cSatTransponder * >(*prevTp))->FEC() == (dynamic_cast < cSatTransponder * >(*tp))->FEC() && (dynamic_cast < cSatTransponder * >(*prevTp))->Modulation() == (dynamic_cast < cSatTransponder * >(*tp))->Modulation() &&  (dynamic_cast < cSatTransponder * >(*prevTp))->Polarization() == (dynamic_cast < cSatTransponder * >(*tp))->Polarization() && /*(*tp)->SourceCode() ==  (*prevTp)->SourceCode() && */  abs((*prevTp)->Frequency() - frequency) <= 1  ) // from the same SAT source ?
		      {
		      alreadyScanned = true;
		      printf("\t(%s:%d) %d Mhz already scanned\n", __FILE__,__LINE__, frequency);
		      fprintf(fp,"(%s:%d) %d Mhz already scanned\n", __FILE__,__LINE__, frequency);
		      break;
		      }
	      fclose(fp);

	      if( alreadyScanned == false ) {
		      ScanDVB_S(*tp, c.get());
		      scannedTps.push_back(*tp);
	      }

              t_out = time(NULL);
              fp = fopen("/tmp/cScan.log","a");
              const time_t tt = time(NULL);
              char *strDate;
              asprintf(&strDate,"%s", asctime(localtime(&tt)));
              strDate[strlen(strDate)-1] = 0;
              fprintf(fp,"\n\n%s tp=%4d, %6d(%d) TV:%4d Radio:%4d in %3d sec",strDate,i , frequency, !alreadyScanned, tvChannelNames.size()-ntv, radioChannelNames.size()-nradio, (int)difftime(t_out,t_in) );
              fclose(fp);

              fp = fopen("/tmp/tScan.log","a");
              //fprintf(fp,"\n\n%s tp=%4d, %6d/%2d/%5d TV:%4d Radio:%4d in %3dsec",strDate,i , frequency,(*tp)->Modulation(), (*tp)->Symbolrate(), tvChannelNames.size()-ntv, radioChannelNames.size()-nradio, (int)difftime(t_out,t_in) );
              fprintf(fp,"\n\ntp=%4d, %6d/%2d/%5d TV:%4d Radio:%4d in %3dsec new:%3d",i , frequency,(*tp)->Modulation(), (*tp)->Symbolrate(), tvChannelNames.size()-ntv, radioChannelNames.size()-nradio, (int)difftime(t_out,t_in),tvChannelNames.size()-ntv+ radioChannelNames.size()-nradio );
              fclose(fp);

              free(strDate);
            }
            else if (device->ProvidesSource(cSource::stCable))
                ScanDVB_C(*tp, c.get());

            ++tp;
            ++i; // tp number
        }

        if (cMenuChannelscan::scanState <= ssGetChannels && transponderMap.size() > oldTransponderMapSize) {
            size_t pos = transponders.v_tp_.size();
            AddTransponders();
            tp = transponders.v_tp_.begin() + pos;
        } else {
            break;
        }
    }

#if EXTRA_FILTERS
        if(tmp_nfilter) {
      device->Detach(tmp_nfilter);
      delete tmp_nfilter;
    }
#if PAT_F
    if(tmp_pfilter) { device->Detach(tmp_pfilter); delete tmp_pfilter; }
    if(tmp_sfilter) { device->Detach(tmp_sfilter); delete tmp_sfilter; }
#endif

// end fix3
#endif    
    
    int duration = time(NULL) - startTime;
    DEBUG_SCAN (" --- End of transponderlist. End of scan! Duratation %d ---\n", duration);
    // avoid warning wihout debug
    duration = 0;


    if (cMenuChannelscan::scanState == ssGetChannels)
        cMenuChannelscan::scanState = ssSuccess;

    //else cMenuChannelscan::scanState = ssInterrupted;
    //cMenuChannelscan::scanning = false;

    DLOG(DBG " End of scan scanState: ssSuccess!");


    DumpHdTransponder();
    ClearMap();
}

//-------------------------------------------------------------------------

void cScan::AddTransponders()
{
    DEBUG_SCAN(" --- AddTransponders \n");
    cTransponders & transponders = cTransponders::GetInstance();
    for (tpMapIter itr = transponderMap.begin(); itr != transponderMap.end(); ++itr) {
        if (cMenuChannelscan::scanState == ssGetTransponders || transponders.MissingTransponder(itr->first)) {
            cSatTransponder *tp = dynamic_cast < cSatTransponder * >(itr->second);
            DEBUG_printf("%d=", cnt++);
            tp->PrintData();
            transponders.v_tp_.push_back(itr->second);
        }
    }
}

void
cScan::DumpHdTransponder()
{

    printf(" Assumed HD Transponders \n");
    while (!hdTransponders.empty()) {

        std::map < int, cSatTransponder >::const_iterator it;
        cSatTransponder t = hdTransponders.begin()->second;

        DEBUG_SCAN
            ("HD transponder : %6d -> mod %d srate %d  fec %d rolloff %d \n",
             (*hdTransponders.begin()).first, t.Modulation(), t.Symbolrate(),
             t.FEC(), t.RollOff());

        hdTransponders.erase(hdTransponders.begin());
    }

}

void cScan::ClearMap()
{
    cTransponders::GetInstance().Clear();

    while (!transponderMap.empty()) {
        if (transponderMap.begin()->second) {
            DEBUG_SCAN("delete tp: %6d\n", (*transponderMap.begin()).first);
            //delete transponderMap.begin()->second;
        } else
            DEBUG_SCAN(" tp: %6d deleted already\n", (*transponderMap.begin()).first);

        transponderMap.erase(transponderMap.begin());
    }
}
