#include <soundpipe.h>
#include <m_pd.h>

static t_class *thresh_class;

typedef struct _thresh
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_thresh *thresh;
    SPFLOAT *args[2];
    t_float *cable[4];
    int ninputs;
} t_thresh;

static t_int *thresh_perform(t_int *w)
{
    t_thresh *x = (t_thresh *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a + 1] + c);
        }
        sp_thresh_compute(x->sp, x->thresh, (x->cable[0] + c), &tmp);
        *(x->cable[3] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void thresh_dsp(t_thresh *x, t_signal **sp)
{
    dsp_add(thresh_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
    x->cable[3] = sp[3]->s_vec;
}

static void *thresh_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_thresh *x = (t_thresh *)pd_new(thresh_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_thresh_create(&x->thresh);
    sp_thresh_init(x->sp, x->thresh);

    x->args[0] = &x->thresh->thresh;
    x->args[1] = &x->thresh->mode;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 2 - argc;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void thresh_free(t_thresh *x) 
{
    sp_thresh_destroy(&x->thresh);
    sp_destroy(&x->sp);
}

void thresh_tilde_setup(void)
{
    thresh_class = class_new(
            gensym("thresh~"), 
            (t_newmethod)thresh_new, 
            (t_method)thresh_free, 
    	    sizeof(t_thresh), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(thresh_class, t_thresh, x_f);
    class_addmethod(thresh_class, (t_method)thresh_dsp, gensym("dsp"), 0);
}
