create or replace package body epc as

function data_type_string
return data_type_subtype
is
begin
  return 1;
end data_type_string;

function data_type_int
return data_type_subtype
is
begin
  return 2;
end data_type_int;

function data_type_long
return data_type_subtype
is
begin
  return 3;
end data_type_long;

function data_type_float
return data_type_subtype
is
begin
  return 4;
end data_type_float;

function data_type_double
return data_type_subtype
is
begin
  return 5;
end data_type_double;

end epc;
/
