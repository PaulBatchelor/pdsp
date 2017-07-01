#include <soundpipe.h>
#include <m_pd.h>

static t_class *sine_class;

typedef struct _sine
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_ftbl *ft;
    sp_osc *osc;
    SPFLOAT *args[2];
    t_float *cable[3];
    int ninputs;
} t_sine;

static t_int *sine_perform(t_int *w)
{
    t_sine *x = (t_sine *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a] + c);
        }
        sp_osc_compute(x->sp, x->osc, NULL, &tmp);
        *(x->cable[2] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void sine_dsp(t_sine *x, t_signal **sp)
{
    dsp_add(sine_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
}

static void *sine_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_sine *x = (t_sine *)pd_new(sine_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_ftbl_create(x->sp, &x->ft, 8192);
    sp_gen_sine(x->sp, x->ft);
    sp_osc_create(&x->osc);
    sp_osc_init(x->sp, x->osc, x->ft, 0);

    x->args[0] = &x->osc->freq;
    x->args[1] = &x->osc->amp;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 2 - argc;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void sine_free(t_sine *x) 
{
    sp_ftbl_destroy(&x->ft);
    sp_osc_destroy(&x->osc);
    sp_destroy(&x->sp);
}

void sine_tilde_setup(void)
{
    sine_class = class_new(
            gensym("sine~"), 
            (t_newmethod)sine_new, 
            (t_method)sine_free, 
    	    sizeof(t_sine), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(sine_class, t_sine, x_f);
    class_addmethod(sine_class, (t_method)sine_dsp, gensym("dsp"), 0);
}
