CREATE TABLE "STD_OBJECTS" 
   (	"GROUP_NAME" VARCHAR2(100), 
	"OBJECT_NAME" VARCHAR2(100), 
	"CREATED_BY" VARCHAR2(30), 
	"CREATION_DATE" DATE, 
	"LAST_UPDATED_BY" VARCHAR2(30), 
	"LAST_UPDATE_DATE" DATE, 
	"OBJ" "STD_OBJECT" 
   )  DEFAULT COLLATION "USING_NLS_COMP" 
 VARRAY TREAT("OBJ" AS "DBUG_OBJ_T")."ACTIVE_STR_TAB" STORE AS SECUREFILE LOB 
 VARRAY TREAT("OBJ" AS "DBUG_OBJ_T")."ACTIVE_NUM_TAB" STORE AS SECUREFILE LOB 
 VARRAY TREAT("OBJ" AS "DBUG_OBJ_T")."CALL_TAB" STORE AS SECUREFILE LOB 
 VARRAY TREAT("OBJ" AS "DBUG_OBJ_T")."BREAK_POINT_LEVEL_STR_TAB" STORE AS SECUREFILE LOB 
 VARRAY TREAT("OBJ" AS "DBUG_OBJ_T")."BREAK_POINT_LEVEL_NUM_TAB" STORE AS SECUREFILE LOB;
