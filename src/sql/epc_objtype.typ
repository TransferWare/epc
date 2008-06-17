create or replace type epc_objtype as object (
  object_id integer        -- key part 1
, object_type varchar2(30) -- key part 2
) not instantiable not final
/

show errors
