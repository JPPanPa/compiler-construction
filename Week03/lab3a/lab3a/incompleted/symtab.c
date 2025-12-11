/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  Type* newType = (Type*) malloc(sizeof(Type));
  newType->typeClass = type->typeClass;
  
  if (type->typeClass == TP_ARRAY) {
    newType->arraySize = type->arraySize;
    newType->elementType = duplicateType(type->elementType);
  }
  
  return newType;
  //TODO
}

int compareType(Type* type1, Type* type2) {
  if (type1->typeClass != type2->typeClass) {
    return 0;
  }
  
  if (type1->typeClass == TP_ARRAY) {
    if (type1->arraySize != type2->arraySize) {
      return 0;
    }
    return compareType(type1->elementType, type2->elementType);
  }
  
  return 1;
  //TODO
}

void freeType(Type* type) {
  if (type == NULL) return;
  
  if (type->typeClass == TP_ARRAY) {
    freeType(type->elementType);
  }
  
  free(type);
  //TODO
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  ConstantValue* constVal = (ConstantValue*)malloc(sizeof(ConstantValue));
  constVal->type = TP_INT;
  constVal->intValue = i;
  return constVal;
  //TODO
}

ConstantValue* makeCharConstant(char ch) {
  ConstantValue* constVal = (ConstantValue*)malloc(sizeof(ConstantValue));
  constVal->type = TP_CHAR;
  constVal->charValue = ch;
  return constVal;
  //TODO
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  ConstantValue* duplicate = (ConstantValue*)malloc(sizeof(ConstantValue));
  duplicate->type = v->type;
  
  switch(v->type) {
    case TP_INT:
      duplicate->intValue = v->intValue;
      break;
    case TP_CHAR:
      duplicate->charValue = v->charValue;
      break;
  }
  
  return duplicate;
  //TODO
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {
  Object* constObj = (Object*) malloc(sizeof(Object));
  strcpy(constObj->name, name);
  constObj->kind = OBJ_CONSTANT;
  constObj->constAttrs = (ConstantAttributes*) malloc(sizeof(ConstantAttributes));
  return constObj;
  //TODO
}

Object* createTypeObject(char *name) {
  Object* typeObj = (Object*) malloc(sizeof(Object));
  strcpy(typeObj->name, name);
  typeObj->kind = OBJ_TYPE;
  typeObj->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  return typeObj;
  //TODO
}

Object* createVariableObject(char *name) {
  Object* varObj = (Object*) malloc(sizeof(Object));
  strcpy(varObj->name, name);
  varObj->kind = OBJ_VARIABLE;
  
  VariableAttributes* attrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  attrs->scope = symtab->currentScope;
  varObj->varAttrs = attrs;
  
  return varObj;
  //TODO
}

Object* createFunctionObject(char *name) {
  Object* funcObj = (Object*) malloc(sizeof(Object));
  strcpy(funcObj->name, name);
  funcObj->kind = OBJ_FUNCTION;
  
  FunctionAttributes* attrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  attrs->paramList = NULL;
  attrs->scope = createScope(funcObj, symtab->currentScope);
  funcObj->funcAttrs = attrs;
  
  return funcObj;
  //TODO
}

Object* createProcedureObject(char *name) {
  Object* procObj = (Object*) malloc(sizeof(Object));
  strcpy(procObj->name, name);
  procObj->kind = OBJ_PROCEDURE;
  
  ProcedureAttributes* attrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  attrs->paramList = NULL;
  attrs->scope = createScope(procObj, symtab->currentScope);
  procObj->procAttrs = attrs;
  
  return procObj;
  //TODO
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  Object* paramObj = (Object*) malloc(sizeof(Object));
  strcpy(paramObj->name, name);
  paramObj->kind = OBJ_PARAMETER;
  
  ParameterAttributes* attrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  attrs->kind = kind;
  attrs->function = owner;
  paramObj->paramAttrs = attrs;
  
  return paramObj;
  //TODO
}

void freeObject(Object* obj) {
  if (obj == NULL) return;
  
  switch (obj->kind) {
    case OBJ_CONSTANT:
      if (obj->constAttrs != NULL) {
        free(obj->constAttrs->value);
        free(obj->constAttrs);
      }
      break;
      
    case OBJ_TYPE:
      if (obj->typeAttrs != NULL) {
        free(obj->typeAttrs->actualType);
        free(obj->typeAttrs);
      }
      break;
      
    case OBJ_VARIABLE:
      if (obj->varAttrs != NULL) {
        free(obj->varAttrs->type);
        free(obj->varAttrs);
      }
      break;
      
    case OBJ_FUNCTION:
      if (obj->funcAttrs != NULL) {
        freeReferenceList(obj->funcAttrs->paramList);
        freeType(obj->funcAttrs->returnType);
        freeScope(obj->funcAttrs->scope);
        free(obj->funcAttrs);
      }
      break;
      
    case OBJ_PROCEDURE:
      if (obj->procAttrs != NULL) {
        freeReferenceList(obj->procAttrs->paramList);
        freeScope(obj->procAttrs->scope);
        free(obj->procAttrs);
      }
      break;
      
    case OBJ_PROGRAM:
      if (obj->progAttrs != NULL) {
        freeScope(obj->progAttrs->scope);
        free(obj->progAttrs);
      }
      break;
      
    case OBJ_PARAMETER:
      if (obj->paramAttrs != NULL) {
        freeType(obj->paramAttrs->type);
        free(obj->paramAttrs);
      }
      break;
  }
  
  free(obj);
  //TODO
}

void freeScope(Scope* scope) {
  if (scope == NULL) return;
  freeObjectList(scope->objList);
  free(scope);
  //TODO
}

void freeObjectList(ObjectNode *objList) {
  //TODO
  ObjectNode* current = objList;
  ObjectNode* next;
  
  while (current != NULL) {
    next = current->next;
    freeObject(current->object);
    free(current);
    current = next;
  }
}

void freeReferenceList(ObjectNode *objList) {
  //TODO
  ObjectNode* current = objList;
  ObjectNode* next;
  
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  //TODO
  ObjectNode* current = objList;
  
  while (current != NULL) {
    if (strcmp(current->object->name, name) == 0) {
      return current->object;
    }
    current = current->next;
  }
  
  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  addObject(&(symtab->currentScope->objList), obj);
}