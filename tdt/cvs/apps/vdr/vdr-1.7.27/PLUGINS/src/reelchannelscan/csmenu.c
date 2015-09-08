/***************************************************************************
 *   Copyright (C) 2005 by Reel Multimedia;  Author  Markus Hahn           *
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
 *   csmenu.c user interaction
 *
 ***************************************************************************/

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include <linux/dvb/frontend.h>

#include <vdr/interface.h>
#include <vdr/diseqc.h>
#include <vdr/menu.h>
#include <vdr/sources.h>

#include "channelscan.h"
#include "csmenu.h"
#include "filter.h"
#include "channellistbackupmenu.h"
//#include "rotortools.h"

#include <bzlib.h>
#include <zlib.h>

#define CHNUMWIDTH 16
#define DVBFRONTEND "/dev/dvb/adapter%d/frontend0"
#define POLLDELAY 1
#define MENUDELAY 3

#define DISEQC_SOURCE_ASTRA_19 35008

using std::ofstream;
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;


#define DBGM " debug [cs menu]"
#define DEBUG_CSMENU(format, args...) printf (format, ## args)
//#define DEBUG_CSMENU(format, args...)
//

// XXX check all static vars!
typedef vector < int >::const_iterator iConstIter;

vector < string > tvChannelNames;
vector < string > radioChannelNames;
vector < string > dataChannelNames;

cMutex mutexNames;
cMutex mutexScan;

volatile int cMenuChannelscan::scanState = ssInit;

int cMenuChannelscan::currentTuner = 0; // static ?? XXX
cTransponders* cTransponders::instance_ = NULL;

int pollCount;
extern bool OnlyChannelList;
int Ntv=0, Nradio=0;

// --- Class cMenuChannelscan ------------------------------------------------

cMenuChannelscan::cMenuChannelscan(int src, int freq, int symrate, char  pol)
:source(src), frequency(freq), symbolrate(symrate), polarization(pol),
cOsdMenu(tr("Channelscan"), CHNUMWIDTH)
{
    data = Setup;
    cTransponders::Create();

    detailedScan = 0;

    scanState = 0;
    detailFlag = 0;             //DVB_T
    expertSettings = 0;
    srcTuners = 0;
    if (cPluginManager::GetPlugin("mcli"))
        TunerDetectionMcli();
    else
        TunerDetection();

    //data_ = ScanSetup;
    data_.ServiceType = 3; // forget this setting
    serviceTypeTexts[0] = tr("TV only");
    serviceTypeTexts[1] = tr("Radio only");
    serviceTypeTexts[2] = tr("HDTV only");
    serviceTypeTexts[3] = tr("Radio + TV");

    fecTexts[0] = " - ";
    fecTexts[1] = "1/2";
    fecTexts[2] = "2/3";
    fecTexts[3] = "3/4";
    fecTexts[4] = "4/5";
    fecTexts[5] = "5/6";
    fecTexts[6] = "6/7";
    fecTexts[7] = "7/8";
    fecTexts[8] = "8/9";

    fecStat = 0;

    fecTextsS2[0] = " - ";
    fecTextsS2[1] = "1/2";
    fecTextsS2[2] = "2/3";
    fecTextsS2[3] = "3/4";
    fecTextsS2[4] = "3/5";      //S2
    fecTextsS2[5] = "4/5";
    fecTextsS2[6] = "5/6";
    fecTextsS2[7] = "6/7";
    fecTextsS2[8] = "7/8";
    fecTextsS2[9] = "8/9";
    fecTextsS2[10] = "9/10";    //S2

    fecStatS2 = 3;
    
    rollTexts[0] = "ROLLOFF_35";
    rollTexts[1] = "ROLLOFF_25";
    rollTexts[2] = "ROLLOFF_20";
    rollTexts[3] = "AUTO";
    
    rollStat = 0;
    
    nitTexts[1] = "Yes";
    nitTexts[0]= "No";

    nitStat = 1;    

    // Cable
    modTexts[0] = "Auto / 64QAM";
    modTexts[1] = "256QAM";
    modTexts[2] = "4QAM / QPSK";
    modTexts[3] = "16QAM";
    modTexts[4] = "32QAM";
    modTexts[5] = "128QAM";

    // Sat S2
    modTextsS2[0] = "Auto";
    modTextsS2[1] = "QPSK";
    modTextsS2[2] = "PSK 8";
    modTextsS2[3] = "VSB 8";
    modTextsS2[4] = "VSB 16";

    modTextsS2[5] = tr("Detailed search");

    modStat = 2;

    searchTexts[0] = tr("Manual");
    searchTexts[1] = tr("SearchMode$Auto");

    addNewChannelsToTexts[0] = tr("End of channellist"); // default in Setup
    addNewChannelsToTexts[1] = tr("New channellist");
    addNewChannelsToTexts[2] = tr("Bouquets");

    addNewChannelsTo = 0;

    if(!source) {
        scanMode = 1;               // auto scan
    } else {
        scanMode = 0;               // manual scan
    }

    bandwidth = 0;              // 0: 7MHz 1: 8MHz

    sRate6875 = 0;

    //sRateItem[2] = "6111";

    sBwTexts[0] = "Auto";
    sBwTexts[1] = "7 MHz";
    sBwTexts[2] = "8 MHz";

    srScanTexts[0] = tr("Intelligent 6900/6875/6111");
    srScanTexts[1] = tr("Try all 6900/6875/6111");
    srScanTexts[2] = tr("Manual");

    srScanMode = 1;

    // CurrentChannel has to be greater than 0!
    currentChannel = (::Setup.CurrentChannel == 0) ? 1 : ::Setup.CurrentChannel;

    if(!source) {
        source = InitSource();
    }

    if (cPluginChannelscan::AutoScanStat != AssNone) {
        scanMode = 1;           //  auto scan
        switch (cPluginChannelscan::AutoScanStat)
        {
        case AssDvbS:
            sourceType = SAT;   // SATS2
            break;
        case AssDvbC:
            sourceType = CABLE;
            break;
        case AssDvbT:
            sourceType = TERR;
            break;
        default:
            cPluginChannelscan::AutoScanStat = AssNone;
            esyslog("Channelscan service handling error");      //??
            break;
        }
        cRemote::Put(kRed);
    }
    Set();
}

void cMenuChannelscan::TunerDetection()
{
    int stp = 0;
    for (int tuner = 0; tuner < CS_MAXTUNERS; tuner++) {
        cDevice *device = cDevice::GetDevice(tuner);
        srcTexts[tuner] = NULL;
        if (device && device->Priority() < 0) { // omit tuners that are recording
            std::stringstream txtstream;
            if (device->ProvidesSource(cSource::stTerr)) {
                txtstream << tr("DVB-T - Terrestrial") << " (" << tr("Tuner") << ' ' << tuner + 1 << ')';
                stp = TERR;
            } else if (device->ProvidesSource(cSource::stCable)) {
                txtstream << tr("DVB-C - Cable") << " (" << tr("Tuner") << ' ' << tuner + 1 << ')';
                stp = CABLE;
            } else if (device->ProvidesSource(cSource::stSat)) {
                if (device->NumProvidedSystems() == 2) { 
//                    if(TunerIsRotor(tuner))
//                       txtstream << tr("DVB-S2 - Rotor") << " (" << tr("Tuner") << ' ' << tuner + 1 << ')';
//                    else
                       txtstream << tr("DVB-S2 - Satellite") << " (" << tr("Tuner") << ' ' << tuner + 1 << ')';
                    stp = SATS2;
                } else {
//                    if(TunerIsRotor(tuner))
//                       txtstream << tr("DVB-S - Rotor") << " (" << tr("Tuner") << ' ' << tuner + 1 << ')';
//                    else
                       txtstream << tr("DVB-S - Satellite") << " (" << tr("Tuner") << ' ' << tuner + 1 << ')';
                    stp = SAT;
                }
            }
            if (txtstream.str().size()) {
                TunerNrArray[srcTuners] = tuner;
                //vdr valid tuner number
                srcTypes[srcTuners] = stp;
                // sourceType pro tuner
                srcTexts[srcTuners++] = strdup(txtstream.str().c_str());
                // coresonding tuner discription
            }
        }
    }
}

