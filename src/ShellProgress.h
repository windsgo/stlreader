#pragma once

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <chrono>
#include <sstream>

class ShellProgress
{
    friend ::std::ostream& operator<<(::std::ostream& os, const ShellProgress& shell_progress);
public:
    ShellProgress() = default;
    void reset();

    void setProgress(float progress);
    inline float getProgress() const {return _progress;}

    void setPrintDurationMS(int64_t ms) {_print_dur_ms = ms;}

private:
    mutable bool _print_new_line = true;
    float _progress = 0.0;
    int64_t _print_dur_ms = 50;

    struct ProgressFormatter
    {
        static const char bit_0_char = '-';
        static const char bit_1_char = '*';
        static constexpr uint16_t bit_num = 50;

        static std::string str(float progress) {
            if (progress < 0.0) {
                progress = 0.0;
            } else if (progress > 1.0) {
                progress = 1.0;
            }
            uint16_t bit_1_num = progress * bit_num;

            std::stringstream ss;
            char c[8];
            sprintf(c, "%-3d", (int)(progress * 100));
            ss << '[' << c << "%] [";

            for (int i = 0; i < bit_1_num; ++i)
                ss << bit_1_char;
            
            for (int i = bit_1_num; i < bit_num; ++i)
                ss << bit_0_char;

            ss << ']';

            return ss.str();
        }
    };
    

};

::std::ostream& operator<<(::std::ostream& os, const ShellProgress& shell_progress);