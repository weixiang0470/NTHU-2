/**CFile***********************************************************************

  FileName    [main.c]

  PackageName [ntr]

  Synopsis    [Main program for the nanotrav program.]

  Description []

  SeeAlso     []

  Author      [Fabio Somenzi/Priyank Kalla]

  Copyright   [Copyright (c) 1995-2004, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.]

******************************************************************************/

#include "ntr.h"
#include "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define NTR_VERSION\
    "Nanotrav Version #0.12, Release date 2003/12/31"

#define BUFLENGTH 8192

/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

#ifndef lint
static char rcsid[] UTIL_UNUSED = "$Id: main.c,v 1.38 2004/08/13 18:28:28 fabio Exp fabio $";
#endif

static  char    buffer[BUFLENGTH];
#ifdef DD_DEBUG
extern  st_table *checkMinterms (BnetNetwork *net, DdManager *dd, st_table *previous);
#endif

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
/*-- Priyank: We don't need any of these--*/
/***************
static NtrOptions * mainInit ();
static void ntrReadOptions (int argc, char **argv, NtrOptions *option);
static void ntrReadOptionsFile (char *name, char ***argv, int *argc);
static char* readLine (FILE *fp);
static FILE * open_file (char *filename, const char *mode);
static int reorder (BnetNetwork *net, DdManager *dd, NtrOptions *option);
static void freeOption (NtrOptions *option);
static DdManager * startCudd (NtrOptions *option, int nvars);
static int ntrReadTree (DdManager *dd, char *treefile, int nvars);
******************/