#define MAX_TUNERS_IN_MENU 16
typedef struct {
    int type[MAX_TUNERS_IN_MENU];
	char name[MAX_TUNERS_IN_MENU][128];
} tuner_info_t;

void cMenuChannelscan::TunerDetectionMcli()
{
    cPlugin *plug = cPluginManager::GetPlugin("mcli");
    if (plug) {
        int stp = 0;
        tuner_info_t info;
        for(int i=0; i<MAX_TUNERS_IN_MENU; i++)
            info.name[i][0] = '\0';
	    plug->Service("GetTunerInfo", &info);
        //int cntrTuners = 0;
        for(int i=0; i<MAX_TUNERS_IN_MENU; i++) {
            if(strlen(info.name[i]) == 0)
                break;
            else {
                switch(info.type[i]) {
                    case FE_QPSK: // DVB-S
                        stp = SAT; 
                        break;
                    case SYS_DVBS2: // DVB-S2
                        stp = SATS2;
                        break;
                    case FE_OFDM: // DVB-T
                        stp = TERR;
                        break;
                    case FE_QAM: // DVB-C
                        stp = CABLE;
                        break;
                }
                bool alreadyThere = false;
                for (int j = 0; j<srcTuners; j++) {
                    if(stp == srcTypes[j])
                        alreadyThere = true;
                }
                if(alreadyThere == false) {
                    srcTypes[srcTuners] = stp;
                    TunerNrArray[srcTuners] = i;
                    switch (info.type[i]) {
                        case SYS_DVBS2:
                            srcTexts[srcTuners] = strdup(tr("Satellite-S2"));
                            break;
                        case FE_QPSK:
                            srcTexts[srcTuners] = strdup(tr("Satellite-S"));
                            break;
                        case FE_OFDM:
                            srcTexts[srcTuners] = strdup(tr("Terrestrial"));
                            break;
                        case FE_QAM:
                            srcTexts[srcTuners] = strdup(tr("Cable"));
                            break;
                    }
                    srcTuners++;
                }
            }
        }
    }
}

void cMenuChannelscan::InitLnbs()
{
    lnbs = 0;
    for (cDiseqc * diseqc = Diseqcs.First(); diseqc; diseqc = Diseqcs.Next(diseqc)) {
        if (diseqc != Diseqcs.First() && diseqc->Source() == Diseqcs.Prev(diseqc)->Source())
            continue;

        DEBUG_CSMENU(DBGM " --Menu --- Diseqc Sources  %d --- \n", diseqc->Source());
        loopSources.push_back(diseqc->Source());
        lnbs++;
    }
}

int cMenuChannelscan::InitSource() {
    int source = 0;
    cChannel *channel = Channels.GetByNumber(currentChannel);

    if (channel)
        source = channel->Source();

    if (source == 0) {
        if (Setup.DiSEqC > 0) {
            cDiseqc *d = Diseqcs.First();
            if (d)
                source = d->Source();
            else
                source = DISEQC_SOURCE_ASTRA_19;
        } else
            source = DISEQC_SOURCE_ASTRA_19;         //astra 19.2E
    }

    return source;
}

void cMenuChannelscan::Set()
{
    int current = Current();

    srcItem = NULL;
    Clear();

    int blankLines = 3;
    sourceType = srcTypes[currentTuner];

    // avoid C/T-positions for SAT
    // and take Astra as start position
    if (srcTypes[currentTuner] >= SAT && (source == cSource::FromString("C") || source == cSource::FromString("T"))) {
        source = cSource::FromString("S19.2E");
    }

    Add(new cMenuEditStraItem(tr("Search Mode"), &scanMode, 2, searchTexts));
//    Add(new cMenuEditStraItem(tr("Add new channels to"), &addNewChannelsTo, 3, addNewChannelsToTexts));
    AddBlankLineItem(1);
    if(srcTuners>0) // if no tune available : prevent CRASH
        Add( new cMenuEditStraItem( tr("Source"), &currentTuner, srcTuners, srcTexts ) );
    else
        Add( new cOsdItem(tr("No Tuners available"),osUnknown,false) );

    switch (sourceType)
    {
    case SAT:
    case SATS2:
        srcItem = new cMyMenuEditSrcItem(tr("Position"), &source, currentTuner);     //XXX all sources in Diseqc?
        Add(srcItem);
        break;
    case TERR:
        Add(new cMenuEditBoolItem(tr("Detailed search"), &detailFlag, tr("no"), tr("yes")));
        if(scanMode != 0)
          Add(new cMenuEditStraItem(tr("Bandwidth"), &bandwidth, 3, sBwTexts));
        break;
    case CABLE:
        // if (extendeded)
        if(scanMode != 0) {
          Add(new cMenuEditStraItem(tr("Symbol Rates"), &srScanMode, 3, srScanTexts));
          Add(new cMenuEditBoolItem(tr("Bandwidth"), &bandwidth, "7 MHz", "8 MHz"));
        }
        break;
    }

    // filter
    //Add(new cMenuEditStraItem(tr("Service type"), &data_.ServiceType, 4, serviceTypeTexts));
    AddBlankLineItem(1);

    if (scanMode == 0) {
        switch (sourceType) {
        case SAT:
            Add(new cMenuEditIntItem(tr("Frequency (MHz)"), &frequency));
            Add(new cMenuEditChrItem(tr("Polarization"), &polarization, "HVLR"));
            Add(new cMenuEditIntItem(tr("Symbolrate"), &symbolrate));
            Add(new cMenuEditStraItem("FEC", &fecStat, 9, fecTexts));
            break;
        case SATS2:
            Add(new cMenuEditIntItem(tr("Frequency (MHz)"), &frequency));
            Add(new cMenuEditChrItem(tr("Polarization"), &polarization, "HVLR"));
            Add(new cMenuEditIntItem(tr("Symbolrate"), &symbolrate));
            Add(new cMenuEditStraItem(tr("Modulation"), &modStat, 6, modTextsS2));
            //if (modStat == 5)
            Add(new cMenuEditStraItem("FEC", &fecStatS2, 11, fecTextsS2));      //  new var  S2  Fec
            Add(new cMenuEditStraItem("ROLLOFF", &rollStat, 4 , rollTexts));      //  new var  S2  RollOff
            break;
        case CABLE:
            Add(new cMenuEditIntItem(tr("Frequency (kHz)"), &frequency));
            //Add(new cMenuEditBoolItem(tr("Bandwidth"), &bandwidth, "7 MHz", "8 MHz"));
            Add(new cMenuEditIntItem(tr("Symbolrate"), &symbolrate));
            Add(new cMenuEditStraItem(tr("Modulation"), &modStat, 6, modTexts));
            break;
        case TERR:
            Add(new cMenuEditIntItem(tr("Frequency (kHz)"), &frequency));
            //Add(new cMenuEditStraItem(tr("Bandwidth"), &bandwidth, 3, sBwTexts));
            AddBlankLineItem(2);
            break;
        default:
            esyslog("ERROR in - %s:%d check source type \n", __FILE__, __LINE__);
            break;
        }
    } else {
      if(srScanMode == 1) {
      Add(new cMenuEditStraItem("Nit Scan", &nitStat, 2 , nitTexts));
      }
        if(srScanMode == 2)
            Add(new cMenuEditIntItem(tr("Symbolrate"), &symbolrate));
        AddBlankLineItem(1);
    }

    if (::Setup.UseSmallFont || strcmp(::Setup.OSDSkin, "Reel") == 0) {
        blankLines -= 1;
    }

    AddBlankLineItem(blankLines);

    //Check this
    SetInfoBar();

#ifdef REELVDR
    if (cRecordControls::Active()) {
    Skins.Message(mtWarning,tr("Recording is running !"), 5);
    }
    SetHelp(tr("Button$Start"), NULL, NULL, tr("Channel sel."));

#else
    SetHelp(tr("Button$Start"), NULL, NULL, NULL);
#endif

    SetCurrent(Get(current));
    Display();
}

