create or replace package body epc as

function data_type_xml
return data_type_subtype
is
begin
  return 7;
end data_type_xml;

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

procedure print(p_msg in varchar2)
is
  l_idx pls_integer;
  l_len pls_integer;
  l_end pls_integer;
  l_line_size constant pls_integer := 255;
begin
  l_idx := 1;
  l_len := length(p_msg);

  while (l_idx <= l_len)
  loop
    l_end := instr(p_msg, chr(10), l_idx);
    if l_end = 0
    then
      l_end := l_len;
    end if;

    while l_idx + l_line_size < l_end
    loop
      dbms_output.put_line(substr(p_msg, l_idx, l_line_size));
      l_idx := l_idx + l_line_size;
    end loop;
    dbms_output.put_line(substr(p_msg, l_idx, l_end - l_idx));
    l_idx := l_end + 1;
  end loop;
end print;

end epc;
/
