#ifndef ___I18N_H
#define ___I18N_H

#include <vdr/i18n.h>

#if APIVERSNUM < 10507
extern const tI18nPhrase LoadepgPhrases[];
#define trNOOP(s) (s)
#endif

#endif //___I18N_H