void cMenuChannelscan::SetInfoBar()  // Check with  cMenuScanActive
{

    DEBUG_CSMENU(" Menus --- %s -- scanState %d ---  \n", __PRETTY_FUNCTION__,
                 scanState);

    switch (scanState)
    {
#ifndef DEVICE_ATTRIBUTES
    case ssInit:
        if (sourceType >= SAT)
        {
            Add(new cMenuInfoItem(tr("DiSEqC"), static_cast < bool > (::Setup.DiSEqC)));
            if (::Setup.DiSEqC)
                DiseqShow();    // TODO  two columns display
        }
        break;
#endif
    case ssInterrupted:
        Add(new cMenuInfoItem(tr("Scanning aborted")));
        break;
    case ssSuccess:
        Add(new cMenuInfoItem(tr("Scanning succed")));
        break;
    case ssNoTransponder:
        Add(new cMenuInfoItem(tr("Retrieving transponders failed!")));
        Add(new cMenuInfoItem(tr("Please check satellites.conf.")));
        break;
    case ssNoLock:
        Add(new cMenuInfoItem(tr("Tuner obtained no lock!")));
        Add(new cMenuInfoItem(tr ("Please check your connections and satellites.conf.")));
        break;
    case ssFilterFailure:
        Add(new cMenuInfoItem(tr("Unpredictable error occured!")));
        Add(new cMenuInfoItem(tr("Please try again later")));
        break;
    default:
        break;
    }
    Add(new cMenuInfoItem(tr("Information:")));
    Add(new cMenuInfoItem(tr("Entries in current channellist"), Channels.MaxNumber()));
}

void cMenuChannelscan::Store()
{
    // TODO: is this really needed? what kind of settings are modified that need to be saved?
    Setup = data;
    Setup.Save();
    ScanSetup = data_; // ScanSetup is not stored, used by filter only
}

