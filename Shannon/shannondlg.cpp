#include "stdafx.h"

#include "math.h"
#include "float.h"

#include "..\shared\prng_mersenne_twister.h"
#include "..\shared\hash_hsieh.h"
#include "..\shared\math_gamma.h"
#include "..\shared\mmf.h"

#include "Shannon.h"
#include "ShannonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD      dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ pVerbs
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

static CALC_OPERATOR pVerbs[]  =
{
   {'(',  "(",       1, 0  }, 
   {'+',  "+",       1, 2  }, 
   {'-',  "-",       1, 3  }, 
   {'%',  "%",       1, 4  }, 
   {'*',  "*",       1, 4  },
   {'/',  "/",       1, 5  }, 
   {'\\', "\\",      1, 5  },    // Calculates the floating-point remainder
   {'^',  "^",       1, 6  }, 
   {')',  ")",       1, 99 }, 
   {'g',  "G(",      2, 0  }, 
   {'H',  "SH(",     3, 0  }, 
   {'h',  "CH(",     3, 0  }, 
   {'L',  "LN(",     3, 0  }, 
   {'G',  "LG(",     3, 0  }, 
   {'S',  "SIN(",    4, 0  }, 
   {'C',  "COS(",    4, 0  }, 
   {'A',  "ABS(",    4, 0  }, 
   {'E',  "EXP(",    4, 0  }, 
   {'t',  "TAN(",    4, 0  }, 
   {'I',  "ASIN(",   5, 0  }, 
   {'O',  "ACOS(",   5, 0  }, 
   {'l',  "LOG2(",   5, 0  }, 
   {'s',  "SQRT(",   5, 0  }, 
   {'T',  "ATAN(",   5, 0  }, 
   { 0,   NULL,      0, 0  }
};
                  
const double Pi = 3.141592653589793238462643383279502884197;
const double e  = 2.718281828459045235360287471352662497757;

static char       op_stack [MAX_PATH];    // Operator stack
static double     arg_stack[MAX_PATH];    // Argument stack
static char       token    [MAX_PATH];    // Token buffer

static int        op_sptr  = 0;           // op_stack pointer
static int        arg_sptr = 0;           // arg_stack pointer
static int        parens   = 0;           // Nesting level
static int        state    = 0;           // 0 = Awaiting expression, 1 = Awaiting operator

CShannonDlg::CShannonDlg(CWnd* pParent /*=NULL*/)
:  CDialog(CShannonDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CShannonDlg)
   m_P = _T("");
   m_H = _T("");
   m_Sum = _T("");
   m_ALL = _T("");
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   _iCalcError = 0;
}

void CShannonDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CShannonDlg)
   DDX_Text(pDX, IDC_EDT_P, m_P);
   DDV_MaxChars(pDX, m_P, 255);
   DDX_Text(pDX, IDC_EDT_H, m_H);
   DDV_MaxChars(pDX, m_H, 255);
   DDX_Text(pDX, IDC_EDT_SUM, m_Sum);
   DDV_MaxChars(pDX, m_Sum, 255);
   DDX_Text(pDX, IDC_REDT_ALL, m_ALL);
   DDV_MaxChars(pDX, m_ALL, 65536);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShannonDlg, CDialog)
   //{{AFX_MSG_MAP(CShannonDlg)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_CALC, OnCalc)
   ON_BN_CLICKED(IDC_BTN_ALL, OnBtnAll)
   ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
   ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnBtnClearAll)
   ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
   ON_BN_CLICKED(IDC_BTN_LOAD, OnBtnLoad)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CShannonDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);       // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon
   
   CenterWindow();
   
   return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShannonDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CShannonDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CShannonDlg::OnDestroy() 
{
   CDialog::OnDestroy();
   
   // TODO: Add your message handler code here
   
}

