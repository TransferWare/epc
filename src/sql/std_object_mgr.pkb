create or replace package body std_object_mgr is

-- index by object_name
type std_object_tabtype is table of std_object index by varchar2(30);

g_std_object_tab std_object_tabtype;
g_group_name integer := null;

procedure set_group_name
( p_group_name in std_objects.group_name%type
)
is
begin
  g_group_name := p_group_name;
end set_group_name;

procedure get_std_object
( p_object_name in varchar2
, p_std_object out nocopy std_object
)
is
  pragma autonomous_transaction;
begin
  if g_group_name is not null
  then
    select  tab.obj
    into    p_std_object
    from    std_objects tab
    where   tab.group_name = g_group_name
    and     tab.obj.object_name = p_object_name;

    commit;
  else
    p_std_object := g_std_object_tab(p_object_name);
  end if;
end get_std_object;

procedure set_std_object
( p_object_name in varchar2
, p_std_object in std_object
)
is
  pragma autonomous_transaction;
begin
  if g_group_name is not null
  then
    update  std_objects tab
    set     tab.obj = p_std_object
    ,       tab.last_updated_by = user
    ,       tab.last_update_date = sysdate
    where   tab.group_name = g_group_name
    and     tab.obj.object_name = p_object_name;

    if sql%rowcount = 0
    then
      insert
      into    std_objects
      ( group_name
      , created_by
      , creation_date
      , last_updated_by
      , last_update_date
      , obj
      )
      values
      ( g_group_name
      , user
      , sysdate
      , user
      , sysdate
      , p_std_object
      );
    end if;

    commit;
  else
    g_std_object_tab(p_object_name) := p_std_object;
  end if;
end set_std_object;

end std_object_mgr;
/

show errors