eOSState cMenuChannelscan::ProcessKey(eKeys Key)
{


  if (cMenuChannelscan::scanState == ssInterrupted )
  {
   // get back to the channel that was being played, if Interrupted. If not available go to #1
  if (!Channels.SwitchTo(::Setup.CurrentChannel))
      Channels.SwitchTo(1);
  printf("%s switching back to %d", __PRETTY_FUNCTION__, ::Setup.CurrentChannel);
  cMenuChannelscan::scanState = ssInit;
  }

    bool HadSubMenu = HasSubMenu();

    int oldDetailedScan = detailedScan;
    int oldScanMode = scanMode;
    oldCurrentTuner = currentTuner;

    int oldSourceStat = currentTuner;
    int oldSRScanMode = srScanMode;
    bool oldExpertSettings = expertSettings;

//    bool removeFlag = false; // for removing channels after "New Channel (auto added)" bouquet
    std::vector<cChannel*> ChannelsToRemove;

    eOSState state = cOsdMenu::ProcessKey(Key);

    /* TB: make the srcItem select a proper satellite */
    if(srcItem && oldCurrentTuner != currentTuner) {
       srcItem->SetCurrentTuner(currentTuner);
       srcItem->ProcessKey(kLeft);
       srcItem->ProcessKey(kRight);
       srcItem->Set();
    }

#ifdef REELVDR
    if (state == osUser1) {
        return AddSubMenu(new cMenuSelectChannelList(&data));
    }
#endif // REELVDR

    if (HadSubMenu && !HasSubMenu()) {
        Set();
    }

    if (state == osUnknown && (!HadSubMenu || !HasSubMenu()))
    {

        sourceType = srcTypes[currentTuner];

        scp.card = TunerNrArray[currentTuner];        // tuner nr; VDR valid tuner number is in TunerNrArray.
        scp.type = sourceType;  // source type
        scp.source = source;
        scp.frequency = frequency;
	scp.bandwidth = bandwidth;
        scp.polarization = polarization;
        scp.symbolrate = symbolrate;
        scp.fec = (sourceType == SATS2) ? fecStatS2 : fecStat;
        scp.detail = scanMode | (detailFlag << 1);
        ///<  detailFlag=1 -> DVB-T offset search
        ///<  espacialy for british broadcasters
        scp.modulation = modStat;
        ///< on S2 value 5 means all mods
        scp.symbolrate_mode = srScanMode;
	scp.rolloff = rollStat;
        scp.nitscan = nitStat;
#if 0
        DEBUG_CSMENU(" DEBUG LOAD TRANSPONDER(S)  ---- \n"
                     " Scanning on Tuner \t%d\n"
                     " SourceType  \t%d \n"
                     " Source \t%d \n"
                     " frequency \t%d \n"
                     " Bandwidth \t%d \n"
                     " Polarisation \t%c \n"
                     " Symbolrate \t%d \n"
                     " fec Stat  \t%d \n"
                     " detail search \t%d \n"
		     " modulationStat  \t%d \n"
                     " rolloffStat  \t%d \n"
                     " Symbolrate mode \t%d \n",
                     scp.card,
                     scp.type,
                     scp.source,
                     scp.frequency,
                     scp.bandwidth,
                     scp.polarization,
                     scp.symbolrate,
                     scp.fec,
                     scp.detail,
		     scp.modulation,
		     scp.rolloff,
		     scp.symbolrate_mode);
#endif

        switch (Key)
        {
        case kOk:
            /*Store();
               SwitchChannel();
               return osContinue; */
        case kRed:
        // Check for recording/free tuners already done before starting the plugin.

        /*if (cRecordControls::Active()) {
        Skins.Message(mtError, tr("Recording is running"));
        break; // donot start ChannelScan
        } */

            //addNewChannels
            //::Setup.AddNewChannels = (addNewChannelsTo!=0);
/*            switch( addNewChannelsTo )
            {
                case 0:
                    Setup.AddNewChannels = 0;
                    break;
                case 1:
                    //clear channellist
                    {
#ifdef REELVDR
                        //disable livebuffer
                        int  LiveBufferTmp =::Setup.LiveBuffer;
                        ::Setup.LiveBuffer = 0;
#endif
                        //stop replay and set channel edit flag
                        cDevice::PrimaryDevice()->StopReplay() ;
                        Channels.IncBeingEdited();

                        vector<cChannel*> tmp_channellist;
                        int i;

                        //get pointer to all cChannel objects in Channels-list
                        for (cChannel *ch = Channels.First(); ch ; ch = Channels.Next(ch))
                            tmp_channellist.push_back(ch);

                        //Delete all channels from Channels list
                        for (i=0; i< (int)tmp_channellist.size(); i++)
                            Channels.Del( tmp_channellist[i] );

                        // Renumber
                        Channels.ReNumber();
                        //finished editing channel list
                        Channels.DecBeingEdited();
#ifdef REELVDR
                        //Set back livebuffer since livebuffer flag is once again set&reset in ActiveScan class
                        ::Setup.LiveBuffer= LiveBufferTmp;
#endif
                    }
                    // fallthrough: no break
                case 2:
                    //add to own Bouquets
                    Setup.AddNewChannels = 1;
                    Setup.Save();
                    break; 
                default:
                // should not be here
                break;  
            }
*/

            dsyslog("\n\nAddNewChannelsTo: %d\n\n\n", addNewChannelsTo);

            ScanSetup = data_; // when kRed is pressed, use the selected filters
            source = sourceType == CABLE ? 0x4000 : sourceType ==  TERR ? 0xC000 : source;

            ///< TERR:  0; CABLE: 1;  SAT:   2; SATS2: 3

            ///< scanMode == 0: manual scan
            ///< scanMode == 1: autoscan; take transponder list
            ///< scanMode == 2 is debricated NITScan  is always performed at SAT && Auto scan

            if (scanMode==0) // Manual
            {
                cMenuChannelscan::scanState = ssGetChannels;

                if (cTransponders::GetInstance().GetNITStartTransponder()) // free nitStartTransponder_
                     cTransponders::GetInstance().ResetNITStartTransponder(0);
            }

            scp.frequency = scanMode ? scanMode : frequency;
            cTransponders::GetInstance().Load(&scp);

            esyslog("%s freq=%d scanMode=%d",__PRETTY_FUNCTION__, scp.frequency,scanMode);
            esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);

            /********** removed the bouquet New Channel (auto added)******************/
            /* not used any more 2009-10-13 by RC (on whish by IB)
            removeFlag = false;
            for (cChannel *c=Channels.First(); c; c = Channels.Next(c))
            {
                if (removeFlag)
                {
                     if (c->GroupSep()) // a new bouquet after auto added! Donot delete this
                          break;
                     ChannelsToRemove.push_back(c);
                }
                else if (strcasestr(c->Name(),"auto added"))
                {
                      removeFlag = true; // remove every channel after this tag but // Leave the tag UNCHANGED
                }

            }

            for (std::vector<cChannel*>::iterator it=ChannelsToRemove.begin();
                  it != ChannelsToRemove.end(); it++)
            {
                printf("removing %s\n", (*it)? (*it)->Name(): " ");
                Channels.Del(*it);
            }
            */

            return AddSubMenu(new cMenuScanActive(&scp));
        case kGreen:
            if (sourceType == CABLE)
            {
                expertSettings = expertSettings ? false : true;
                Set();
            }
            break;
#ifdef REELVDR
        case kBlue:
        if (cRecordControls::Active()) {
        Skins.Message(mtError, tr("Recording is running"));
        break; // donot selectChannelist
           }
            return AddSubMenu(new cMenuSelectChannelList(&data));

            break;
#endif
        default:
            state = osContinue;
        }
    }

    // forces setup if menu layout should be changed
    if (Key != kNone && !HadSubMenu)
    {
        if (oldDetailedScan != detailedScan || oldScanMode != scanMode ||
            oldExpertSettings != expertSettings || oldSourceStat != currentTuner || oldSRScanMode != srScanMode)
        {
            if (sourceType == CABLE && symbolrate > 8000)       // avoid DVB-S symbolrates in DVB-C
            {
                symbolrate = 6900;
            }
            oldSourceStat = currentTuner;
            Set();
        }
    }
    return state;
}

void cMenuChannelscan::DiseqShow() {

    for (iConstIter iter = loopSources.begin(); iter != loopSources.end(); ++iter) {
        char buffer[256];

        snprintf(buffer, sizeof(buffer), "LNB %c: %s", 'A' + (int)(iter - loopSources.begin()), *cSource::ToString(*iter));
        //DLOG ("Show fetch Source [%d] %d %s ", (int) iter-loopSources.begin() , *iter, buffer);
        Add(new cMenuInfoItem(buffer));
    }
}

cMenuChannelscan::~cMenuChannelscan()
{
    for (int i = 0; i < srcTuners; i++) {
        if (srcTexts[i]) {
            free(srcTexts[i]);
        }
    }

    if (HasSubMenu()) {
        CloseSubMenu();
    }

    tvChannelNames.clear();
    radioChannelNames.clear();
    dataChannelNames.clear();
    cPluginChannelscan::AutoScanStat = AssNone;
    cTransponders::Destroy();

    // Really needed?
    Store();

}

// taken fron vdr/menu.c
void cMenuChannelscan::AddBlankLineItem(int lines)
{
    for (int i = 0; i < lines; i++) {
        cOsdItem *item = new cOsdItem;
        item->SetSelectable(false);
        item->SetText(strndup(" ", 1), false);
        Add(item);
    }
}

void cMenuChannelscan::SwitchChannel()
{

    Channels.SwitchTo(currentChannel);
    /*
       cChannel *c = GetByNumber(currentChannel);
       if (c)
       {
       device->SwitchChannel(c,true);
       }
     */

}

// --- cMenuScanActive -------------------------------------------------------
#ifndef DBG
#define DBG "DEBUG [cMenuScanActive]: "
#endif

#define COLUMNWIDTH 24

cMenuScanActive::cMenuScanActive(cScanParameters * sp)
:cOsdMenu(tr("Scan active"), COLUMNWIDTH)
{

    Channels.IncBeingEdited();

    scp = sp;

    DEBUG_CSMENU(" Menus --- %s -- freq  %d ---  \n", __PRETTY_FUNCTION__, scp->frequency);

    oldUpdateChannels =::Setup.UpdateChannels;
    ::Setup.UpdateChannels = 0; // prevent  VDRs own update Channel

#ifdef REELVDR
    LiveBufferTmp =::Setup.LiveBuffer;
    ::Setup.LiveBuffer = 0;
#endif

    oldChannelNumbers = Channels.MaxNumber();

    // Make class
    tvChannelNames.clear();
    radioChannelNames.clear();
    esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);

    if (cTransponders::GetInstance().GetNITStartTransponder()) {
        DEBUG_CSMENU(" Menus ---  Set NIT Auto search \n");
        cMenuChannelscan::scanState = ssGetTransponders;
    }
    esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);

    // auto_ptr
    Scan.reset(new cScan());
    esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);

    isyslog(" start Scanning @ Card %d --- ", scp->card);

    if (!Scan->StartScanning(scp)) {
        DEBUG_CSMENU(" Scan() returns failure   \n");
        esyslog(ERR "  Tuner Error");
        cMenuChannelscan::scanState = ssInterrupted;
    }
    esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);

    Setup();
    Channels.Save();
    esyslog("%s ssGetChannels?=%d",__PRETTY_FUNCTION__,cMenuChannelscan::scanState == ssGetChannels);

}

