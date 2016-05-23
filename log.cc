#include "log.h"

namespace M
{

std::string info_string()
{
    return "\e[1;32m[Info]\e[0m ";
}

std::string warning_string()
{
    return "\e[1;33m[Warning]\e[0m ";
}

std::string error_string()
{
    return "\e[1;31m[Error]\e[0m ";
}

} // namespace M