void CShannonDlg::OnCalc() 
{
   UpdateData(TRUE);

   m_P.Replace(',','.');

   double   fResult  = Evaluate(m_P);
   double   fEntropy = 0.0;

   if (fResult)
   {
      fEntropy = -fResult * Log2(fResult);
   }

   m_H.Format("%.15f",fEntropy);

   if (!m_ALL.IsEmpty())
   {
      m_ALL += "\n";
   }
   
   m_ALL += m_H;

   UpdateData(FALSE);
}

void CShannonDlg::OnBtnAll() 
{
   const char*    pszDelimiters = " \t\v\r\n";

   UpdateData(TRUE);

   if (m_ALL.IsEmpty())
   {
      // Nothing to do !
      return;
   }

   m_ALL.Replace(',','.');   // Float point 

   DWORD    dwSize = m_ALL.GetLength();

   char*    pBuf = new char[dwSize + 1];

   memset(pBuf,0,dwSize + 1);

   strncpy(pBuf,(LPCTSTR)m_ALL,dwSize);
   pBuf[dwSize] = 0;    // Ensure ASCIIZ

   double   fSum = 0.0;

   char*    pszLine = strtok(pBuf,pszDelimiters);

   while (pszLine && *pszLine)
   {
      double   fLine = atof(pszLine);

      fSum += fLine;
      
      pszLine = strtok(NULL,pszDelimiters);
   }      

   delete[] pBuf;
   pBuf = NULL;

   m_Sum.Format("%.15f",fSum);

   UpdateData(FALSE);
}

void CShannonDlg::OnBtnClear() 
{
   m_H = _T("");

   UpdateData(FALSE);
}