void cMenuScanActive::Setup()
{
    int num_tv = 0, num_radio = 0;

    Clear();
    mutexNames.Lock();

    vector < string >::iterator tv;
    vector < string >::iterator radio;

    tv = tvChannelNames.begin();
    radio = radioChannelNames.begin();

    num_tv = tvChannelNames.size();
    num_radio = radioChannelNames.size();

    //maxLINES defines the number of TV/Radio channelnames displayed on the OSD
    //DisplayMenu()->MaxItems() get the number from the Skin
    int maxLINES = DisplayMenu()->MaxItems() - 7;
    if ((int)tvChannelNames.size() > maxLINES)
        tv += tvChannelNames.size() - maxLINES;

    if ((int)radioChannelNames.size() > maxLINES)
        radio += radioChannelNames.size() - maxLINES;
    Add(new cMenuInfoItem(tr("TV Channels\tRadio Channels")), COLUMNWIDTH);
    Add(new cMenuInfoItem(""));
    /// Display channel names
    for (;;) {
        if (tv == tvChannelNames.end() && radio == radioChannelNames.end())
            break;

        cMenuScanActiveItem *Item = new cMenuScanActiveItem(tv == tvChannelNames.end()? "" : tv->c_str(), radio == radioChannelNames.end()? "" : radio-> c_str());
        Add(Item);
        if (tv != tvChannelNames.end()) {
            ++tv;
        }
        if (radio != radioChannelNames.end()) {
            ++radio;
        }
    }

    int nameLines = tvChannelNames.size() > radioChannelNames.size()? tvChannelNames.size() : radioChannelNames.  size();

    if (nameLines > maxLINES)
        nameLines = maxLINES;

    mutexNames.Unlock();

    AddBlankLineItem(maxLINES + 1 - nameLines);

    char buffer[50];

    if (cMenuChannelscan::scanState == ssGetTransponders) {
        AddBlankLineItem(1);
    } else {
        snprintf(buffer, 50, "TV: %i \tRadio: %i ", num_tv, num_radio);
        FILE *fp = fopen("/tmp/cScan.log","a");
        const time_t tt = time(NULL);
        char *strDate;
        asprintf(&strDate,"%s", asctime(localtime(&tt)));
        strDate[strlen(strDate)-1] = 0;
        fprintf(fp,"\n%s TV:%d \t Radio: %d",strDate ,num_tv, num_radio);
        for (int i = Ntv;i<num_tv;i++)
            fprintf(fp,"\n\t%s",tvChannelNames[i].c_str());
        for (int i = Nradio;i<num_radio;i++)
            fprintf(fp,"\n\t\t%s",radioChannelNames[i].c_str());
        fclose(fp);
        free(strDate);

        Ntv = num_tv; Nradio = num_radio;
        Add(new cMenuInfoItem(buffer));
        transponderNum_ = cTransponders::GetInstance().v_tp_.size();
    }

    if (cMenuChannelscan::scanState <= ssGetChannels) {

        // TV SD, TV HD, Radio only, TV (HD+SD),  TV TV (HD+SD);
        const char *serviceTxt[4] =
            { trNOOP("TV only"), trNOOP("Radio only"), trNOOP("HDTV only"), trNOOP("Radio + TV") };

        if (cMenuChannelscan::scanState == ssGetTransponders) {
            snprintf(buffer, sizeof(buffer), tr("Retrieving transponder list from %s"), tr(cTransponders::GetInstance().Position().c_str()));
            Add(new cMenuStatusBar(100, Scan->GetCurrentTransponderNr() * 2, 0, true));
        } else if (cMenuChannelscan::scanState == ssGetChannels) {
            if (scp->type == SAT || scp->type == SATS2) {
                snprintf(buffer, sizeof(buffer), tr("Scanning %s (%iMHz)\t%s"), tr(cTransponders::GetInstance().Position().c_str()),
                         Scan->GetCurrentFrequency(), tr(serviceTxt[ScanSetup.ServiceType]));
                Add(new cMenuStatusBar(transponderNum_, Scan->GetCurrentTransponderNr(), Scan->GetCurrentChannelNumber()));
            } else {
                printf("%s", tr(cTransponders::GetInstance().Position().c_str()));
                snprintf(buffer, sizeof(buffer), tr("Scanning %s (%.3fMHz)\t%s"), tr(cTransponders::GetInstance().Position().c_str()),
                         Scan->GetCurrentFrequency() / (1000.0 * 1000), tr(serviceTxt[ScanSetup.ServiceType]));
                Add(new cMenuStatusBar(transponderNum_, Scan->GetCurrentTransponderNr(), Scan->GetCurrentChannelNumber()));
            }
        }
        Add(new cMenuInfoItem(buffer));
    } else if (cMenuChannelscan::scanState >= ssInterrupted) {
        if (cMenuChannelscan::scanState == ssSuccess) {
            if ((Channels.MaxNumber() > 1) && Channels.MaxNumber() == oldChannelNumbers)
                Add(new cMenuInfoItem(tr("No new channels found")));
        } else if (Channels.MaxNumber() > oldChannelNumbers) {
            Add(new cMenuInfoItem(tr("Added new channels"), (Channels.MaxNumber() - oldChannelNumbers)));
        }
        Add(new cMenuInfoItem(""));
         if (cPluginChannelscan::AutoScanStat == AssNone)
            Add(new cMenuInfoItem(tr("Press OK to finish or Exit for new scan")));
        ErrorMessage();
    }
    Display();
}

// show this only cMenuChannelscan
void
cMenuScanActive::ErrorMessage()
{
    if (cMenuChannelscan::scanState >= ssInterrupted)
    {

        /*
           if (cPluginChannelscan::AutoScanStat != AssNone)
           {
           cPluginChannelscan::AutoScanStat = AssNone;
           cRemote::CallPlugin("install");
           }
         */
        switch (cMenuChannelscan::scanState)
        {
        case ssInterrupted:
            Skins.Message(mtInfo, tr("Scanning aborted"));
            break;
            // Scanning aborted by user
            /*
               case ssDeviceFailure: Skins.Message(mtError, tr("Tuner error! ")); break;
               // missing device file etc.
               case ssNoTransponder: Skins.Message(mtError, tr("Missing parameter")); break;
               // missing transponderlist due to wrong parameter
               case ssNoLock: Skins.Message(mtError, tr("Tuner error!")); break;
               // reciver error
               case ssFilterFailure: Skins.Message(mtError, tr("DVB services error!")); break;
               // DVB SI or filter error
             */
        default:
            break;
        }
    }
}

