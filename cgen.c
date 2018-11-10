/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the TINY compiler                            */
/* (generates code for the TM machine)              */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

/* tmpOffset is the memory offset for temps
   It is decremented each time a temp is
   stored, and incremeted when loaded again
*/
static int tmpOffset = 0;

static void saveTmp(void);
static void loadTmp(void);

/* prototype for internal recursive code generator */
static void cGen (TreeNode * tree);


static void saveTmp(void){

  switch(tmpOffset){
    case 0:
      emitRO("ST",ac, tmpOffset--, ac1, "op: push left");       /*emitRO não opera com as instruções LD e ST*/
      break;
    case (-1):
      emitRO("ST",ac, tmpOffset--, reg_temp2, "op: push left");
      break;
    case (-2):
      emitRO("ST",ac, tmpOffset--, reg_temp3, "op: push left");
      break;
    case (-3):
      break;
    case (-4):
      break;

    default:
      break;
  }
}

static void loadTmp(void){

  switch(tmpOffset){
    /*case 0:
      emitRO("LD",ac1,++tmpOffset,reg_temp1,"op: load left");
      break;*/
    case (-1):
      emitRO("LD",ac1,++tmpOffset,reg_temp1,"op: load left");   /*emitRO não opera com as instruções LD e ST*/
      break;
    case (-2):
      emitRO("LD",ac1,++tmpOffset,reg_temp2,"op: load left");
      break;
    case (-3):
      emitRO("LD",ac1,++tmpOffset,reg_temp3,"op: load left");
      break;
    case (-4):
      break;

    default:
      break;
  }
}

