--$NO_KEYWORD_EXPANSION$
REMARK
REMARK  $Header$
REMARK

create or replace package body std_object_mgr is

-- index by std_objects.object_name
type std_object_tabtype is table of std_objects%rowtype index by std_objects.object_name%type;

g_std_object_tab std_object_tabtype;

g_group_name std_objects.group_name%type := null;

procedure set_group_name
( p_group_name in std_objects.group_name%type
)
is
begin
  g_group_name := p_group_name;
end set_group_name;

procedure get_anydata
( p_object_name in std_objects.object_name%type
, p_anydata out nocopy sys.anydata
)
is
begin
  if g_group_name is not null
  then
    select  tab.dat
    into    p_anydata
    from    std_objects tab
    where   tab.group_name = g_group_name
    and     tab.object_name = p_object_name;
  else
    p_anydata := g_std_object_tab(p_object_name).dat;
  end if;
end get_anydata;

procedure set_anydata
( p_object_name in std_objects.object_name%type
, p_anydata in sys.anydata
)
is
  pragma autonomous_transaction;
begin
  if g_group_name is not null
  then
    update  std_objects tab
    set     tab.dat = p_anydata
    ,       tab.last_updated_by = user
    ,       tab.last_update_date = sysdate
    where   tab.group_name = g_group_name
    and     tab.object_name = p_object_name;

    if sql%rowcount = 0
    then
      insert
      into    std_objects
      ( group_name
      , object_name
      , created_by
      , creation_date
      , last_updated_by
      , last_update_date
      , dat
      )
      values
      ( g_group_name
      , p_object_name
      , user
      , sysdate
      , user
      , sysdate
      , p_anydata
      );
    end if;

    commit;
  else
    g_std_object_tab(p_object_name).dat := p_anydata;
  end if;
end set_anydata;

procedure get_std_object
( p_object_name in std_objects.object_name%type
, p_std_object out nocopy std_object
)
is
begin
  if g_group_name is not null
  then
    select  tab.obj
    into    p_std_object
    from    std_objects tab
    where   tab.group_name = g_group_name
    and     tab.object_name = p_object_name;
  else
    p_std_object := g_std_object_tab(p_object_name).obj;
  end if;

  p_std_object.dirty := null;
end get_std_object;

procedure set_std_object
( p_object_name in std_objects.object_name%type
, p_std_object in std_object
)
is
  pragma autonomous_transaction;
begin
  if p_std_object.dirty = 0
  then
    null; -- not changed
  elsif g_group_name is not null
  then
    update  std_objects tab
    set     tab.obj = p_std_object
    ,       tab.last_updated_by = user
    ,       tab.last_update_date = sysdate
    where   tab.group_name = g_group_name
    and     tab.object_name = p_object_name;

    if sql%rowcount = 0
    then
      insert
      into    std_objects
      ( group_name
      , object_name
      , created_by
      , creation_date
      , last_updated_by
      , last_update_date
      , obj
      )
      values
      ( g_group_name
      , p_object_name
      , user
      , sysdate
      , user
      , sysdate
      , p_std_object
      );
    end if;

    commit;
  else
    g_std_object_tab(p_object_name).obj := p_std_object;
  end if;
end set_std_object;

procedure delete_std_objects
( p_group_name in std_objects.group_name%type default '%'
, p_object_name in std_objects.object_name%type default '%'
)
is
  pragma autonomous_transaction;

  l_object_name std_objects.object_name%type;
  l_object_name_prev std_objects.object_name%type;
begin
  if p_object_name is null
  then
    raise value_error;
  elsif p_group_name is not null
  then
    delete
    from    std_objects tab
    where   tab.group_name like p_group_name
    and     tab.object_name like p_object_name;

    commit;
  else
    l_object_name := g_std_object_tab.first;
    loop
      exit when l_object_name is null;

      /* a delete now may influence the next operation, 
         so first do next and then maybe delete (the previous) */
      l_object_name_prev := l_object_name;
      l_object_name := g_std_object_tab.next(l_object_name);
      if l_object_name_prev like p_object_name
      then
        g_std_object_tab.delete(l_object_name_prev);
      end if;
    end loop;
  end if;
end delete_std_objects;

end std_object_mgr;
/

show errors
