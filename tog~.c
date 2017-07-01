#include <soundpipe.h>
#include <m_pd.h>

static t_class *tog_class;

typedef struct _tog
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    t_float *cable[2];
    SPFLOAT tog;
} t_tog;

static t_int *tog_perform(t_int *w)
{
    t_tog *x = (t_tog *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        if(*(x->cable[0] + c) != 0) {
            x->tog = (x->tog == 1 ? 0 : 1);
        }
        *(x->cable[1] + c) = x->tog;
        c++;
    }

    return (w+3);
}

static void tog_dsp(t_tog *x, t_signal **sp)
{
    dsp_add(tog_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
}

static void *tog_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_tog *x = (t_tog *)pd_new(tog_class);
    x->x_f = 0;
    x->tog = 0;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void tog_free(t_tog *x) 
{
}

void tog_tilde_setup(void)
{
    tog_class = class_new(
            gensym("tog~"), 
            (t_newmethod)tog_new, 
            (t_method)tog_free, 
    	    sizeof(t_tog), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(tog_class, t_tog, x_f);
    class_addmethod(tog_class, (t_method)tog_dsp, gensym("dsp"), 0);
}
