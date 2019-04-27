#include <libguile.h>

int main()
{
	scm_init_guile();
	scm_c_eval_string("(display \"hello world\") (newline)");
	scm_gc(); // still seems to leak memory even if this is included
}
