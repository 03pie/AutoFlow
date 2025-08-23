/******************************************************************************
 * @File     : GeneralDefine.h
 * @Brief    : General definitions header file
 * @Author   : 03pei
 * @Date     : 2025-08-22
 * @Version  : 1.0
 * @Note     :
 *      1. general_logs_path   Path to the general log file
 *      2. general_temp_path    Path to the general temporary file
 *      3. general_config_path  Path to the general configuration file
 * @Include  :
 *      - <string>
 ******************************************************************************/
#pragma once

#include <string>

namespace GeneralDefine
{
    const std::string general_logs_path = "log/logs.txt"; // 通用log文件路径
}