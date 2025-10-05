enum su_LogErrorSeverity {
    su_LOG_ERROR_SEVERITY_LOW = 1,
    su_LOG_ERROR_SEVERITY_MEDIUM = 2,
    su_LOG_ERROR_SEVERITY_HIGH = 3,
    su_LOG_ERROR_SEVERITY_CRASH = 4,
};

SA_API void su_log_error(const enum su_LogType type, const enum su_LogErrorSeverity severity, const enum su_LogContext context, const char* message, const char* file, const int line);
