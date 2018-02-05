#if !defined(AFX_SHANNONDLG_H__1E814C42_7163_4F64_A49D_1F8E036F1F06__INCLUDED_)
#define AFX_SHANNONDLG_H__1E814C42_7163_4F64_A49D_1F8E036F1F06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* ******************************************************************** **
** @@ struct CALC_OPERATOR
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

struct CALC_OPERATOR
{
   char              token;
   char*             tag;
   size_t            taglen;
   int               precedence;
};

class CShannonDlg : public CDialog
{
   private:

      int         _iCalcError;

// Construction
public:
   CShannonDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
   //{{AFX_DATA(CShannonDlg)
	enum { IDD = IDD_SHANNON };
   CString  m_P;
   CString  m_H;
   CString  m_Sum;
	CString	m_ALL;
	//}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CShannonDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   //{{AFX_MSG(CShannonDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnDestroy();
   afx_msg void OnCalc();
   afx_msg void OnBtnAll();
   afx_msg void OnBtnClear();
   afx_msg void OnBtnClearAll();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnLoad();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

      // Calculator
      double            PRNG();
      double            Gamma(double);
      double            Log2(double);
      int               DoOperation();
      int               DoParenthesis();
      void              PushOperation(char);
      void              PushArgument(double);
      int               PopArgument(double*);
      int               PopOperation(int*);
      char*             GetExpression(char*);
      CALC_OPERATOR*    GetOperator(char*);
      int               GetPrecedence(char);
      int               GetStackTopPrecedence();
      double            Evaluate(const CString& rExpr);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