void cMenuScanActive::DeleteDummy()
{
    DEBUG_CSMENU(" --- %s --- %d -- \n", __PRETTY_FUNCTION__, Channels.Count());

    if (Channels.Count() < 3)
        return;

    cChannel *channel = Channels.GetByNumber(1);
    if (channel && strcmp(channel->Name(), "ReelBox") == 0) {
        Channels.Del(channel);
    }
    Channels.ReNumber();
}

eOSState cMenuScanActive::ProcessKey(eKeys Key)
{
    //if (cPluginChannelscan::AutoScanStat != AssNone && !cMenuChannelscan::scanning)
    if (cPluginChannelscan::AutoScanStat != AssNone && cMenuChannelscan::scanState >= ssInterrupted) {
        cPluginChannelscan::AutoScanStat = AssNone;
        return osEnd;
    }

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key)
        {
        case kOk:
            if (cMenuChannelscan::scanState >= ssInterrupted) {
                cMenuChannelscan::scanState = ssInit;
                printf(" cMenuChannelscan Call Channels.Save() \n");
                Channels.Save();
                    cRemote::Put(kChannels);
                return osEnd;
            } else {
                state = osContinue;
            }
            break;
        case kStop:
        case kBack:
            // shut down scanning
            cMenuChannelscan::scanState = ssInterrupted;
            // free nitStartTransponder_ firefox
            cTransponders::GetInstance().ResetNITStartTransponder(0);
            //cMenuChannelscan::scanning = false;
            // Channel changed back to ::Setup.CurrentChannel in cMenuChannelscan::ProcessKey()
            return osBack;
        default:
            state = osContinue;
        }
        Setup();
    }

    return state;
}

void cMenuScanActive::AddBlankLineItem(int lines)
{
    for (int i = 0; i < lines; i++)
    {
        cOsdItem *item = new cOsdItem;
        item->SetSelectable(false);
        item->SetText(strndup(" ", 1), false);
        Add(item);
    }
}

bool scanning_on_receiving_device;

cMenuScanActive::~cMenuScanActive()
{
    Scan->ShutDown();

    tvChannelNames.clear();
    radioChannelNames.clear();
    dataChannelNames.clear();
    // XXX
    //cMenuChannelscan::scanning = false;
    cMenuChannelscan::scanState = ssInterrupted;
    scanning_on_receiving_device = false;

    // restore original settings
    ::Setup.UpdateChannels = oldUpdateChannels;

    // call cMenuChannels if kOk
    DeleteDummy();

    Channels.DecBeingEdited();

    // try going back to the "current" Channel if not then  channel#1
    cChannel *channel = Channels.GetByNumber(::Setup.CurrentChannel);
    if (channel)                // && !scanning_on_receiving_device)
    {
        cDevice::PrimaryDevice()->SwitchChannel(channel, true);
        //printf("\nSleeping %s %d",__PRETTY_FUNCTION__,__LINE__);
        usleep(200 * 1000);
    } else
    {
     // switch to Channel #1
     Channels.SwitchTo(1);
    }

#ifdef REELVDR
    // restore original settings
    ::Setup.LiveBuffer = LiveBufferTmp;
#endif

}

// --- cMenuScanActiveItem ----------------------------------------------------
cMenuScanActiveItem::cMenuScanActiveItem(const char *TvChannel, const char *RadioChannel)
{
    tvChannel = strdup(TvChannel);
    radioChannel = strdup(RadioChannel);
    char *buffer = NULL;
    asprintf(&buffer, "%s \t%s", tvChannel, radioChannel);
    SetText(buffer, false);
    SetSelectable(false);
}

cMenuScanActiveItem::~cMenuScanActiveItem()
{
    free(tvChannel);
    free(radioChannel);
}

// cMenuEditSrcItem taken fron vdr/menu.c
// --- cMenuEditSrcItem ------------------------------------------------------

cMyMenuEditSrcItem::cMyMenuEditSrcItem(const char *Name, int *Value, int CurrentTuner):
cMenuEditIntItem(Name, Value, 0)
{
    source = Sources.Get(*Value);
    currentTuner = CurrentTuner;
    Set();
}

void cMyMenuEditSrcItem::Set(void)
{
    if (source)
    {
        char *buffer = NULL;
        asprintf(&buffer, "%s - %s", *cSource::ToString(source->Code()),
                 source->Description());
        SetValue(buffer);
        free(buffer);
    }
    else
    {
        cMenuEditIntItem::Set();
    }
}

eOSState cMyMenuEditSrcItem::ProcessKey(eKeys Key)
{
    eOSState state = cMenuEditItem::ProcessKey(Key);

    int oldCode = source->Code();
    const cSource *oldSrc = source;
    bool found = false;

    if (state == osUnknown)
    {
        if (NORMALKEY(Key) == kLeft)    // TODO might want to increase the delta if repeated quickly?
        {
           if(cSource::IsSat(source->Code()) && !cPluginManager::GetPlugin("mcli")) {
                source = oldSrc;
                while(!found && source && (source->Code() & cSource::stSat)) {
                    for (cDiseqc *p = Diseqcs.First(); p && !found; p = Diseqcs.Next(p)) {
                       /* only look at sources configured for this tuner */
                       if(source && (source->Code() != oldSrc->Code())&& (source->Code() == p->Source())) {
                            *value = source->Code();
                            found = true;
                            break;
                       }
                    }
                    if(!found)
                       source = (cSource *) source->Prev();
                }
           } else {
             if (source && source->Prev()) {
                found = true;
                source = (cSource *) source->Prev();
                *value = source->Code();
             }
           }
        } else if (NORMALKEY(Key) == kRight) {
            if (cSource::IsSat(source->Code()) && !cPluginManager::GetPlugin("mcli")) {
               source = oldSrc;
               while(!found && source && (source->Code() & cSource::stSat)) {
                   for (cDiseqc *p = Diseqcs.First(); p && !found; p = Diseqcs.Next(p)) {
                        /* only look at sources configured for this tuner */
                        if(source && (source->Code() != oldSrc->Code()) && (source->Code() == p->Source())) {
                            *value = source->Code();
                            found = true;
                            break;
                        }
                   }
                   if(!found)
                      source = (cSource *) source->Next();
               }
            } else {
              if (source && source->Next()) {
                found = true;
                source = (cSource *) source->Next();
                *value = source->Code();
              }
            }
        } else
            return state;       // we don't call cMenuEditIntItem::ProcessKey(Key) here since we don't accept numerical input

        if(!found)
           source = Sources.Get(oldCode);

        Set();
        state = osContinue;
    }
    return state;
}

//taken from rotor rotor plugin

cMenuScanInfoItem::cMenuScanInfoItem(const string & Pos, int f, char pol, int sr, int fecIndex)
{

    const char *pos = Pos.c_str();
    char *buffer = NULL;
    asprintf(&buffer, "%s :\t%s %d %c %d %s", tr("Scanning on transponder"), pos, f, pol, sr, FECToStr(fecIndex));

    SetText(strdup(buffer), false);
    SetSelectable(false);
}

const char * cMenuScanInfoItem::FECToStr(int Index)
{
    switch (Index)
    {
    case 1:
        return "1/2";
    case 2:
        return "2/3";
    case 3:
        return "3/4";
    case 4:
        return "4/5";
    case 5:
        return "5/6";
    case 6:
        return "6/7";
    case 7:
        return "7/8";
    default:
        return tr("Auto");
    }
    return tr("Auto");
}

