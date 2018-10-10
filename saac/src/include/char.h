#ifndef _CHAR_H_
#define _CHAR_H_

#include "version.h"

#define MAXCHAR_PER_USER 2

int isLocked(char *id);

void charLoadCallback(int ti, int auth_ok, char *c0, char *c1,
                      char *c2, char *c3, char *c4, int i0, int i1);int charSave(int ti, char *c0,
             char *c2, char *c3, char *c4, int i0, int i1, int charindex);

void charListCallback(int ti, int auth_ok, char *c0, char *c1,
                      char *c2, char *c3, char *c4, int i0, int i1);

void charDeleteCallback(int ti, int auth_ok,
                        char *c0, char *c1, char *c2, char *c3,
                        char *c4, int i0, int i1);

int lockUser(char *gmsvname, char *id, char *passwd, int lock,
             char *result, int resultlen,
             char *retdata, int retdatalen, char *process, char *deadline);

int loadCharOne(char *idstring, int num, char *output, int outlen);

int saveCharOne(char *idstring, int num, char *input);

int loadCharNameAndOption(char *id, char *output, int outlen);

int getCharIndexByName(char *id, char *charname);

void getCharInfoFromString(char *str, char *out);

#endif

