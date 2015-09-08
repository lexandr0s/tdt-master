#ifdef USE_WAREAGLEICON
/*
 * iconpatch.h: Information of iconpatch
 *
 * Diese Datei ist die �bersichtsdatei f�r den Iconpatch.
 * Hier werden kleine Infos abgelegt.
 * Der Iconpatch �ndert die Dateien:
 *   iconpatch.h
 *   menu.c
 *   recording.c
 *   fontosd.c
 *
 */

// Iconpatch-Variablen - Anfang
#define ICON_NUMBERSIGN        "\x23"
#define ICON_ASTERISK          "\x2A"
#define ICON_GREATER           "\x3E"
#define ICON_EXCLAM            "\x21"
#define ICON_PLUSMINUS         "\xB1"

#define ICON_RESUME            "\x80"
#define ICON_DVD               "\x81"
#define ICON_FOLDER            "\x82"
#define ICON_BLANK             "\x83"
#define ICON_CUTTING           "\x84"
#define ICON_MOVE_FILE         "\x85"
#define ICON_MOVE_FOLDER       "\x86"
#define ICON_BAR_START         "\x87"
#define ICON_BAR_FILLED        "\x88"
#define ICON_BAR_CLEAR         "\x89"
#define ICON_BAR_END           "\x8A"
#define ICON_REC               "\x8B"
#define ICON_CLOCK             "\x8C"
#define ICON_TV_CRYPTED        "\x8D"
#define ICON_RADIO             "\x8E"
#define ICON_TV                "\x8F"
#define ICON_NEW               "\x90"
#define ICON_ARROW             "\x91"
#define ICON_RUNNING           "\x92"
#define ICON_VPS               "\x93"
#define ICON_CLOCK_UH          "\x94"
#define ICON_CLOCK_LH          "\x95"

// UTF-8 Icons
#define ICON_RESUME_UTF8       "\uE000"
#define ICON_DVD_UTF8          "\uE001"
#define ICON_FOLDER_UTF8       "\uE002"
#define ICON_BLANK_UTF8        "\uE003"
#define ICON_CUTTING_UTF8      "\uE004"
#define ICON_MOVE_FILE_UTF8    "\uE005"
#define ICON_MOVE_FOLDER_UTF8  "\uE006"
#define ICON_BAR_START_UTF8    "\uE007"
#define ICON_BAR_FILLED_UTF8   "\uE008"
#define ICON_BAR_EMPTY_UTF8    "\uE009"
#define ICON_BAR_CLOSE_UTF8    "\uE00A"
#define ICON_REC_UTF8          "\uE00B"
#define ICON_CLOCK_UTF8        "\uE00C"
#define ICON_TV_CRYPTED_UTF8   "\uE00D"
#define ICON_RADIO_UTF8        "\uE00E"
#define ICON_TV_UTF8           "\uE00F"
#define ICON_NEW_UTF8          "\uE010"
#define ICON_ARROW_UTF8        "\uE011"
#define ICON_RUNNING_UTF8      "\uE012"
#define ICON_VPS_UTF8          "\uE013"
#define ICON_CLOCK_UH_UTF8     "\uE014"
#define ICON_CLOCK_LH_UTF8     "\uE015"

// Iconpatch-Variablen - Ende

bool IsLangUtf8(void);

#endif /* WAREAGLEICON */
