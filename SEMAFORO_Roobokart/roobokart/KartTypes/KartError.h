#ifndef __PLT_ERRORS_H__
#define __PLT_ERRORS_H__

#include "mbed.h"
#include "component.h"

class KartError : private Component
{
    public:
    enum class code_t
    {
        UKNOW = 0,
        DEVICE_NOT_FOUND,
        DEVICE_NOT_SUPPORTED,
        METHOD_NOT_IMPLEMENTED,
        METHOD_INVALID_ARGUMENT,
        OPERATION_TIMEOUT,
        CALIBRATION_FAIL,
    };

    enum class level_t
    {
           SAFE = 0,
        WARNING = 1,
          ERROR = 2
    };

    private:
    code_t code;
    level_t level;

    public:
    KartError(const char * device_name = "")
    : Component(device_name),
      code(code_t::UKNOW), 
      level(level_t::SAFE)
    { }
    
    KartError(const level_t & error_level,
              const code_t & error_code,
              const char * device_name) 
    : Component(device_name),
      code(error_code),
      level(error_level)
    { }
    
    KartError(const KartError & obj) = delete;
    KartError(KartError && obj) = delete;

    KartError & operator=(KartError && obj) = delete;
    KartError & operator=(const KartError & obj)
    {    
        if (level < obj.level)
        {
            name = obj.name;
            code = obj.code;
            level = obj.level;
        }    
        return *this;
    }

    void setError(const code_t & code, const char * name = nullptr)
    {
        if (level < level_t::ERROR)
        {
            this->code = code;
            level = level_t::ERROR;
            if (name) this->name = name;
        }
    }

    void setWarning(const code_t & code, const char * name = nullptr)
    {
        if (level < level_t::WARNING)
        {
            this->code = code;
            level = level_t::WARNING;
            if (name) this->name = name;
        }
    }

    void setSafe()
    {
        code = code_t::UKNOW;
        level = level_t::SAFE;
    }

    inline operator bool() const
    {
        return level > level_t::SAFE;
    }

    inline bool isError() const
    {
        return level == level_t::ERROR;
    }

    inline bool isWarning() const
    {
        return level == level_t::WARNING;
    }

    inline bool isSafe() const
    {
        return level == level_t::SAFE;
    }

    inline const code_t & getCode() const
    {
        return code;
    }

    inline const level_t & getLevel() const
    {
        return level;
    }

    inline const char * getDeviceName() const
    {
        return name;
    }

    const char * codeToString() const
    {
        switch (code)
        {
            default: return "Uknow Error";            
        }
    }
};

#endif