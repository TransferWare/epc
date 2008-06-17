create or replace package body epc_objtype_mgr is

-- index by object_type
type epc_tabtype is table of epc_objtype index by varchar2(30);

g_epc_tab epc_tabtype;
g_object_id integer := null;

procedure set_persistent
( p_object_id in integer
)
is
begin
  g_object_id := p_object_id;
end set_persistent;

procedure get_object
( p_object_type in varchar2
, p_epc_obj out nocopy epc_objtype
)
is
  pragma autonomous_transaction;
begin
  if g_object_id is not null
  then
    select  value(tab)
    into    p_epc_obj
    from    epc_tab tab
    where   tab.object_id = g_object_id
    and     tab.object_type = p_object_type;

    commit;
  else
    p_epc_obj := g_epc_tab(p_object_type);
  end if;
end get_object;

procedure set_object
( p_object_type in varchar2
, p_epc_obj in epc_objtype
)
is
  pragma autonomous_transaction;
begin
  if g_object_id is not null
  then
    update  epc_tab tab
    set     value(tab) = p_epc_obj
    where   tab.object_id = g_object_id
    and     tab.object_type = p_object_type;

    if sql%rowcount = 0
    then
      insert
      into    epc_tab
      values  p_epc_obj;
    end if;

    commit;
  else
    g_epc_tab(p_object_type) := p_epc_obj;
  end if;
end set_object;

end epc_objtype_mgr;
/

show errors