void CShannonDlg::OnBtnClearAll() 
{
   m_H   = _T("");
   m_ALL = _T("");

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CShannonDlg::PRNG()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

double CShannonDlg::PRNG()
{
   // Generate PRNG Seed
   GUID     _GUID = GUID_NULL;

   ::CoCreateGuid(&_GUID);

   MTRand      _PRNG;

   // Compress to applicable size & Initialize
   DWORD    dwSeed = SuperFastHash((BYTE*)&_GUID,sizeof(GUID));    
   
   _PRNG.seed(dwSeed);

   // Generate double random in (0,1)
   return _PRNG.randDblExc(); // 
}

/* ******************************************************************** **
** @@ CShannonDlg::Gamma()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

double CShannonDlg::Gamma(double fArg)
{
   if (fabs(fArg) > 170.0)
   {
      MessageBox("Argument out of range [0..170].\n\nCalculation aborted!","Euler Gamma function",MB_OK | MB_ICONSTOP);
      return 0.0;
   }

   return ::Gamma(fArg);
}

/* ******************************************************************** **
** @@ CShannonDlg::Log2()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

double CShannonDlg::Log2(double fArg)
{
   return log(fArg) / log(2.0);
}

/* ******************************************************************** **
** @@ CShannonDlg::DoOperation()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Evaluate stacked arguments and operands
** ******************************************************************** */

int CShannonDlg::DoOperation()
{
   double   arg1 = 0.0;
   double   arg2 = 0.0;

   int      op = 0;

   if (PopOperation(&op) == -1)
   {
      return -1;
   }

   PopArgument(&arg1);
   PopArgument(&arg2);
   
   switch (op)
   {  
      case '+':
      {
         PushArgument(arg2 + arg1);
         break;
      }
      case '-':
      {
         PushArgument(arg2 - arg1);
         break;
      }
      case '%':
      {
         PushArgument(arg2 * arg1 / 100.0);
         break;
      }
      case '*':
      {
         PushArgument(arg2 * arg1);
         break;
      }
      case '/':
      {
         if (fabs(arg1) < FLT_EPSILON)
         {
            return -1;
         }

         PushArgument(arg2 / arg1);
         break;
      }
      case '\\':  // Calculates the floating-point remainder
      {
         if (fabs(arg1) < FLT_EPSILON)
         {
            return -1;
         }

         PushArgument(fmod(arg2,arg1));
         break;
      }
      case '^':
      {
         PushArgument(pow(arg2,arg1));
         break;
      }
      case 't':   // TAN
      {
         ++arg_sptr; // Function()
         PushArgument(tan(arg1));
         break;
      }
      case 'S':   // SIN
      {
         ++arg_sptr; // Function()
         PushArgument(sin(arg1));
         break;
      }
      case 's':   // SQRT
      {
         ++arg_sptr; // Function()
         PushArgument(sqrt(arg1));
         break;
      }
      case 'C':   // COS
      {
         ++arg_sptr; // Function()
         PushArgument(cos(arg1));
         break;
      }
      case 'A':   // ABS
      {
         ++arg_sptr; // Function()
         PushArgument(fabs(arg1));
         break;
      }
      case 'L':   // LN
      {
         if (fabs(arg1) > FLT_EPSILON)
         {
            ++arg_sptr; // Function()
            PushArgument(log(arg1));
            break;
         }
         else
         {
            return -1;
         }
      }
      case 'E':   // EXP
      {
         ++arg_sptr; // Function()
         PushArgument(exp(arg1));
         break;
      }
      case 'H':   // SH
      {
         ++arg_sptr; // Function()
         PushArgument(sinh(arg1));
         break;
      }
      case 'h':   // CH
      {
         ++arg_sptr; // Function()
         PushArgument(cosh(arg1));
         break;
      }
      case 'g':   // Gamma
      {
         ++arg_sptr; // Function()
         PushArgument(Gamma(arg1));
         break;
      }
      case 'G':   // LG
      {
         ++arg_sptr; // Function()
         PushArgument(log10(arg1));
         break;
      }
      case 'I':   // ARCSIN
      {
         ++arg_sptr; // Function()
         PushArgument(asin(arg1));
         break;
      }
      case 'O':   // ARCCOS
      {
         ++arg_sptr; // Function()
         PushArgument(acos(arg1));
         break;
      }
      case 'l':   // LOG2
      {
         ++arg_sptr; // Function()
         PushArgument(Log2(arg1));
         break;
      }
      case 'T':   // ATAN
      {
         ++arg_sptr; // Function()
         PushArgument(atan(arg1));
         break;
      }
      case '(':
      {
         arg_sptr += 2;
         break;
      }
      default:
      {
         return -1;
      }
   }

   if (arg_sptr < 1)
   {
      return -1;
   }

   return op;
}

/* ******************************************************************** **
** @@ CShannonDlg::DoParenthesis()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Evaluate one level
** ******************************************************************** */

int CShannonDlg::DoParenthesis()
{
   int   op = 0;

   if (parens-- < 1)
   {
      return -1;
   }

   do
   {
      op = DoOperation();

      if (op < 0)
      {
         break;
      }
   }
   while (GetPrecedence((char)op));

   return op;
}

/* ******************************************************************** **
** @@ CShannonDlg::PushOperation()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Stack operations
** ******************************************************************** */

void CShannonDlg::PushOperation(char op)
{
   if (!GetPrecedence(op))
   {
      ++parens;
   }
  
   op_stack[op_sptr++] = op;
}

/* ******************************************************************** **
** @@ CShannonDlg::PushArgument()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CShannonDlg::PushArgument(double arg)
{
   arg_stack[arg_sptr++] = arg;
}

/* ******************************************************************** **
** @@ CShannonDlg::PopArgument()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

int CShannonDlg::PopArgument(double* arg)
{  
   *arg = arg_stack[--arg_sptr];
   
   return (arg_sptr < 0)  ?  -1  :  0;
}

/* ******************************************************************** **
** @@ CShannonDlg::PopOperation()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

int CShannonDlg::PopOperation(int* op)
{
   if (!op_sptr)
   {
      return -1;
   }

   *op = op_stack[--op_sptr];

   return 0;
}

/* ******************************************************************** **
** @@ CShannonDlg::GetExpression()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get an expression
** ******************************************************************** */

char* CShannonDlg::GetExpression(char* str)
{
   char*    ptr  = str; 
   char*    tptr = token;
  
   CALC_OPERATOR*    op = NULL;

   if (!strncmp(str,"PI",2))
   {
      return strcpy(token,"PI");
   }

   while (*ptr)
   {
      op = GetOperator(ptr);

      if (op)
      {
         if ('-' == *ptr)
         {
            if (str != ptr && 'E' != ptr[-1])
            {
               break;
            }

            if (str == ptr && !isdigit(ptr[1]) && '.' != ptr[1])
            {
               PushArgument(0.0);
               strcpy(token,op->tag);
               return token;
            }
         }
         else if (str == ptr)
         {
            strcpy(token,op->tag);
            return token;
         }
         else
         {
            break;
         }
      }

      *tptr++ = *ptr++;
   }

   *tptr = NULL;

   return token;
}

/* ******************************************************************** **
** @@ CShannonDlg::GetOperator()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get an operator
** ******************************************************************** */

CALC_OPERATOR* CShannonDlg::GetOperator(char* str)
{
   for (CALC_OPERATOR* pCO = pVerbs; pCO->token; ++pCO)
   {
      if (!strncmp(str,pCO->tag,pCO->taglen))
      {
         return pCO;
      }
   }

   return NULL;
}

/* ******************************************************************** **
** @@ CShannonDlg::GetPrecedence()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get precedence of a token
** ******************************************************************** */

int CShannonDlg::GetPrecedence(char token)
{
   for (CALC_OPERATOR* pCO = pVerbs; pCO->token; ++pCO)
   {
      if (pCO->token == token)
      {
         return pCO->precedence;
      }
   }

   return 0;
}

/* ******************************************************************** **
** @@ CShannonDlg::GetStackTopPrecedence()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Get precedence of TOS token
** ******************************************************************** */

int CShannonDlg::GetStackTopPrecedence()
{
   if (!op_sptr)
   {
      return 0;
   }

   return GetPrecedence(op_stack[op_sptr - 1]);
}

/* ******************************************************************** **
** @@ CShannonDlg::Evaluate()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

// Basically, EVAL.C converts infix notation to postfix notation. If you're
// not familiar with these terms, infix notation is standard human-readable
// equations such as you write in a C program. Postfix notation is most familiar
// as the "reverse Polish" notation used in Hewlett Packard calculators and in
// the Forth language. Internally, all languages work by converting to postfix
// evaluation. Only Forth makes it obvious to the programmer.
// 
// EVAL.C performs this conversion by maintaining 2 stacks, an operand stack
// and an operator stack. As it scans the input, each time it comes across a
// numerical value, it pushes it onto the operand stack. Whenever it comes
// across an operator, it pushes it onto the operator stack. Once the input
// expression has been scanned, it evaluates it by popping operands off the
// operand stack and applying the operators to them.
// 
// For example the simple expression "2+3-7" would push the values 2, 3, and 7
// onto the operand stack and the operators "+" and "-" onto the operator stack.
// When evaluating the expression, it would first pop 3 and 7 off the operand
// stack and then pop the "-" operator off the operator stack and apply it. This
// would leave a value of -4 on the stack. Next the value of 2 would be popped
// from the operand stack and the remaining operator off of the operator stack.
// Applying the "+" operator to the values 2 and -4 would leave the result of
// the evaluation, -2, on the stack to be returned.
// 
// The only complication of this in EVAL.C is that instead of raw operators
// (which would all have to be 1 character long), I use operator tokens which
// allow multicharacter operators and precedence specification. What I push on
// the operator stack is still a single character token, but its the operator
// token which is defined in the 'verbs' array of valid tokens. Multicharacter
// tokens are always assumed to include any leading parentheses. For example, in
// the expression "SQRT(37)", the token is "SQRT(".
// 
// Using parentheses forces evaluation to be performed out of the normal
// sequence. I use the same sort of mechanism to implement precedence rules.
// Unary negation is yet another feature which takes some explicit exception
// processing to process. Other than these exceptions, it's pretty
// straightforward stuff.
//                                                                       
//   EVAL.C - A simple mathematical expression evaluator in C            
//                                                                       
//   operators supported: Operator               Precedence              
//                                                                       
//                          (                     Lowest                 
//                          )                     Highest                
//                          +   (addition)        Low                    
//                          -   (subtraction)     Low                    
//                          *   (multiplication)  Medium                 
//                          /   (division)        Medium                 
//                          \   (modulus)         High                   
//                          ^   (exponentiation)  High                   
//                          sin(                  Lowest                 
//                          cos(                  Lowest                 
//                          atan(                 Lowest                 
//                          abs(                  Lowest                 
//                          sqrt(                 Lowest                 
//                          ln(                   Lowest                 
//                          exp(                  Lowest                 
//                                                                       
//   constants supported: pi                                             
//                                                                       
//   Original Copyright 1991-93 by Robert B. Stout as part of            
//   the MicroFirm Function Library (MFL)                                
//                                                                       
//   The user is granted a free limited license to use this source file  
//   to create royalty-free programs, subject to the terms of the        
//   license restrictions specified in the LICENSE.MFL file.             

double CShannonDlg::Evaluate(const CString& rExpr)
{
   char     pszExpr[MAX_PATH + 1];

   double      arg     = 0.0;
   double      fResult = 0.0;

   int         ercode  = 0;

   char*             str    = NULL; 
   char*             endptr = NULL;
   CALC_OPERATOR*    op     = NULL;

   CString     sExpr = rExpr;

   sExpr.MakeUpper();

   sExpr.Remove(0x08);
   sExpr.Remove(0x09);
   sExpr.Remove(0x0A);
   sExpr.Remove(0x0B);
   sExpr.Remove(0x0C);
   sExpr.Remove(0x0D);
   sExpr.Remove(0x20);

   memset(pszExpr,0,sizeof(pszExpr));

   strncpy(pszExpr,(LPCTSTR)sExpr,MAX_PATH);

   char*    ptr = pszExpr; 

   // Reset
   state    = 0;
   op_sptr  = 0;
   arg_sptr = 0; 
   parens   = 0;

   while (*ptr)
   {
      switch (state)
      {
         case 0:
         {
            str = GetExpression(ptr);

            if (str)
            {
               op = GetOperator(str);

               if (op && (strlen(str) == op->taglen))
               {
                  PushOperation(op->token);
                  ptr += op->taglen;
                  break;
               }

               if (!strcmp(str,"-"))
               {
                  PushOperation(*str);
                  ++ptr;
                  break;
               }

               if (!strcmp(str,"PI"))
               {
                  PushArgument(Pi);
               }
               else if (!strcmp(str,"E"))
               {
                  PushArgument(e);
               }
               else if (!strcmp(str,"RAND"))
               {
                  PushArgument(PRNG());
               }
               else
               {
                  arg = strtod(str,&endptr);

                  if ((fabs(arg) < FLT_EPSILON) && !strchr(str,'0'))
                  {
                     // Error !
                     _iCalcError = -1;
                     return 0.0;
                  }

                  PushArgument(arg);
               }

               ptr += strlen(str);
            }
            else
            {
               // Error !
               _iCalcError = -2;
               return 0.0;
            }

            state = 1;
            break;
         }
         case 1:
         {
            op = GetOperator(ptr);

            if (op)
            {
               if (*ptr == ')')
               {
                  ercode = DoParenthesis();

                  if (ercode < 0)
                  {
                     // Error !
                     _iCalcError = -3;
                     return 0.0;
                  }
               }
               else
               {
                  while (op_sptr && op->precedence <= GetStackTopPrecedence())
                  {
                     DoOperation();
                  }

                  PushOperation(op->token);
                  state = 0;
               }

               ptr += op->taglen;
            }
            else
            {
               // Error !
               _iCalcError = -4;
               return 0.0;
            }

            break;
         }
      }
   }

   while (arg_sptr > 1)
   {
      ercode = DoOperation();

      if (ercode < 0)
      {
         // Error !
         _iCalcError = -5;
         return 0.0;
      }
   }

   if (!op_sptr)
   {
      if (PopArgument(&fResult) < 0)
      {
         // Error !
         _iCalcError = -6;
         return 0.0;
      }

      return fResult;
   }

   // Error !
   _iCalcError = -7;
   return 0.0;
}

void CShannonDlg::OnBtnSave() 
{
   UpdateData(TRUE);

   char     pszNewFile[MAX_PATH + 1];

   memset(pszNewFile,0,sizeof(pszNewFile));

   DWORD    dwFlags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
      
   char     pszFilter[] = "*.txt\0*.txt\0*.*\0*.*\0\0";

   char     pszDrive[_MAX_DRIVE];
   char     pszDir  [_MAX_DIR];
   char     pszFName[_MAX_FNAME];
   char     pszExt  [_MAX_EXT];

   CWaitCursor    Waiter;

/*
   if (!_sCipher.IsEmpty())
   {
      _splitpath((LPCTSTR)_sCipher,pszDrive,pszDir,pszFName,pszExt);
   }
*/

   CString     sReport = _T("");

   OPENFILENAME         OFN;

   memset(&OFN,0,sizeof(OPENFILENAME));

   OFN.lStructSize     = sizeof(OPENFILENAME); 
   OFN.hwndOwner       = GetSafeHwnd();
   OFN.lpstrFilter     = pszFilter; 
   OFN.nFilterIndex    = 1;
   OFN.lpstrInitialDir = pszDir;
   OFN.lpstrFile       = pszNewFile;
   OFN.nMaxFile        = MAX_PATH;
   OFN.lpstrFileTitle  = NULL;
   OFN.nMaxFileTitle   = MAX_PATH;
   OFN.Flags           = dwFlags;

   if (GetSaveFileName(&OFN) == TRUE)
   {
      sReport = pszNewFile; 

      FILE*    pReport = fopen((LPCTSTR)sReport,"wt");

      if (!pReport)
      {
         // Error !
         ASSERT(0);
         return;
      }
      
      fprintf(pReport,"%s",(LPCTSTR)m_ALL);

      fclose(pReport);
      pReport = NULL;
   }
}

void CShannonDlg::OnBtnLoad() 
{
   DWORD    dwFlags =   OFN_ENABLESIZING     |
                        OFN_FILEMUSTEXIST    |
                        OFN_PATHMUSTEXIST;
                              
   char     pszFilter[MAX_PATH] = "TXT  (*.txt)|*.txt|"
                                  "ALL  (*.*)|*.*||";

   CFileDialog    FileSrc(TRUE,NULL,NULL,dwFlags,pszFilter);

   CWaitCursor    Waiter;

   if (FileSrc.DoModal() == IDOK)
   {  
      CString    sReport = FileSrc.GetPathName();

      MMF      _MF;

      bool     bOpen = _MF.OpenReadOnly((LPCTSTR)sReport) == TRUE;

      if (!bOpen)
      {
         // Error !
         ASSERT(0);

         CString     sMessage = _T("");

         sMessage.Format("Can't Open file [%s] for Read.",(LPCTSTR)sReport);

         MessageBox(sMessage,"Error",MB_OK | MB_ICONEXCLAMATION);
         return;
      }

      m_ALL = _MF.Buffer();

      _MF.Close();
   }

   UpdateData(FALSE);
}
