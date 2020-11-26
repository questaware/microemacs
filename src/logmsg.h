
#define Q_LOG_DEBUG 1
#define Q_LOG_LOG   2
#define Q_LOG_INFO  3
#define Q_LOG_WARN  4
#define Q_LOG_ERR   5
#define Q_LOG_SEV   6
#define Q_LOG_FATAL 7


#if LOGGING_ON == 0

#define log_init(a,b,c)

#define logdebug(s)
#define logdebug1(s,a)
#define logdebug2(s,a,b)
#define logdebug3(s,a,b,c)
#define logdebug4(s,a,b,c,d)

#define loglog(s)
#define loglog1(s,a)
#define loglog2(s,a,b)
#define loglog3(s,a,b,c)
#define loglog4(s,a,b,c,d)

#define loginfo(s)
#define loginfo1(s,a)
#define loginfo2(s,a,b)
#define loginfo3(s,a,b,c)
#define loginfo4(s,a,b,c,d)

#define logwarn(s)
#define logwarn1(s,a)
#define logwarn2(s,a,b)
#define logwarn3(s,a,b,c)
#define logwarn4(s,a,b,c,d)

#define logerr(s)
#define logerr1(s,a)
#define logerr2(s,a,b)
#define logerr3(s,a,b,c)
#define logerr4(s,a,b,c,d)

#else

#ifdef __cplusplus
extern "C" {
#endif


void log_init(char * filename, int filesize, int severity);

void logmsg( int severity, const char * format, ... );
void logmsg0( int severity, const char * format);

#ifdef __cplusplus
};
#endif


#if NODEBUG_LO

#define logdebug(s)
#define logdebug1(s,a)
#define logdebug2(s,a,b)
#define logdebug3(s,a,b,c)
#define logdebug4(s,a,b,c,d)

#else
#define logdebug(s)          logmsg(Q_LOG_DEBUG, s)
#define logdebug1(s,a)       logmsg(Q_LOG_DEBUG, s,a)
#define logdebug2(s,a,b)     logmsg(Q_LOG_DEBUG, s,a,b)
#define logdebug3(s,a,b,c)   logmsg(Q_LOG_DEBUG, s,a,b,c)
#define logdebug4(s,a,b,c,d) logmsg(Q_LOG_DEBUG, s,a,b,c,d)
#endif


#define loglog(s)          logmsg(Q_LOG_LOG, s)
#define loglog1(s,a)       logmsg(Q_LOG_LOG, s,a)
#define loglog2(s,a,b)     logmsg(Q_LOG_LOG, s,a,b)
#define loglog3(s,a,b,c)   logmsg(Q_LOG_LOG, s,a,b,c)
#define loglog4(s,a,b,c,d) logmsg(Q_LOG_LOG, s,a,b,c,d)

#define loginfo(s)          logmsg(Q_LOG_INFO, s)
#define loginfo1(s,a)       logmsg(Q_LOG_INFO, s,a)
#define loginfo2(s,a,b)     logmsg(Q_LOG_INFO, s,a,b)
#define loginfo3(s,a,b,c)   logmsg(Q_LOG_INFO, s,a,b,c)
#define loginfo4(s,a,b,c,d) logmsg(Q_LOG_INFO, s,a,b,c,d)

#define logwarn(s)          logmsg(Q_LOG_WARN, s)
#define logwarn1(s,a)       logmsg(Q_LOG_WARN, s,a)
#define logwarn2(s,a,b)     logmsg(Q_LOG_WARN, s,a,b)
#define logwarn3(s,a,b,c)   logmsg(Q_LOG_WARN, s,a,b,c)
#define logwarn4(s,a,b,c,d) logmsg(Q_LOG_WARN, s,a,b,c,d)

#define logerr(s)          logmsg(Q_LOG_ERR, s)
#define logerr1(s,a)       logmsg(Q_LOG_ERR, s,a)
#define logerr2(s,a,b)     logmsg(Q_LOG_ERR, s,a,b)
#define logerr3(s,a,b,c)   logmsg(Q_LOG_ERR, s,a,b,c)
#define logerr4(s,a,b,c,d) logmsg(Q_LOG_ERR, s,a,b,c,d)


#endif
