#include <soundpipe.h>
#include <m_pd.h>

static t_class *saw_class;

typedef struct _saw
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_blsaw *saw;
    SPFLOAT *args[2];
    t_float *cable[3];
    int ninputs;
} t_saw;

static t_int *saw_perform(t_int *w)
{
    t_saw *x = (t_saw *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a] + c);
        }
        sp_blsaw_compute(x->sp, x->saw, NULL, &tmp);
        *(x->cable[2] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void saw_dsp(t_saw *x, t_signal **sp)
{
    dsp_add(saw_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
}

static void *saw_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_saw *x = (t_saw *)pd_new(saw_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_blsaw_create(&x->saw);
    sp_blsaw_init(x->sp, x->saw);

    x->args[0] = x->saw->freq;
    x->args[1] = x->saw->amp;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 2 - argc;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void saw_free(t_saw *x) 
{
    sp_blsaw_destroy(&x->saw);
    sp_destroy(&x->sp);
}

void saw_tilde_setup(void)
{
    saw_class = class_new(
            gensym("saw~"), 
            (t_newmethod)saw_new, 
            (t_method)saw_free, 
    	    sizeof(t_saw), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(saw_class, t_saw, x_f);
    class_addmethod(saw_class, (t_method)saw_dsp, gensym("dsp"), 0);
}
