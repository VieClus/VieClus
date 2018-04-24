#include "experimentinfo.h"

#include "logging/log.h"
#include <ctime>            // time() with smallest unit seconds
#include <sys/time.h>       // only UNIX, gettimeofday(), a WINDOWS version can be found in log.h:NowTime()
#include <unistd.h>         // gethostname()

std::shared_ptr<ExperimentInfo> ExperimentInfo::INSTANCE(nullptr);


std::string ExperimentInfo::getHostName()
{
    char hostn[256] = { 0 };     // host name is at most 255 bytes long
    int ret;

    ret = gethostname(hostn, 255);

    THR_EXP_IF(ret != 0, "Problem getting host name.");

    return hostn;
}


double ExperimentInfo::wallTime()
{
    static long zsec =0;
    static long zusec = 0;
    double esec;
    struct timeval tp;
    // timezone is said to be obsolete
    // struct timezone tzp;
    gettimeofday(&tp, 0);

    // only save the first time as reference time
    if(zsec ==0) zsec  = tp.tv_sec;
    if(zusec==0) zusec = tp.tv_usec;

    esec =  ((double) (tp.tv_sec - zsec)) + ((double) (tp.tv_usec-zusec)) * 0.000001;
    return esec;
}


std::string ExperimentInfo::getCurrentDateTime()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[25] = { 0 };

    // save current time
    time(&rawtime);
    timeinfo = localtime (&rawtime);

    // ISO 8601 format with ' ' instead of 'T' between date and time and without "+01" for CEST
    // "2016-09-01T16:28:30
    strftime(buffer, 25, "%Y-%m-%d %H:%M:%S", timeinfo);

    return buffer;
}


ExperimentInfo::ExperimentInfo()
{
    //ctor
}

ExperimentInfo::~ExperimentInfo()
{
    //dtor
}
