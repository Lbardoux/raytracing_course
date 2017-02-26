#include "time_core.hpp"
#include <thread>

/* Function that retu */
void get_time(struct timespec * ts)
{
    clock_gettime(CLOCK_MONOTONIC, ts);
}

void timespecDiff(struct timespec *a, struct timespec *b, struct timespec *res)
{
    /* a <= b? */
    if ((a->tv_sec < b->tv_sec) ||
            ((a->tv_sec == b->tv_sec) && (a->tv_nsec <= b->tv_nsec)))
    {
        res->tv_sec = res->tv_nsec = 0 ;
    }
    /* a > b */
    else
    {
        res->tv_sec = a->tv_sec - b->tv_sec ;
        if (a->tv_nsec < b->tv_nsec)
        {
            res->tv_nsec = a->tv_nsec + 1000000000L - b->tv_nsec ;
            res->tv_sec-- ;             /* Borrow a second. */
        }
        else
            res->tv_nsec = a->tv_nsec - b->tv_nsec ;
    }
}

void repeatChar(int nb, char c)
{
    int i;
    for(i = 0; i < nb; ++i)
        fprintf(stdout, "%c", c);
}

struct timespec timeStart, timeEnd, timeRes;
std::string funcName;


const int NB_DOT_MAX = 5;
bool canPrintRendering;
std::thread rendering;

void printRendering()
{
    int nb_dots = 1;

    while(canPrintRendering)
    {
        repeatChar(80, '\b');
        fprintf(stdout, "Rendering");
        repeatChar(nb_dots, '.');
        repeatChar(NB_DOT_MAX - nb_dots, ' ');


        nb_dots = (nb_dots % NB_DOT_MAX) + 1;
        fflush(stdout);
        usleep(1000000 - 100);
    }
    fprintf(stdout, "\n");
}

void timeBeginFunc(const std::string& func_name)
{
    std::cout << "Début de la fonction : " << func_name << std::endl;
    funcName = func_name;
    canPrintRendering = true;

    rendering = std::thread(printRendering);

    get_time(&timeStart);
}

void timeEndFunc()
{
    canPrintRendering = false;
    get_time(&timeEnd);
}

void timePrint()
{
    timespecDiff(&timeEnd, &timeStart, &timeRes);
    printf("Fonction \"%s\" : %ld.%09ld secondes\n", funcName.c_str(), (long int)timeRes.tv_sec, (long int)timeRes.tv_nsec);
}