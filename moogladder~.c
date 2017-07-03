#include <soundpipe.h>
#include <m_pd.h>

static t_class *moogladder_class;

typedef struct _moogladder
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_moogladder *moogladder;
    SPFLOAT *args[2];
    t_float *cable[4];
    int ninputs;
} t_moogladder;

static t_int *moogladder_perform(t_int *w)
{
    t_moogladder *x = (t_moogladder *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a + 1] + c);
        }
        sp_moogladder_compute(x->sp, x->moogladder, (x->cable[0] + c), &tmp);
        *(x->cable[3] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void moogladder_dsp(t_moogladder *x, t_signal **sp)
{
    dsp_add(moogladder_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
    x->cable[3] = sp[3]->s_vec;
}

static void *moogladder_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_moogladder *x = (t_moogladder *)pd_new(moogladder_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_moogladder_create(&x->moogladder);
    sp_moogladder_init(x->sp, x->moogladder);

    x->args[0] = &x->moogladder->freq;
    x->args[1] = &x->moogladder->res;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 2 - argc;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void moogladder_free(t_moogladder *x) 
{
    sp_moogladder_destroy(&x->moogladder);
    sp_destroy(&x->sp);
}

void moogladder_tilde_setup(void)
{
    moogladder_class = class_new(
            gensym("moogladder~"), 
            (t_newmethod)moogladder_new, 
            (t_method)moogladder_free, 
    	    sizeof(t_moogladder), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(moogladder_class, t_moogladder, x_f);
    class_addmethod(moogladder_class, (t_method)moogladder_dsp, gensym("dsp"), 0);
}
