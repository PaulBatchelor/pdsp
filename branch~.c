#include <soundpipe.h>
#include <m_pd.h>

static t_class *branch_class;

typedef struct _branch
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    t_float *cable[4];
} t_branch;

static t_int *branch_perform(t_int *w)
{
    t_branch *x = (t_branch *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    while (n--) {
        if(*(x->cable[0] + c)) {
            *(x->cable[3] + c) = *(x->cable[1] + c);
        } else {
            *(x->cable[3] + c) = *(x->cable[2] + c);
        }
        c++;
    }

    return (w+3);
}

static void branch_dsp(t_branch *x, t_signal **sp)
{
    dsp_add(branch_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
    x->cable[3] = sp[3]->s_vec;
}

static void *branch_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_branch *x = (t_branch *)pd_new(branch_class);
    x->x_f = 0;

    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void branch_free(t_branch *x) 
{
}

void branch_tilde_setup(void)
{
    branch_class = class_new(
            gensym("branch~"), 
            (t_newmethod)branch_new, 
            (t_method)branch_free, 
    	    sizeof(t_branch), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(branch_class, t_branch, x_f);
    class_addmethod(branch_class, (t_method)branch_dsp, gensym("dsp"), 0);
}
