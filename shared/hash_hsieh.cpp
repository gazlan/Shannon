/* ******************************************************************** **
** @@ SuperFastHash
** @  Copyrt : © Copyright 2004-2008 by Paul Hsieh
** @  Author : Paul Hsieh
** @  Modify :
** @  Update :
** @  Notes  : http://www.azillionmonkeys.com/qed/dwHash.html
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "hash_hsieh.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ Get16bits()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

inline WORD Get16bits(const void* const pData)      
{
   return (*((const WORD*)(pData)));
}

/* ******************************************************************** **
** @@ SuperFastHash()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

DWORD __fastcall SuperFastHash(const BYTE* pBuf,DWORD dwSize)
{
   if (!dwSize || !pBuf)
   {
      // Error !
      ASSERT(0);
      return 0;
   }

   if (IsBadReadPtr(pBuf,dwSize))
   {
      // Error !
      ASSERT(0);
      return 0;
   }

   DWORD    dwHash = dwSize;

   int      iRem = dwSize & 3;

   dwSize >>= 2;

   // Main loop
   for ( ; dwSize > 0; dwSize--)
   {
      dwHash += Get16bits(pBuf);

      DWORD    dwTemp = (Get16bits(pBuf + 2) << 11) ^ dwHash;
      
      dwHash = (dwHash << 16) ^ dwTemp;
      
      pBuf += 2 * sizeof(WORD);
      
      dwHash += dwHash >> 11;
   }

   // Handle end cases 
   switch (iRem)
   {
      case 3:
      {
         dwHash += Get16bits(pBuf);
         dwHash ^= dwHash << 16;
         dwHash ^= pBuf[sizeof(WORD)] << 18;
         dwHash += dwHash >> 11;
         break;
      }
      case 2:
      {
         dwHash += Get16bits(pBuf);
         dwHash ^= dwHash << 11;
         dwHash += dwHash >> 17;
         break;
      }
      case 1:
      {
         dwHash += *pBuf;
         dwHash ^= dwHash << 10;
         dwHash += dwHash >> 1;
         break;
      }
   }

   // Force "avalanching" of final 127 bits 
   dwHash ^= dwHash << 3;
   dwHash += dwHash >> 5;
   dwHash ^= dwHash << 4;
   dwHash += dwHash >> 17;
   dwHash ^= dwHash << 25;
   dwHash += dwHash >> 6;

   return dwHash;
}

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