// --- cMenuStatusBar ----------------------------------------------------

cMenuStatusBar::cMenuStatusBar(int Total, int Current, int Channel, bool BarOnly)
{
    int barWidth = 50;
    int percent;
    Current += 1;

    if (Current > Total) {
        Current = Total;
    }
    if (Total < 1) {
        Total = 1;
    }

    // GetReal EditableWidth
    percent = static_cast < int >(((Current) * barWidth / (Total)));

    char buffer[barWidth + 1];
    int i;

    buffer[0] = '[';
    for (i = 1; i < barWidth; i++)
        i < percent ? buffer[i] = '|' : buffer[i] = ' ';

    buffer[i++] = ']';
    buffer[i] = '\0';

    char *tmp;
    int l = 0;
    if (BarOnly)
        l = asprintf(&tmp, "%s", buffer);
    /*
       else if (SAT)
       l = asprintf(&tmp,"%s\t  %d / %d", buffer, Current, Total);
     */
    else
        l = asprintf(&tmp, "%s\t  %d / %d  (CH: %d)", buffer, Current, Total, Channel);

    SetText(strndup(tmp, l), false);
    SetSelectable(false);
    free(tmp);
}

// --- Class cMenuInfoItem -------------------------------------

cMenuInfoItem::cMenuInfoItem(const char *text, const char *textValue)
{
    char *buffer = NULL;
    asprintf(&buffer, "%s  %s", text, textValue ? textValue : "");

    SetText(strdup(buffer), false);
    SetSelectable(false);
    free(buffer);
}

cMenuInfoItem::cMenuInfoItem(const char *text, int intValue)
{
    char *buffer = NULL;
    asprintf(&buffer, "%s: %d", text, intValue);

    SetText(strdup(buffer), false);
    SetSelectable(false);
    free(buffer);
}

cMenuInfoItem::cMenuInfoItem(const char *text, bool boolValue)
{
    char *buffer = NULL;
    asprintf(&buffer, "%s: %s", text, boolValue ? tr("enabled") : tr("disabled"));

    SetText(strdup(buffer), false);
    SetSelectable(false);
    free(buffer);
}

#ifdef REELVDR
// --- Class cMenuChannelsItem  ------------------------------------------------

cMenuChannelsItem::cMenuChannelsItem(cDirectoryEntry * Entry)
{
    entry_ = Entry;
    char *buffer = NULL;
    isDir_ = false;
    if (Entry->IsDirectory())
    {
        isDir_ = true;
        asprintf(&buffer, "\x82 %s", Entry->Title());
    }
    else
    {
        asprintf(&buffer, "     %s", Entry->Title());
    }

    SetText(buffer, true);
    free(buffer);
}

cMenuChannelsItem::~cMenuChannelsItem()
{

}

// --- Class cMenuSelectChannelList ------------------------------------------------

cMenuSelectChannelList::cMenuSelectChannelList(cSetup *SetupData):cOsdMenu(tr("Channel lists"), 10)
{
    WizardMode_ = false;
    SetupData_ = SetupData;
    helpKeys_ = -1;
    level_ = current_ = 0;
    DirectoryFiles.Load();
    Set();
    hasTimers_ = Timers.Count();
    LiveBufferTmp =::Setup.LiveBuffer;
}

cMenuSelectChannelList::~cMenuSelectChannelList()
{
    // restore original settings
    ::Setup.LiveBuffer = LiveBufferTmp;
}

cMenuSelectChannelList::cMenuSelectChannelList(char *newTitle, char *Path, std::string AdditionalText, bool WizardMode): cOsdMenu(tr("Channel lists"), 10)
{
    WizardMode_ = WizardMode;
    if(newTitle && strlen(newTitle))
        SetTitle(newTitle);
    helpKeys_ = -1;
    SetupData_ = &Setup;
    level_ = current_ = 0;
    additionalText_ = AdditionalText;
    if(Path && strlen(Path)) {
        string StartPath = Path;
        DirectoryFiles.Load(StartPath);
    } else
        DirectoryFiles.Load();
    Set();
    hasTimers_ = Timers.Count();
    if(WizardMode_)
        SetHelp(NULL, tr("Back"), tr("Skip"), NULL);
    LiveBufferTmp =::Setup.LiveBuffer;
}

void cMenuSelectChannelList::Set()
{
    //if (Current() >= 0)
    //    current_ = Current();

    current_ = 2;
    Clear();

    if(additionalText_.size()) {
        AddFloatingText(additionalText_.c_str(), 50);
        Add(new cOsdItem("", osUnknown, false));
    }

    Add(new cOsdItem(tr("Please select a channellist:"), osUnknown, false));
    Add(new cOsdItem("", osUnknown, false));

    for (cDirectoryEntry * d = DirectoryFiles.First(); d; d = DirectoryFiles.Next(d)) {
        cMenuChannelsItem *item = new cMenuChannelsItem(d);
        Add(item);
    }

    SetCurrent(Get(current_));

//    SetHelp(NULL, NULL, NULL, NULL);
    SetHelp(NULL, NULL, NULL, tr("Functions"));
    Display();
}


bool cMenuSelectChannelList::ChangeChannelList()
{
    cMenuChannelsItem *ci = (cMenuChannelsItem *) Get(Current());

    ::Setup.LiveBuffer = 0;

    if (ci && !ci->IsDirectory() && ci->FileName()) {
        // new thread?
        std::string cmd = "/usr/bin/sudo -s /etc/bin/etc_backup.sh -c"; // backup channels.conf after switch
        SystemExec(cmd.c_str());

        string buff = ci->FileName();
        cerr << " cp \"" << ci->FileName() << "\" to \"" << setup::FileNameFactory("link") << "\"" << endl;
        CopyUncompressed(buff);
        bool ret = Channels.Reload(setup::FileNameFactory("link").c_str());
#if !defined(RBMINI) && !defined(RBLITE) && defined(REELVDR)
        CopyToTftpRoot("/etc/vdr/channels.conf");
#endif
        return ret;
    }
    return false;
}

