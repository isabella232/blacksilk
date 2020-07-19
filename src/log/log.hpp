#ifndef LOG_H
#define LOG_H

#include <string>
#ifdef QT_CORE_LIB
#include <QDebug>
#endif // QT_CORE_LIB

#define LEVEL_TRACE   " Trace  "
#define LEVEL_DEBUG   " Debug  "
#define LEVEL_INFO    " Info   "
#define LEVEL_WARNING " Warning"
#define LEVEL_ERROR   " Error  "

#define LOG(A)         logging::writeLog( LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, A )
#define LOGB(B,A)      if(B){ logging::writeLog( LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, A );}
#ifdef _DEBUG
#define LOG_DEBUG(A)   logging::writeLog( LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, A )
#define LOGB_DEBUG(B,A)if(B){ logging::writeLog( LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, A );}
#else
#define LOG_DEBUG(A)
#endif
#define LOG_INFO(A)         logging::writeLog( LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, A )
#define LOGB_INFO(B,A)      if(B){ logging::writeLog( LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, A );}
#define LOG_WARNING(A)      logging::writeLog( LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, A )
#define LOGB_WARNING(B,A)   if(B){ logging::writeLog( LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, A );}
#define LOGB_RETURN(B,A,C)  if(B){ logging::writeLog( LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, A ); return C; }
#define LOG_ERROR(A)        logging::writeLog( LEVEL_ERROR,   __FILE__, __LINE__, __FUNCTION__, A )
#define LOGB_ERROR(B,A)     if(B){ logging::writeLog( LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, A );}
#define LOGB_ASSERT(B,A)    if(!(B)){ logging::writeLog( LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, A ); assert(B);}
#define TRACE               logging::writeLog( LEVEL_TRACE,   __FILE__, __LINE__, __FUNCTION__, "TRACE" );
#define LOGQT(TYPE,A,FILE,LINE,FUNC) logging::writeLog( TYPE, FILE, LINE, FUNC, A )
#define CHECK_QT_CONNECT( A ) if( !(A) ) { LOG_WARNING( "QObject::connect() failed" ); }


namespace logging {
bool            writeLog( const char* level, const char* file, int line, const char* function, std::string content );
std::string     logFilename();

#ifdef QT_CORE_LIB
void customMessageHandler( QtMsgType type, const QMessageLogContext& context, const QString& message );
#endif // QT_CORE_LIB
}

#endif // LOG_H
