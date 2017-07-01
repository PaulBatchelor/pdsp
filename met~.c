#include <soundpipe.h>
#include <m_pd.h>

static t_class *met_class;

typedef struct _met
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_metro *met;
    SPFLOAT *args[1];
    t_float *cable[2];
    int ninputs;
} t_met;

static t_int *met_perform(t_int *w)
{
    t_met *x = (t_met *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a] + c);
        }
        sp_metro_compute(x->sp, x->met, NULL, &tmp);
        *(x->cable[1] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void met_dsp(t_met *x, t_signal **sp)
{
    dsp_add(met_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
}

static void *met_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_met *x = (t_met *)pd_new(met_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_metro_create(&x->met);
    sp_metro_init(x->sp, x->met);

    x->args[0] = &x->met->freq;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 1 - argc;
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void met_free(t_met *x) 
{
    sp_metro_destroy(&x->met);
    sp_destroy(&x->sp);
}

void met_tilde_setup(void)
{
    met_class = class_new(
            gensym("met~"), 
            (t_newmethod)met_new, 
            (t_method)met_free, 
    	    sizeof(t_met), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(met_class, t_met, x_f);
    class_addmethod(met_class, (t_method)met_dsp, gensym("dsp"), 0);
}
