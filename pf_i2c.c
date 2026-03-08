#ifndef PF_USER_CUSTOM

#include "pforth/csrc/pf_all.h"

static cell_t I2CREAD( cell_t Val );
static void I2CWRITE( cell_t Val1, cell_t Val2 );

static cell_t I2CREAD( cell_t Val )
{
    MSG_NUM_D("I2CREAD: Val = ", Val);
    return Val+1;
}

static void I2CWRITE( cell_t Val1, cell_t Val2 )
{

    MSG("I2CWRITE: Val1 = "); ffDot(Val1);
    MSG_NUM_D(", Val2 = ", Val2);
}

#ifdef PF_NO_GLOBAL_INIT
/******************
** If your loader does not support global initialization, then you
** must define PF_NO_GLOBAL_INIT and provide a function to fill
** the table. Some embedded system loaders require this!
** Do not change the name of LoadCustomFunctionTable()!
** It is called by the pForth kernel.
*/
#define NUM_CUSTOM_FUNCTIONS  (2)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = I2CREAD;
    CustomFunctionTable[1] = I2CWRITE;
    return 0;
}

#else
/******************
** If your loader supports global initialization (most do.) then just
** create the table like this.
*/
CFunc0 CustomFunctionTable[] =
{
    (CFunc0) I2CREAD,
    (CFunc0) I2CWRITE
};
#endif

#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))
Err CompileCustomFunctions( void )
{
    Err err;
    int i = 0;
/* Compile Forth words that call your custom functions.
** Make sure order of functions matches that in LoadCustomFunctionTable().
** Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
    err = CreateGlueToC( "I2CREAD", i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "I2CWRITE", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;

    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

#endif  /* PF_USER_CUSTOM */

