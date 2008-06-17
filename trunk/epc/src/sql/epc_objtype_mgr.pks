create or replace package epc_objtype_mgr is

procedure set_persistent
( p_object_id in integer
);

procedure get_object
( p_object_type in varchar2
, p_epc_obj out nocopy epc_objtype
);

procedure set_object
( p_object_type in varchar2
, p_epc_obj in epc_objtype
);

end;
/
