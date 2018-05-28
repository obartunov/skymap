#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *add_obj_button(char *alpha_s, char *delta_s, char *objname)
{
    char *button_script;
    button_script=(char *)malloc(sizeof(char)*(128+strlen(alpha_s)+strlen(delta_s)+strlen(objname)));
    sprintf(button_script,"<input type=\"button\" value=\"ADD\" onclick=\"window.opener.opener.add_to_list('%s %s %s\\n'); return false\">",
           alpha_s, delta_s, objname);
    return button_script;
}