bool cMenuSelectChannelList::CopyUncompressed(std::string & buff)
{

    //std::string comprFileName(buff + ".bz2");
    //if (buff.find(".bz2") == string::npos)
    //compressed = false;

    std::string outName = (setup::FileNameFactory("link").c_str());
    std::ifstream infile(buff.c_str(), std::ios_base::in | std::ios_base::binary);
    std::ofstream outfile(outName.c_str());

    if (outfile.fail() || outfile.bad()) {
        cerr << " can`t open output file  " << outName << endl;
        return false;
    }

    if (infile.fail() || infile.bad()) {
        cerr << " can`t open input file  " << buff << endl;
        return false;
    }

    if (buff.find(".bz2") != string::npos) {
        cerr << " using bzip2 decompressor" << endl;
        int bzerror = 0;
        int buf = 0;
        FILE *file = fopen(buff.c_str(), "r");
        if(file && !ferror(file)) {
            BZFILE *bzfile = BZ2_bzReadOpen(&bzerror, file, 0, 0, NULL, 0);
            if ( bzerror != BZ_OK ) {
                BZ2_bzReadClose ( &bzerror, bzfile); /* TODO: handle error */
                fclose(file);
                return false;
            }
            bzerror = BZ_OK;
            while ( bzerror == BZ_OK /* && arbitrary other conditions */) {
                int nBuf = BZ2_bzRead ( &bzerror, bzfile, &buf, 1/* size of buf */ );
                if (nBuf && bzerror == BZ_OK ) {
                    outfile.put(buf);
                }
            }
            if ( bzerror != BZ_STREAM_END ) {
               BZ2_bzReadClose ( &bzerror, bzfile ); /* TODO: handle error */
               fclose(file);
               return false;
            } else {
               BZ2_bzReadClose ( &bzerror, bzfile );
               fclose(file);
               return true;
            }
        }
    } else if (buff.find(".gz") != string::npos) {
        cerr << " using gzip decompressor " << endl;
        gzFile gzfile = gzopen(buff.c_str(), "r");
        int buf;
        if(gzfile) {
            while(!gzeof(gzfile)){
                buf = gzgetc(gzfile);
                if(buf != 255)
                    outfile.put(buf);
            }
            gzclose(gzfile);
        }
    } else {                      // uncompressed
        int c;
        /* copy byte-wise */
        while(infile.good()) {
            c = infile.get();
            if(infile.good())
                outfile.put(c);
        }
    }

    outfile.close();

    return true;
}


eOSState cMenuSelectChannelList::ProcessKey(eKeys Key)
{
    bool HadSubMenu = HasSubMenu();

    eOSState state = cOsdMenu::ProcessKey(Key);
    if(state == osUser1) {
        CloseSubMenu();
        AddSubMenu(new cChannellistBackupMenu(eImport));
        return osContinue;
    } else if(state == osUser2) {
        CloseSubMenu();
        AddSubMenu(new cChannellistBackupMenu(eExport));
        return osContinue;
    } else if(state == osUser3) {
        CloseSubMenu();
        std::string cmd = std::string("getConfigsFromAVGServer.sh ") + Setup.NetServerIP + std::string(" channels.conf ; ");
        Skins.Message(mtInfo, tr("Loading channel list ..."));
        SystemExec(cmd.c_str());
        Channels.Reload("/etc/vdr/channels.conf");
        Skins.Message(mtInfo, tr("Changes Done"));
        return osContinue;
    }
    if(HadSubMenu)
            return osContinue;

    //printf("\n%s\n", __PRETTY_FUNCTION__);
    if (Key == kBack) {
        Key = kNone;

        if (level_ > 0) {
            Open(true);         // back
            Set();
            return osUnknown;
        } else {
            if (OnlyChannelList) {
                OnlyChannelList = false;
                return osEnd;
            }
            return osBack;
        }
    } else {
        current_ = 0;
    }

//    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown)
    {
        switch (Key)
        {
        case kOk:
            if (DirState() == 1) {      // if no directory
                Skins.Message(mtInfo, tr("Loading channel list ..."));
                if (ChangeChannelList()) {
                    if(SetupData_) {
                        cChannel *chan = Channels.GetByNumber(1);
                        while (chan && chan->GroupSep()) {
                            chan = (cChannel*)chan->Next();
                        }
                        if (chan && !chan->GroupSep()) {
                           SetupData_->TimeTransponder = chan->Transponder();
                           SetupData_->TimeSource = chan->Source();
                        }
                        // Don't Save() since it's done in destructor (<-whose silly idea is that?) of parent menu (cMenuChannelScan)
                    } else {
                        cChannel *chan = Channels.GetByNumber(1);
                        while (chan && chan->GroupSep()) {
                            chan = (cChannel*)chan->Next();
                        }
                        if (chan && !chan->GroupSep()) {
                           ::Setup.TimeTransponder = chan->Transponder();
                           ::Setup.TimeSource = chan->Source();
                           ::Setup.Save();
                        }
                    }
                    Skins.Message(mtInfo, tr("Changes Done"));
                    if (hasTimers_ > 0)
                        Skins.Message(mtInfo, tr("Please check your Timers!"));
                    return osUser1;
                } else {
                    Skins.Message(mtError, tr("Changes failed"));
                }
            } else if (DirState() == 2) {
                Open();
                Set();
            } else if (DirState() == 0) { // if directory
                Open(true);     // back
                Set();
            }

            break;

            // case kYellow: state = Delete(); Set(); break;
            // case kBlue: return AddSubMenu(new cMenuBrowserCommands); ; Set(); break; /// make Commands Menu
            // Don`t forget Has || Had Submenus check
          case kBlue:
            if(!WizardMode_)
                AddSubMenu(new cMenuSelectChannelListFunctions());
            return osContinue;
        default:
            break;
        }
    }

    state = osUnknown;
    return state;
}

int cMenuSelectChannelList::DirState()
{
    cMenuChannelsItem *ci = (cMenuChannelsItem *) Get(Current());
    return ci ? ci->IsDirectory()? 2 : 1 : 0;
}

eOSState cMenuSelectChannelList::Open(bool Back)
{
    if (!Back) {
        level_++;
    } else {
        level_--;
        DirectoryFiles.Load(Back);
        return osUnknown;
    }

    cMenuChannelsItem *ci = (cMenuChannelsItem *) Get(Current());

    if (ci && ci->FileName()) {
        string fname = ci->FileName();
        if (Back) {
            DirectoryFiles.Load(Back);
            return osUnknown;
        } else {
            DirectoryFiles.Load(fname);
            return osUnknown;
        }
    }
    return osUnknown;
}

eOSState cMenuSelectChannelList::Delete()        // backup active list
{
    cMenuChannelsItem *ci = (cMenuChannelsItem *) Get(Current());
    if (ci) {
        if (Interface->Confirm(tr("Delete channel list?"))) {
            if (unlink(ci->FileName()) != 0) {
                int errnr = errno;
                esyslog("ERROR [channelscan]: can`t delete file %s errno: %d", ci->FileName(), errnr);
            }
        }
    }
    string tmp = "";
    DirectoryFiles.Load(tmp);   // reload current directory
    return osUnknown;
}

void cMenuSelectChannelList::DumpDir()
{
    for (cDirectoryEntry * d = DirectoryFiles.First(); d; d = DirectoryFiles.Next(d)) {
        DEBUG_CSMENU(" d->Entry: %s, Dir? %s %s  \n", d->Title(), d->IsDirectory()? "YES" : "NO", d->FileName());
    }
}

// --- Class cMenuSelectChannelListFunctions ---------------------------------------
cMenuSelectChannelListFunctions::cMenuSelectChannelListFunctions() : cOsdMenu(tr("Channel lists functions"))
{
    Set();
    Display();
}

cMenuSelectChannelListFunctions::~cMenuSelectChannelListFunctions()
{
}

void cMenuSelectChannelListFunctions::Set()
{
    SetHasHotkeys();
    Add(new cOsdItem(hk(tr("Import current channellist")), osUser1, true));
    Add(new cOsdItem(hk(tr("Export current channellist")), osUser2, true));
    if (::Setup.ReelboxModeTemp == eModeClient)
        Add(new cOsdItem(hk(tr("Import channels from ReelBox Avantgarde")), osUser3, true));
}

#endif
