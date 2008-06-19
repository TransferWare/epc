--$NO_KEYWORD_EXPANSION$
REMARK
REMARK  $Header$
REMARK

create or replace type std_object as object (
  /*
  -- The dirty flag is used to speed up std_object_mgr.set_std_object.
  -- When std_object_mgr.get_std_object is called, the dirty flag is
  -- set to null.  Now when the application sets it to 0, the
  -- std_object_mgr.set_std_object() will do nothing since it is
  -- assumed not to be dirty. The application should set it to 1 in a
  -- constructor (new is changed is dirty) and modify dirty to 1 if
  -- the object gets changed somewhere else.
  */
  dirty integer

, not instantiable
  member function name(self in std_object)
  return varchar2

, final
  member procedure store(self in std_object)

, final
  member procedure remove(self in std_object)

) not instantiable not final
/

show errors

@verify "std_object" "type"