/**AutomaticEnd***************************************************************/

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Main program for ntr.]

  Description [Main program for ntr. Performs
  initialization. Introduces variables, builds ROBDDs for some
  functions, and prints-out the BDD structure. A sample file given to
  the 5740 class.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
void print_bdd_minterms(DdManager* dd, DdNode* f, int var_num);
void export_bdd_to_dot(DdManager* dd, DdNode* f, const char* filename, char** input_names, char** output_names);

int
main(
  int  argc,
  char ** argv)
{
    NtrOptions	*option;	/* options */
    DdManager	*dd;		/* pointer to DD manager */
    int		exitval;	/* return value of Cudd_CheckZeroRef */
    int		ok;		/* overall return value from main() */
    int		result;		/* stores the return value of functions */
    int		i;		/* loop index */
    int		j;		/* loop index */
    double	*signatures;	/* array of signatures */
    int		pr;		/* verbosity level */
    int		reencoded;	/* linear transformations attempted */
    int var_num;
    char* onames[1] = {"f"};

    /****** Priyank's additions *******/
    DdNode *one, *zero;
    DdNode *a, *b, *c, *d, *e, *f, *g, *h;
    DdNode *f_a, *f_abar;// Case 0
    DdNode *ab, *ac, *bc, *tmp; // Case 1
    DdNode *w, *x, *y, *z; // Case 2
    DdNode *cd, *ef; // Case 3
    DdNode *x1, *x2, *x3; // Case 4 ~ 6
    DdNode *f_from_AND_OR, *f_from_ITE; // Case 4
    /*********************************/

    /* Initialize manager. We start with 0 variables */
    dd = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    /*startCudd(option,net1->ninputs);*/
    if (dd == NULL) { exit(2); }

    /************ lets do our work here *************/
    one = Cudd_ReadOne( dd );
    Cudd_Ref(one); /* referenced for the first time */
    zero = Cudd_Not( one ); /* not the same as Cudd_ReadZero */
    Cudd_Ref(zero);
    /* Cudd_Ref(zero); */
    /* reference count increment not needed because Cudd_Not returns the projection */ 

    /***** Add a new variable ***********/
    printf("\nCase 0: f = ab + a'c\n");
    var_num = 3;
    a = Cudd_bddNewVar(dd);
    b = Cudd_bddNewVar(dd);
    c = Cudd_bddNewVar(dd);
    f = Cudd_bddIte(dd, a, b, c); /* compute ITE(a, b, c); */
    Cudd_Ref(f);
    /* print BDD structure of f */
    printf("Printing the BDD for f: ptr to the nodes, T & E children\n");
    Cudd_PrintDebug(dd, f, 3, 3); 

    char* inames_0[3] = {"a", "b", "c"};
    print_bdd_minterms(dd, f, var_num);
    export_bdd_to_dot(dd, f, "bdd_case0.dot", inames_0, onames);

    f_a = Cudd_Cofactor(dd, f, a); Cudd_Ref(f_a);
    /* f_a = +ve cofactor of f w.r.t. a: f_a = b */
    f_abar = Cudd_Cofactor(dd, f, Cudd_Not(a) ); Cudd_Ref(f_abar);
    /* f_abar = -ve cofactor of f w.r.t. a: f_abar = c */
    g = Cudd_bddAnd(dd, f_a, f_abar); Cudd_Ref(g);
    printf("Printing the BDD for g = b AND c\n");
    Cudd_PrintDebug( dd, g, 3, 3); 
    h = Cudd_bddAnd(dd, b, c); Cudd_Ref(h);
    if( h == g ){
      printf("Equivalence h = g means same DdNode pointers\n");
    }
    else{
      printf("Equivalence h = g does not mean same DdNode pointers? Something must be wrong\n");
    }


    // Case 1
    printf("\nCase 1: f = ab + ac + bc\n");
    var_num = 3;
    DdManager	*dd1;
    dd1 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd1 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd1); Cudd_Ref(one);
    zero = Cudd_Not( one );

    a = Cudd_bddNewVar(dd1);
    b = Cudd_bddNewVar(dd1);
    c = Cudd_bddNewVar(dd1);
    ab = Cudd_bddAnd(dd1, a, b); Cudd_Ref(ab);
    ac = Cudd_bddAnd(dd1, a, c); Cudd_Ref(ac);
    bc = Cudd_bddAnd(dd1, b, c); Cudd_Ref(bc);
    tmp = Cudd_bddOr(dd1, ab, ac); Cudd_Ref(tmp);
    f = Cudd_bddOr(dd1, tmp, bc); Cudd_Ref(f);
    Cudd_PrintDebug(dd1, f, var_num, 3);

    char* inames_1[3] = {"a", "b", "c"};
    print_bdd_minterms(dd1, f, var_num);
    export_bdd_to_dot(dd1, f, "bdd_case1.dot", inames_1, onames);


    // 清理（選擇性）
    Cudd_RecursiveDeref(dd1, ab);
    Cudd_RecursiveDeref(dd1, ac);
    Cudd_RecursiveDeref(dd1, bc);
    Cudd_RecursiveDeref(dd1, tmp);
    Cudd_RecursiveDeref(dd1, f);
    Cudd_RecursiveDeref(dd1, one);
    Cudd_Quit(dd1);

    // Case 2
    printf("\nCase 2: Given a truth table, convert it into a BDD\n");
    printf("Method 1: Sum of Product\n");
    var_num = 4;
    DdManager	*dd2;
    dd2 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd2 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd2); Cudd_Ref(one);
    zero = Cudd_Not( one );

    w = Cudd_bddNewVar(dd2);
    x = Cudd_bddNewVar(dd2);
    y = Cudd_bddNewVar(dd2);
    z = Cudd_bddNewVar(dd2);

    DdNode* vars[4] = {w, x, y, z};
    DdNode* cubes[7];

    int cube[7][4] = {
      {1, 1, 0, 2}, // 第一列: w=1 x=1 y=0 z=don't care
      {1, 2, 0, 1}, // 第二列: w=1, x=don't care, y=0, z=1
      {1, 2, 2, 2},
      {2, 1, 2, 1},
      {0, 1, 1, 2},
      {0, 0, 2, 2},
      {0, 0, 1, 0}
    };
    for(i = 0; i < 7; i++){
      cubes[i] = Cudd_bddComputeCube(dd2, vars, cube[i], 4); Cudd_Ref(cubes[i]);
    }

    // 把所有 c1 ~ c7 做 OR
    f = zero;
    for( i = 0; i < 7; i++){
      tmp  = Cudd_bddOr(dd2, f, cubes[i]); Cudd_Ref(f);
      Cudd_RecursiveDeref(dd2, f);
      f = tmp;
    }
    Cudd_PrintDebug(dd2, f, var_num, 3);
    char* inames_2[4] = {"w", "x", "y", "z"};
    print_bdd_minterms(dd2, f, var_num);
    export_bdd_to_dot(dd2, f, "bdd_case2.dot", inames_2, onames);

    // method 2
    printf("Method 2: f = w + x' + y + z\n");
    dd2 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd2 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd2); Cudd_Ref(one);
    zero = Cudd_Not( one );

    w = Cudd_bddNewVar(dd2);
    x = Cudd_bddNewVar(dd2);
    y = Cudd_bddNewVar(dd2);
    z = Cudd_bddNewVar(dd2);
    tmp = Cudd_bddOr(dd2, w, Cudd_Not(x)); Cudd_Ref(tmp);
    tmp = Cudd_bddOr(dd2, tmp, z); Cudd_Ref(tmp);
    f = Cudd_bddOr(dd2, tmp, y); Cudd_Ref(f);
    Cudd_PrintDebug(dd2, f, var_num, 3);

    print_bdd_minterms(dd2, f, var_num);
    export_bdd_to_dot(dd2, f, "bdd_case2_.dot", inames_2, onames);
   
    // Case 3
    printf("\nCase 3: f = ab + cd + ef\n");
    var_num = 6;
    DdManager	*dd3;
    dd3 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd3 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd3); Cudd_Ref(one);
    zero = Cudd_Not( one );

    a = Cudd_bddNewVar(dd3);
    b = Cudd_bddNewVar(dd3);
    c = Cudd_bddNewVar(dd3);
    d = Cudd_bddNewVar(dd3);
    e = Cudd_bddNewVar(dd3);
    f = Cudd_bddNewVar(dd3);
    ab = Cudd_bddAnd(dd3, a, b); Cudd_Ref(ab);
    cd = Cudd_bddAnd(dd3, c, d); Cudd_Ref(cd);
    ef = Cudd_bddAnd(dd3, e, f); Cudd_Ref(ef);
    tmp = Cudd_bddOr(dd3, ab, cd); Cudd_Ref(tmp);
    f = Cudd_bddOr(dd3, tmp, ef); Cudd_Ref(f);
    Cudd_PrintDebug(dd3, f, var_num, 3);

    char* inames_3[6] = {"a", "b", "c", "d", "e", "f"};
    print_bdd_minterms(dd3, f, var_num);
    export_bdd_to_dot(dd3, f, "bdd_case3.dot", inames_3, onames);

    // Case 4
    printf("\nCase 4: f = x1'x2'x3' + x1'x2x3' + x1'x2'x3 + x1x2'x3 + x1x2x3' + x1x2x3\n");
    var_num = 3;
    DdManager	*dd4;
    dd4 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd4 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd4); Cudd_Ref(one);
    zero = Cudd_Not( one );

    x1 = Cudd_bddNewVar(dd4);
    x2 = Cudd_bddNewVar(dd4);
    x3 = Cudd_bddNewVar(dd4);

    // method 1
    printf("Method 1: Use AND OR combination\n");
    tmp = Cudd_bddAnd(dd4, Cudd_Not(x1), Cudd_Not(x2)); Cudd_Ref(tmp);
    a = Cudd_bddAnd(dd4, tmp, Cudd_Not(x3)); Cudd_Ref(a);
    tmp = Cudd_bddAnd(dd4, Cudd_Not(x1), x2); Cudd_Ref(tmp);
    b = Cudd_bddAnd(dd4, tmp, Cudd_Not(x3)); Cudd_Ref(b);
    tmp = Cudd_bddAnd(dd4, Cudd_Not(x1), Cudd_Not(x2)); Cudd_Ref(tmp);
    c = Cudd_bddAnd(dd4, tmp, x3); Cudd_Ref(c);
    tmp = Cudd_bddAnd(dd4, x1, Cudd_Not(x2)); Cudd_Ref(tmp);
    d = Cudd_bddAnd(dd4, tmp, x3); Cudd_Ref(d);
    tmp = Cudd_bddAnd(dd4, x1, x2); Cudd_Ref(tmp);
    e = Cudd_bddAnd(dd4, tmp, Cudd_Not(x3)); Cudd_Ref(e);
    tmp = Cudd_bddAnd(dd4, x1, x2); Cudd_Ref(tmp);
    g = Cudd_bddAnd(dd4, tmp, x3); Cudd_Ref(g);

    tmp = Cudd_bddOr(dd4, a, b); Cudd_Ref(tmp);
    tmp = Cudd_bddOr(dd4, tmp, c); Cudd_Ref(tmp);
    tmp = Cudd_bddOr(dd4, tmp, d); Cudd_Ref(tmp);
    tmp = Cudd_bddOr(dd4, tmp, e); Cudd_Ref(tmp);
    f_from_AND_OR = Cudd_bddOr(dd4, tmp, g); Cudd_Ref(f);

    Cudd_PrintDebug(dd4, f_from_AND_OR, var_num, 3);

    // method 2
    printf("Method 2: ITE\n");
    dd4 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd4 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd4); Cudd_Ref(one);
    zero = Cudd_Not( one );
    x1 = Cudd_bddNewVar(dd4);
    x2 = Cudd_bddNewVar(dd4);
    x3 = Cudd_bddNewVar(dd4);

    a = Cudd_bddIte(dd4, x2, one, x3); // if x1
    b = Cudd_bddIte(dd4, x2, Cudd_Not(x3), one); // if x1'
    f_from_ITE = Cudd_bddIte(dd4, x1, a, b);
    Cudd_PrintDebug(dd4, f_from_ITE, var_num, 3);

    if (f_from_AND_OR == f_from_ITE) {
      printf("The f generated by the two methods are logically equivalent!\n");
    }

    char* inames_4[3] = {"x1", "x2", "x3"};
    print_bdd_minterms(dd4, f_from_ITE, var_num);
    export_bdd_to_dot(dd4, f_from_ITE, "bdd_case4.dot", inames_4, onames);


    // Case 5
    printf("\nCase 5: f = x1'x3' + x2'x3 + x1x2\n");
    var_num = 3;
    DdManager	*dd5;
    dd5 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd5 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd5); Cudd_Ref(one);
    zero = Cudd_Not( one );

    x1 = Cudd_bddNewVar(dd5);
    x2 = Cudd_bddNewVar(dd5);
    x3 = Cudd_bddNewVar(dd5);

    a = Cudd_bddIte(dd5, x2, one, x3); // if x1
    b = Cudd_bddIte(dd5, x2, Cudd_Not(x3), one); // if x1'
    f = Cudd_bddIte(dd5, x1, a, b);
    Cudd_PrintDebug(dd5, f, var_num, 3);

    char* inames_5[3] = {"x1", "x2", "x3"};
    print_bdd_minterms(dd5, f, var_num);
    export_bdd_to_dot(dd5, f, "bdd_case5.dot", inames_5, onames);

    // Case 6
    printf("\nCase 6: f = x1'x2' + x2x3' + x1x3\n");
    var_num = 3;
    DdManager	*dd6;
    dd6 = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
    if (dd6 == NULL) { exit(2); }
    one = Cudd_ReadOne(dd6); Cudd_Ref(one);
    zero = Cudd_Not( one );

    x1 = Cudd_bddNewVar(dd6);
    x2 = Cudd_bddNewVar(dd6);
    x3 = Cudd_bddNewVar(dd6);

    a = Cudd_bddIte(dd6, x2, one, x3); // if x1
    b = Cudd_bddIte(dd6, x2, Cudd_Not(x3), one); // if x1'
    f = Cudd_bddIte(dd6, x1, a, b);
    Cudd_PrintDebug(dd6, f, var_num, 3);

    char* inames_6[3] = {"x1", "x2", "x3"};
    print_bdd_minterms(dd6, f, var_num);
    export_bdd_to_dot(dd6, f, "bdd_case6.dot", inames_6, onames);
    exit(0);

} /* end of main */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/
void print_bdd_minterms(DdManager* dd, DdNode* f, int var_num) {
    printf("==== Minterms ====\n");
    DdGen *gen;
    int *inputs;
    CUDD_VALUE_TYPE val;

    Cudd_ForeachCube(dd, f, gen, inputs, val) {
        printf("f = 1 when: ");
        int i;
        for (i = 0; i < var_num; i++) {
            if (inputs[i] == 0)
                printf("x%d=0 ", i+1);
            else if (inputs[i] == 1)
                printf("x%d=1 ", i+1);
            else
                printf("x%d=? ", i+1); // don't care
        }
        printf("\n");
    }
}

void export_bdd_to_dot(DdManager* dd, DdNode* f, const char* filename, char** input_names, char** output_names) {
    FILE *dotFile = fopen(filename, "w");
    if (!dotFile) {
        fprintf(stderr, "Error opening file %s for writing.\n", filename);
        return;
    }
    DdNode* farray[1] = {f};
    // Cudd_AutodynDisable(dd, CUDD_REORDER_SIFT);
    Cudd_DumpDot(dd, 1, farray, input_names, output_names, dotFile);
    fclose(dotFile);
    printf("\nDOT file '%s' generated.\n", filename);
}
