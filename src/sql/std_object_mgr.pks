create or replace package std_object_mgr is

procedure set_group_name
( p_group_name in std_objects.group_name%type
);

procedure get_std_object
( p_object_name in varchar2
, p_epc_obj out nocopy std_object
);

procedure set_std_object
( p_object_name in varchar2
, p_epc_obj in std_object
);

end;
/
