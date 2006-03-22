#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <cairo.h>
#include <cairo-xlib.h>

#define WIDTH  250
#define HEIGHT 250
#define SIZE   20
#define RADIUS 100
#define TICKS  60

void draw_cairo(Display * dpy, Window win, Visual * vis) {
    cairo_surface_t * surface;
    cairo_t * cr;

    XClearWindow(dpy, win);
    surface = cairo_xlib_surface_create(dpy, win, vis, WIDTH, HEIGHT);
    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_save(cr);

    cairo_set_font_size (cr, 20);
    cairo_move_to (cr, 10, 20);
    cairo_show_text (cr, "Clock");
    cairo_stroke(cr);

    cairo_restore(cr);
    cairo_new_path(cr);
    cairo_translate(cr, WIDTH/2, HEIGHT/2);
    cairo_set_line_width(cr, 2);
    cairo_arc(cr, 0, 0, RADIUS, 0, 2 * M_PI);
    for(int x = 0; x < TICKS; ++x) {
        printf("%i * M_PI / 6 = %f\n", x, x * M_PI / 6);
        cairo_move_to(cr, 0, 0);
        if( x % (TICKS/4) == 0 ) {
            cairo_move_to(cr, 0, 80);
        }
        else if( x % (TICKS/12) == 0) {
            cairo_move_to(cr, 0, 90);
        }
        else {
            cairo_move_to(cr, 0, 95);
        }
        cairo_line_to(cr, 0, 100);
        cairo_rotate(cr, 2 * M_PI / TICKS);
    }
    cairo_stroke(cr);
}


int main(int argc, char ** argv) {
    Display * dpy;
    Window root, win;
    Visual * visual;
    int scr;
    XEvent xev;

    dpy    = XOpenDisplay(getenv("DISPLAY"));
    scr    = DefaultScreen(dpy);
    root   = RootWindow(dpy, scr);
    visual = DefaultVisual(dpy, scr);
    win    = XCreateSimpleWindow(dpy, root, 0, 0, WIDTH, HEIGHT, 0, BlackPixel(dpy, scr), BlackPixel(dpy, scr));

    XSelectInput(dpy, win, KeyPressMask|StructureNotifyMask|ExposureMask);
    XMapWindow(dpy, win);

    //cairo stuff here
    draw_cairo(dpy, win, visual);
    //end cairo stuff

    while(1) {
        XNextEvent(dpy, &xev);

        switch(xev.type) {
            case Expose:
                XExposeEvent * eev = &xev.xexpose;
                if (eev->count == 0)
                    draw_cairo(dpy, win, visual);

                break;
        }
    }

    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}
