/******************************************************************************
 * @File     : GeneralDefine.h
 * @Brief    : General definitions header file
 * @Author   : 03pei
 * @Date     : 2025-08-22
 * @Version  : 1.0
 * @Include  :
 *      - <string>
 ******************************************************************************/
#pragma once

#include <string>

namespace GeneralDefine
{
#if defined(_WIN32) || defined(_WIN64)
#define SHARED_LIB_SUFFIX ".dll"
#elif defined(__linux__)
#define SHARED_LIB_SUFFIX ".so"
#elif defined(__APPLE__)
#define SHARED_LIB_SUFFIX ".dylib"
#else
#define SHARED_LIB_SUFFIX ""
#endif

    const std::string general_logs_path = "log/logs.txt"; // 通用log文件路径
}