// $Header: /var/lib/gforge/chroot/cvsroot/enigma2-plugins/enigma2-plugins/webinterface/src/web-data/config.js,v 1.38 2009-09-08 16:39:47 sreichholf Exp $

var DBG = true;
DBG = false;

var url_tpl = '/web-data/tpl/default/';

var url_getcurrent = '/web/getcurrent';

var url_getvolume = '/web/vol?set=state'; 
var url_setvolume = '/web/vol?set=set'; // plus new value eg: set=set15
var url_volumeup = '/web/vol?set=up';
var url_volumedown = '/web/vol?set=down';
var url_volumemute = '/web/vol?set=mute';

var url_epgservice = "/web/epgservice?sRef="; // plus serviceRef
var url_epgsearch = "/web/epgsearch?search="; // plus serviceRef
var url_epgservicenow = "/web/epgservicenow?sRef=" // plus serviceRef
var url_epgservicenext = "/web/epgservicenext?sRef=" // plus serviceRef
var url_epgnow = "/web/epgnow?bRef="; // plus bouquetRev
var url_epgnext = "/web/epgnext?bRef="; // plus bouquetRev

var url_getServices = "/web/getservices?sRef="; // plus serviceref
var url_subservices = "/web/subservices"; // subservices for current service
var url_streamsubservices = "/web/streamsubservices?sRef="; // subservices for streaming service

var url_updates= "/web/updates.html";

var url_movielist= "/web/movielist"; // plus dirname,tag

var url_about= "/web/about";

var url_settings= "/web/settings";

var url_parentcontrol= "/web/parentcontrollist";

var url_moviedelete= "/web/moviedelete"; // plus serviceref

var url_mediaplayerlist= "/web/mediaplayerlist?types=audio&path="; // plus full qualified path
var url_mediaplayerplay= "/web/mediaplayerplay?file="; // plus file-serviceref
var url_mediaplayercmd= "/web/mediaplayercmd?command="; // plus command
var url_mediaplayerwrite= "/web/mediaplayerwrite?filename="; // plus command

var url_filelist = "/web/mediaplayerlist?path="; // plus full qualified path

var url_timerlist= "/web/timerlist";
var url_recordnow= "/web/recordnow";
var url_timeradd= "/web/timeradd"; // plus serviceref,begin,end,name,description,dirname,tags,eit,disabled,justplay,afterevent
var url_timerchange= "/web/timerchange"; // plus serviceref,begin,end,name,description,dirname,tags,eit,disabled,justplay,afterevent
var url_timeraddbyeventid= "/web/timeraddbyeventid"; // plus serviceref,eventid
var url_timerdelete= "/web/timerdelete"; // plus serviceref,begin,end
var url_timerlistwrite="/web/timerlistwrite?write=saveWriteNow";
var url_timercleanup="/web/timercleanup?cleanup=true";

var url_getcurrlocation="/web/getcurrlocation";
var url_getlocations="/web/getlocations";
var url_gettags="/web/gettags";

var url_message = "/web/message"; // plus text,type,timeout
var url_messageanswer = "/web/messageanswer?getanswer=now"; 

var url_powerstate = "/web/powerstate"; // plus new powerstate
var url_remotecontrol = "/web/remotecontrol"; // plus command
var url_signal = "/web/signal";
var url_deviceinfo = "/web/deviceinfo";

var bouquetsTv = '1:7:1:0:0:0:0:0:0:0:(type == 1) || (type == 17) || (type == 195) || (type == 25) FROM BOUQUET "bouquets.tv" ORDER BY bouquet';
var bouquetsRadio = '1:7:2:0:0:0:0:0:0:0:(type == 2)FROM BOUQUET "bouquets.radio" ORDER BY bouquet';
var providerTv = '1:7:1:0:0:0:0:0:0:0:(type == 1) || (type == 17) || (type == 195) || (type == 25) FROM PROVIDERS ORDER BY name';
var providerRadio ='1:7:2:0:0:0:0:0:0:0:(type == 2) FROM PROVIDERS ORDER BY name';
var satellitesTv = '1:7:1:0:0:0:0:0:0:0:(type == 1) || (type == 17) || (type == 195) || (type == 25) FROM SATELLITES ORDER BY name';
var satellitesRadio ='1:7:2:0:0:0:0:0:0:0:(type == 2) FROM SATELLITES ORDER BY name';
var allTv = '1:7:1:0:0:0:0:0:0:0:(type == 1) || (type == 17) || (type == 195) || (type == 25) ORDER BY name';
var allRadio = '1:7:2:0:0:0:0:0:0:0:(type == 2) ORDER BY name';