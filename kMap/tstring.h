#ifndef TSTRING_H
#define TSTRING_H

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#endif