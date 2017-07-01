#include <soundpipe.h>
#include <m_pd.h>

static t_class *tenv_class;

typedef struct _tenv
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_tenv *tenv;
    SPFLOAT *args[3];
    t_float *cable[5];
    int ninputs;
} t_tenv;

static t_int *tenv_perform(t_int *w)
{
    t_tenv *x = (t_tenv *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a + 1] + c);
        }
        sp_tenv_compute(x->sp, x->tenv, (x->cable[0] + c), &tmp);
        *(x->cable[4] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void tenv_dsp(t_tenv *x, t_signal **sp)
{
    dsp_add(tenv_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
    x->cable[3] = sp[3]->s_vec;
    x->cable[4] = sp[4]->s_vec;
}

static void *tenv_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_tenv *x = (t_tenv *)pd_new(tenv_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_tenv_create(&x->tenv);
    sp_tenv_init(x->sp, x->tenv);

    x->args[0] = &x->tenv->atk;
    x->args[1] = &x->tenv->hold;
    x->args[2] = &x->tenv->rel;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 3 - argc;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void tenv_free(t_tenv *x) 
{
    sp_tenv_destroy(&x->tenv);
    sp_destroy(&x->sp);
}

void tenv_tilde_setup(void)
{
    tenv_class = class_new(
            gensym("tenv~"), 
            (t_newmethod)tenv_new, 
            (t_method)tenv_free, 
    	    sizeof(t_tenv), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(tenv_class, t_tenv, x_f);
    class_addmethod(tenv_class, (t_method)tenv_dsp, gensym("dsp"), 0);
}
