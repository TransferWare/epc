--$NO_KEYWORD_EXPANSION$
REMARK
REMARK  $HeadURL$
REMARK

create or replace package body epc as

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

@verify "epc" "package body"
