CREATE OR REPLACE TYPE BODY "STD_OBJECT" 
is

final
member procedure store(self in std_object)
is
begin
  std_object_mgr.set_std_object(name(), self);
end store;

final
member procedure remove(self in std_object)
is
begin
  std_object_mgr.del_std_object(name());
end remove;

end;
/

