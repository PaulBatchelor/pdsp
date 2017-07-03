#include <soundpipe.h>
#include <m_pd.h>

static t_class *port_class;

typedef struct _port
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    sp_port *port;
    SPFLOAT *args[1];
    t_float *cable[3];
    int ninputs;
} t_port;

static t_int *port_perform(t_int *w)
{
    t_port *x = (t_port *)(w[1]);
    SPFLOAT tmp;
    int n = (int)(w[2]);
    int c = 0;
    int a;
    while (n--) {
        for(a = 0; a < x->ninputs; a++) {
            *x->args[a] = *(x->cable[a + 1] + c);
        }
        sp_port_compute(x->sp, x->port, (x->cable[0] + c), &tmp);
        *(x->cable[2] + c) = tmp;
        c++;
    }

    return (w+3);
}

static void port_dsp(t_port *x, t_signal **sp)
{
    dsp_add(port_perform, 2, x, 
            sp[0]->s_n
            );

    x->cable[0] = sp[0]->s_vec;
    x->cable[1] = sp[1]->s_vec;
    x->cable[2] = sp[2]->s_vec;
}

static void *port_new(t_symbol *s, int argc, t_atom *argv)
{
    int i;
    t_port *x = (t_port *)pd_new(port_class);
    x->x_f = 0;
    sp_create(&x->sp);
    sp_port_create(&x->port);
    if(argc) {
        sp_port_init(x->sp, x->port, atom_getfloatarg(0, argc, argv));
    }

    x->args[0] = &x->port->htime;

    for(i = 0; i < argc; i++) {
        *x->args[i] = atom_getfloatarg(i, argc, argv);
    }
    
    x->ninputs = 1 - argc;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static void port_free(t_port *x) 
{
    sp_port_destroy(&x->port);
    sp_destroy(&x->sp);
}

void port_tilde_setup(void)
{
    port_class = class_new(
            gensym("port~"), 
            (t_newmethod)port_new, 
            (t_method)port_free, 
    	    sizeof(t_port), 
            0, 
            A_GIMME, 
            0);

    CLASS_MAINSIGNALIN(port_class, t_port, x_f);
    class_addmethod(port_class, (t_method)port_dsp, gensym("dsp"), 0);
}
