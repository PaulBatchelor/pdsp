#include <soundpipe.h>
#include <m_pd.h>
#define FLT_EPSILON 1.1920928955078125e-07F
#define EPS FLT_EPSILON

static t_class *round_class;
static const SPFLOAT toint = 1/EPS;

typedef struct _round
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    t_float *cable[2];
} t_round;

static SPFLOAT sproundf(SPFLOAT x)
{
	union {SPFLOAT f; uint32_t i;} u = {x};
	int e = u.i >> 23 & 0xff;
	SPFLOAT y;

	if (e >= 0x7f+23)
		return x;
	if (u.i >> 31)
		x = -x;
	if (e < 0x7f-1) {
        /* TODO: I don't understand this */
		/* FORCE_EVAL(x + toint); */
		return 0*u.f;
	}
	y = x + toint - toint - x;
	if (y > 0.5f)
		y = y + x - 1;
	else if (y <= -0.5f)
		y = y + x + 1;
	else
		y = y + x;
	if (u.i >> 31)
		y = -y;
	return y;
}

static t_int *round_perform(t_int *w)
{
    t_round *x = (t_round *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        *(x->cable[1] + c) = sproundf(*(x->cable[0] + c));
        c++;
    }

    return (w+3);
}

static void round_dsp(t_round *x, t_signal **sp)
{
    dsp_add(round_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
}

static void *round_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_round *x = (t_round *)pd_new(round_class);
    x->x_f = 0;
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void round_free(t_round *x) 
{
}

void round_tilde_setup(void)
{
    round_class = class_new(
            gensym("round~"), 
            (t_newmethod)round_new, 
            (t_method)round_free, 
    	    sizeof(t_round), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(round_class, t_round, x_f);
    class_addmethod(round_class, (t_method)round_dsp, gensym("dsp"), 0);
}