/* Procedure genStmt generates code at a statement node */
static void genStmt( TreeNode *tree){
 
  TreeNode *p1, *p2, *p3;
  int savedLoc1, savedLoc2, currentLoc;
  int loc;
  switch (tree->kind.stmt) {

      case IfK :
         if (TraceCode) emitComment("-> if") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         p3 = tree->child[2] ;
         /* generate code for test expression */
         cGen(p1);
         savedLoc1 = emitSkip(1) ;
         emitComment("if: jump to else belongs here");
         /* recurse on then part */
         cGen(p2);
         savedLoc2 = emitSkip(1) ;
         emitComment("if: jump to end belongs here");
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc1) ;
         emitRM_Abs("JEQ",ac,currentLoc,"if: jmp to else");
         emitRestore() ;
         /* recurse on else part */
         cGen(p3);
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc2) ;
         emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
         emitRestore() ;
         if (TraceCode)  emitComment("<- if") ;
         break; /* if_k */

      case RepeatK:
         if (TraceCode) emitComment("-> repeat") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         savedLoc1 = emitSkip(0);
         emitComment("repeat: jump after body comes back here");
         /* generate code for body */
         cGen(p1);
         /* generate code for test */
         cGen(p2);
         emitRM_Abs("JEQ",ac,savedLoc1,"repeat: jmp back to body");
         if (TraceCode)  emitComment("<- repeat") ;
         break; /* repeat */

      case AssignK:
         if (TraceCode) emitComment("-> assign") ;
         /* generate code for rhs */
         cGen(tree->child[0]);
         /* now store value */
         loc = st_lookup(tree->attr.name);
         emitRM("ST",ac,loc,gp,"assign: store value");
         if(tmpOffset < 0)
          ++tmpOffset;

         if (TraceCode)  emitComment("<- assign") ;
         break; /* assign_k */

      case ReadK:
         emitRO("IN",ac,0,0,"read integer value");
         loc = st_lookup(tree->attr.name);
         emitRM("ST",ac,loc,gp,"read: store value");
         break;
      case WriteK:
         /* generate code for expression to write */
         cGen(tree->child[0]);
         /* now output it */
         emitRO("OUT",ac,0,0,"write ac");
         break;
      default:
         break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp( TreeNode * tree){ 
  
  int loc;
  TreeNode *p1, *p2;
  
  switch (tree->kind.exp) {

    case ConstK :
      if (TraceCode) emitComment("-> Const") ;

/***************************** Alteração *************************************/
      /* gen code to load integer constant using LDC */
      if(tmpOffset < 0){
        emitRM("LDC",ac1,tree->attr.val,0,"load const"); 
        ++tmpOffset;
      }else{
        emitRM("LDC",ac,tree->attr.val,0,"load const"); 
        tmpOffset--;
      }
/***************************** FIM *******************************************/
      if (TraceCode)  emitComment("<- Const") ;
      break; /* ConstK */
    
    case IdK :
      if (TraceCode) emitComment("-> Id") ;
      loc = st_lookup(tree->attr.name);
/***************************** Alteração *************************************/
      if(tmpOffset < 0){
        emitRM("LD",ac1,loc,gp,"load id value");
        ++tmpOffset;
      }
      else{
        emitRM("LD",ac,loc,gp,"load id value");
        --tmpOffset;
      }
/***************************** FIM *******************************************/
      if(TraceCode)
        emitComment("<- Id") ;
      break; /* IdK */

    case OpK :
        if (TraceCode) emitComment("-> Op") ;
        p1 = tree->child[0];
        p2 = tree->child[1];

        /* gen code for ac = left arg */
        cGen(p1);

        /* gen code to push left operand */
        /*if(tmpOffset < -4)
          /*emitRM("ST",ac,tmpOffset--,mp,"op: push left");    /*dMem[tmpOffset+reg[mp]]  <-- ac*/
      
        /* gen code for ac = right operand */
        cGen(p2);

        /* now load left operand */ 
        /*if(tmpOffset > -4)
          /*emitRM("LD",ac1,++tmpOffset,mp,"op: load left");   /*ac1 <- dMem[tmpOffset+reg[mp]]*/
        
        switch (tree->attr.op){
            case PLUS :
               emitRO("ADD",ac,ac,ac1,"op +");
               break;
            case MINUS :
               emitRO("SUB",ac,ac,ac1,"op -");
               break;
            case TIMES :
               emitRO("MUL",ac,ac,ac1,"op *");
               break;
            case OVER :
               emitRO("DIV",ac,ac,ac1,"op /");
               break;
            case LT :
               emitRO("SUB",ac,ac,ac1,"op <") ;
               emitRM("JLT",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case EQ :
               emitRO("SUB",ac,ac,ac1,"op ==") ;
               emitRM("JEQ",ac,2,pc,"br if true");
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            default:
               emitComment("BUG: Unknown operator");
               break;
         } /* case op */
         if (TraceCode)  emitComment("<- Op") ;
         break; /* OpK */

    default:
      break;
  }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen( TreeNode * tree){

  if (tree != NULL){
    switch (tree->nodekind) {
      case StmtK:
        /*printf("chamando genStmt\n");*/
        genStmt(tree);
        break;

      case ExpK:
        /*printf("chamando genExp\n");*/
        genExp(tree);
        break;

      default:
        break;
    }
    cGen(tree->sibling);
  }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile){
 
   char * s = malloc(strlen(codefile)+7);
   strcpy(s,"File: ");
   strcat(s,codefile);
   emitComment("TINY Compilation to TM Code");
   emitComment(s);
   /* generate standard prelude */
   emitComment("Standard prelude:");
   emitRM("LD",mp,0,ac,"load maxaddress from location 0");    /*mp <- dMem[0+reg[ac]]*/
   emitRM("ST",ac,0,ac,"clear location 0");                   /*ac <- dMem[0+reg[ac]]*/
   emitComment("End of standard prelude.");
   /* generate code for TINY program */
   cGen(syntaxTree);
   /* finish */
   emitComment("End of execution.");
   emitRO("HALT",0,0,0,"");
}

/*
$0 --> ac
$1 --> ac1
$6 --> mp
$5 --> gp

tmpOffset de reg[0-4];

*/